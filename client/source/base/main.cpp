#include "pch.h"
#include "main.h"
#include "services/service_manager.h"
#include "services/render_service.h"
#include "services/input_service.h"
#include "services/tracker_service.h"
#include <curl/curl.h>


#ifdef __APPLE__
#import <Foundation/Foundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif
#endif

#ifdef __ANDROID__
#include <android_native_app_glue.h>
#include <android/window.h>
extern struct android_app* __state;
#endif



DfgGame::DfgGame()
    : _hyperKeyPressed(false)
{
#ifdef __EMSCRIPTEN__
    _hasIndexedDB = EM_ASM_INT_V({
        return typeof window === 'object' && (window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB) ? 1 : 0;
    });
#endif
}

void DfgGame::initialize()
{
#ifndef __EMSCRIPTEN__
    // initialize curl before any services get created
    curl_global_init(CURL_GLOBAL_ALL);
#endif

    // create services
    _inputService = ServiceManager::getInstance()->registerService< InputService >(NULL);

    Service * render_dep[] = { _inputService, NULL };
    _renderService = ServiceManager::getInstance()->registerService< RenderService >(render_dep);

    setVsync(false);

    _userFolder = getAppPrivateFolderPath();
#ifdef WIN32
    _mkdir(_userFolder.c_str());
#else
    mkdir( _userFolder.c_str( ), 0777 );
#endif
    _userFolder += "/Dream Farm Games";
#ifdef WIN32
    _mkdir(_userFolder.c_str());
#else
    mkdir( _userFolder.c_str( ), 0777 );
#endif
    _userFolder += "/";
    _userFolder += getConfig()->getNamespace("window", true)->getString("title");
#ifdef WIN32
    _mkdir(_userFolder.c_str());
#else
    mkdir( _userFolder.c_str( ), 0777 );
#endif

    // set default resources locale
    _gameLocale = getConfig()->getString("language");
    setGameLocale();

#ifdef __APPLE__
#if TARGET_OS_IPHONE
    [NSNotificationCenter.defaultCenter addObserverForName:UIKeyboardDidShowNotification object:nil queue:nil usingBlock:^(NSNotification *note)
     {
         CGSize keyboardSize = [note.userInfo[UIKeyboardFrameEndUserInfoKey] CGRectValue].size;
         ServiceManager::getInstance()->signals.virtualKeyboardSizeChanged(keyboardSize.width * [[UIScreen mainScreen] scale], keyboardSize.height * [[UIScreen mainScreen] scale]);
     }];
    [NSNotificationCenter.defaultCenter addObserverForName:UIKeyboardDidHideNotification object:nil queue:nil usingBlock:^(NSNotification *note)
     {
         ServiceManager::getInstance()->signals.virtualKeyboardSizeChanged(0.0f, 0.0f);
     }];
#endif
#endif
}

void DfgGame::setGameLocale(const char * newLocale)
{
    std::string systemLocale(_gameLocale);
    if (newLocale == NULL)
    {
        // get system-wide locale and override _gameLocale if we have resources for it
#ifdef __APPLE__
        NSString* preferredLang = [[[NSBundle mainBundle] localizedInfoDictionary] objectForKey:@"GameLanguage"];
        systemLocale = [preferredLang cStringUsingEncoding : NSASCIIStringEncoding];
#endif

#ifdef __ANDROID__
        android_app* app = __state;
        JNIEnv* env = app->activity->env;
        JavaVM* vm = app->activity->vm;
        vm->AttachCurrentThread(&env, NULL);

        jclass android_content_Context = env->FindClass("android/content/Context");
        jmethodID midGetPackageName = env->GetMethodID(android_content_Context, "getPackageName", "()Ljava/lang/String;");
        jstring packageName = (jstring)env->CallObjectMethod(app->activity->clazz, midGetPackageName);
        jmethodID midGetResources = env->GetMethodID(android_content_Context, "getResources", "()Landroid/content/res/Resources;");
        jobject jResource = env->CallObjectMethod(app->activity->clazz, midGetResources);

        jclass resource_Class = env->GetObjectClass(jResource);
        jmethodID midGetIdentifier = env->GetMethodID(resource_Class, "getIdentifier", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
        jstring languageVar = env->NewStringUTF("GameLanguage");
        jstring TypeName = env->NewStringUTF("string");
        jint id = env->CallIntMethod(jResource, midGetIdentifier, languageVar, TypeName, packageName);

        jmethodID midGetAppName = env->GetMethodID(resource_Class, "getString", "(I)Ljava/lang/String;");
        jstring language = (jstring)env->CallObjectMethod(jResource, midGetAppName, id);

        const char * lng = env->GetStringUTFChars(language, NULL);
        if (lng)
            systemLocale = lng;
        env->ReleaseStringUTFChars(language, lng);

        vm->DetachCurrentThread();
#endif

        newLocale = systemLocale.c_str();
    }

    std::string aliasesName("aliases_");
    if (getConfig()->getNamespace((aliasesName + newLocale).c_str(), true))
        _gameLocale = newLocale;

    gameplay::FileSystem::loadResourceAliases(getConfig()->getNamespace((aliasesName + _gameLocale).c_str(), true));
}

void DfgGame::finalize()
{
    ServiceManager::getInstance()->shutdown();
    Caches::getInstance()->destroyAll();

#ifndef __EMSCRIPTEN__
    // free any curl resources after it's no longer used.
    curl_global_cleanup();
#endif
}

void DfgGame::update(float elapsedTime)
{
    ServiceManager::getInstance()->update(elapsedTime);
}

void DfgGame::render(float /*elapsedTime*/)
{
    if (_renderService)
    {
        ServiceManager::getInstance()->signals.framePreRender();
        _renderService->renderFrame();
        ServiceManager::getInstance()->signals.framePostRender();
    }
}

void DfgGame::keyEvent(gameplay::Keyboard::KeyEvent evt, int key, bool processed)
{
    if (_inputService)
    {
        if (processed)
            _inputService->injectKeyGlobalEvent(evt, key);
        else
            _inputService->injectKeyEvent(evt, key);
    }

    if (key == gameplay::Keyboard::KEY_HYPER)
        _hyperKeyPressed = evt == gameplay::Keyboard::KEY_PRESS;
    else if (_hyperKeyPressed && evt == gameplay::Keyboard::KEY_PRESS &&
        (key == gameplay::Keyboard::KEY_CAPITAL_Q || key == gameplay::Keyboard::KEY_Q))
    {
        exit();
    }
}

void DfgGame::touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex, bool processed)
{
    if (!_inputService)
        return;

    if (processed)
        _inputService->injectTouchGlobalEvent(evt, x, y, contactIndex);
    else
        _inputService->injectTouchEvent(evt, x, y, contactIndex);
}

bool DfgGame::mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, float wheelDelta, bool processed)
{
    if (!_inputService)
        return false;

    if (processed)
        return _inputService->injectMouseGlobalEvent(evt, x, y, wheelDelta);
    return _inputService->injectMouseEvent(evt, x, y, wheelDelta);
}

void DfgGame::gesturePinchEvent(int x, int y, float scale, int numberOfTouches)
{
    if (_inputService)
        _inputService->injectGesturePinchEvent(x, y, scale, numberOfTouches);
}

void DfgGame::gestureSwipeEvent(int x, int y, int direction)
{
    if (_inputService)
        _inputService->injectGestureSwipeEvent(x, y, direction);
}

void DfgGame::pause()
{
    ServiceManager::getInstance()->signals.pauseEvent();

#ifndef _DEBUG
    TrackerService * trackerService = ServiceManager::getInstance()->findService< TrackerService >();
    if (trackerService)
        trackerService->endSession("Pause");

    gameplay::Game::pause();
#endif
}

void DfgGame::resume()
{
#ifndef _DEBUG
    gameplay::Game::resume();
#endif

    ServiceManager::getInstance()->signals.resumeEvent();
}

void DfgGame::reportError(bool isFatal, const char * errorMessage, ...)
{
    va_list args;
    va_start(args, errorMessage);

    char exceptionDesc[1024];
    vsnprintf(exceptionDesc, 1023, errorMessage, args);
    exceptionDesc[1023] = '\0';

    if (isFatal)
    {
        // notify user
#ifdef WIN32
        MessageBoxA(NULL, exceptionDesc, "Critical Error", MB_OK | MB_ICONERROR);
#endif
    }

    TrackerService * trackerService = ServiceManager::getInstance()->findService< TrackerService >();
    if (!trackerService)
        return;

    exceptionDesc[149] = '\0';
    trackerService->sendException(exceptionDesc, isFatal);

    if (isFatal)
        trackerService->endSession();
}

void DfgGame::scheduleLocalNotification(time_t datetime, const char * utf8Body, const char * utf8ActionButton, int badgeNumber, const std::unordered_map< std::string, std::string >& userDictionary)
{
#ifdef __APPLE__
#if TARGET_OS_IPHONE

    if ([[UIApplication sharedApplication] respondsToSelector:@selector(registerUserNotificationSettings:)])
    {
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert|UIUserNotificationTypeBadge|UIUserNotificationTypeSound categories:nil]];
    }

    UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    if (localNotif == nil)
        return;

    localNotif.fireDate = [NSDate dateWithTimeIntervalSince1970:datetime];
    localNotif.timeZone = [NSTimeZone defaultTimeZone];

    localNotif.alertBody = [NSString stringWithUTF8String:utf8Body];
    localNotif.alertAction = [NSString stringWithUTF8String:utf8ActionButton];

    localNotif.soundName = UILocalNotificationDefaultSoundName;
    localNotif.applicationIconBadgeNumber = badgeNumber;

    NSMutableDictionary *infoDict = [NSMutableDictionary dictionaryWithCapacity:userDictionary.size( )];

    for( std::unordered_map< std::string, std::string >::const_iterator it = userDictionary.begin( ), end_it = userDictionary.end( ); it != end_it; it++ )
        [infoDict setObject:[NSString stringWithUTF8String:( *it ).second.c_str( )] forKey:[NSString stringWithUTF8String:( *it ).first.c_str( )]];

    localNotif.userInfo = infoDict;

    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
#endif
#endif
}

void DfgGame::cancelAllLocalNotifications()
{
#ifdef __APPLE__
#if TARGET_OS_IPHONE
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
    [UIApplication sharedApplication].applicationIconBadgeNumber = 0;
#endif
#endif
}

void DfgGame::preventFromSleeping(bool prevent)
{
#if defined(__APPLE__)
#if TARGET_OS_IPHONE
    [UIApplication sharedApplication].idleTimerDisabled = prevent ? YES : NO;
#endif
#elif defined(__ANDROID__)
    ANativeActivity* activity = __state->activity;
    if (prevent)
        ANativeActivity_setWindowFlags(activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
    else
        ANativeActivity_setWindowFlags(activity, 0, AWINDOW_FLAG_KEEP_SCREEN_ON);
#endif
}

void DfgGame::resizeEvent(unsigned int width, unsigned int height)
{
    ServiceManager::getInstance()->signals.resizeEvent(width, height);
}

void DfgGame::copyToClipboard(const char * textUTF8) const
{
#if defined(WIN32)

    std::wstring str(Utils::UTF8ToWCS(textUTF8));
    const wchar_t * wstring = &(*str.begin());
    const size_t len = wcslen(wstring) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
    memcpy(GlobalLock(hMem), wstring, len * sizeof(wchar_t));
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();

#elif defined(__APPLE__)

    UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    pasteboard.string = [NSString stringWithUTF8String:textUTF8];

#elif defined(__ANDROID__)

    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    jclass looperClass = env->FindClass("android/os/Looper");
    jmethodID prepareMethodID = env->GetStaticMethodID(looperClass, "prepare", "()V");
    env->CallStaticVoidMethod(looperClass, prepareMethodID);

    jclass clsContext = env->FindClass("android/content/Context");
    jmethodID getSystemService = env->GetMethodID(clsContext, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jfieldID CLIPBOARD_SERVICE_ID = env->GetStaticFieldID(clsContext, "CLIPBOARD_SERVICE", "Ljava/lang/String;");
    jstring CLIPBOARD_SERVICE = (jstring)env->GetStaticObjectField(clsContext, CLIPBOARD_SERVICE_ID);
    jobject clipboard = env->CallObjectMethod(app->activity->clazz, getSystemService, CLIPBOARD_SERVICE);

    jclass clsClipData = env->FindClass("android/content/ClipData");
    jmethodID methodNewPlainText = env->GetStaticMethodID(clsClipData, "newPlainText", "(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Landroid/content/ClipData;");
    jstring text = env->NewStringUTF(textUTF8);
    jobject clipData = env->CallStaticObjectMethod(clsClipData, methodNewPlainText, text, text);

    jclass clsClipboardManager = env->FindClass("android/content/ClipboardManager");
    jmethodID methodSetPrimaryClip = env->GetMethodID(clsClipboardManager, "setPrimaryClip", "(Landroid/content/ClipData;)V");
    env->CallVoidMethod(clipboard, methodSetPrimaryClip, clipData);

    vm->DetachCurrentThread();

#elif defined (__EMSCRIPTEN__)

    // fallback to document.execCommand('copy') if Clipboard API is not supported
    // see http://stackoverflow.com/questions/25099409/copy-to-clipboard-as-plain-text
    EM_ASM_({
        try
        {
            var copyEvent = new ClipboardEvent('copy', { dataType: 'text/plain', data: Module.Pointer_stringify($0) } );
            document.dispatchEvent(copyEvent);
        }
        catch(err)
        {
            var copyDiv = document.createElement('div');
            copyDiv.contentEditable = true;
            document.body.appendChild(copyDiv);
            copyDiv.innerHTML = Module.Pointer_stringify($0);
            copyDiv.unselectable = "off";
            copyDiv.focus();
            document.execCommand('SelectAll');
            document.execCommand('Copy', false, null);
            document.body.removeChild(copyDiv);
        }
    }, textUTF8);

#endif
}