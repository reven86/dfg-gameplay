#include "pch.h"
#include "main.h"
#include "services/service_manager.h"
#include "services/render_service.h"
#include "services/input_service.h"
#include "services/tracker_service.h"

#ifdef WIN32
#include <direct.h>
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

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
}

void DfgGame::initialize()
{
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
}

void DfgGame::update(float elapsedTime)
{
    ServiceManager::getInstance()->update(elapsedTime);
}

void DfgGame::render(float /*elapsedTime*/)
{
    if (ServiceManager::getInstance()->getState() == Service::RUNNING)
        _renderService->renderFrame();
}

void DfgGame::keyEvent(gameplay::Keyboard::KeyEvent evt, int key)
{
    if (_inputService)
        _inputService->injectKeyEvent(evt, key);

    if (key == gameplay::Keyboard::KEY_HYPER)
        _hyperKeyPressed = evt == gameplay::Keyboard::KEY_PRESS;
    else if (_hyperKeyPressed && evt == gameplay::Keyboard::KEY_PRESS &&
        (key == gameplay::Keyboard::KEY_CAPITAL_Q || key == gameplay::Keyboard::KEY_Q))
    {
        exit();
    }
}

void DfgGame::touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (_inputService)
        _inputService->injectTouchEvent(evt, x, y, contactIndex);
}

bool DfgGame::mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, float wheelDelta)
{
    if (_inputService)
        return _inputService->injectMouseEvent(evt, x, y, wheelDelta);
    return false;
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

    TrackerService * trackerService = ServiceManager::getInstance()->findService< TrackerService >();
    if (trackerService)
        trackerService->endSession("Pause");

    gameplay::Game::pause();
}

void DfgGame::resume()
{
    gameplay::Game::resume();

    ServiceManager::getInstance()->signals.resumeEvent();
}

void DfgGame::reportError(bool isFatal, const char * errorMessage, ...)
{
    va_list args;
    va_start(args, errorMessage);

    char exceptionDesc[150];
    vsnprintf(exceptionDesc, 149, errorMessage, args);
    exceptionDesc[149] = '\0';

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