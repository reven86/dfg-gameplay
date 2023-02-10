#include "pch.h"
#include "game_advanced.h"
#include "main/settings.h"
#include "main/memory_stream.h"
#include "main/zip_packages.h"
#include "services/service_manager.h"
#include "services/tracker_service.h"


#ifdef __APPLE__
#if TARGET_OS_IPHONE
#import "iRate/iRate.h"

@interface iRateTrackingDelegate : NSObject <iRateDelegate>
@end

@implementation iRateTrackingDelegate

- (void)iRateDidPromptForRating
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
    {
        TrackerService::Parameter params[] = {
            { "rate_action", VariantType(std::string("Prompt")) },
        };

        tracker->sendEvent("app_rate", params, sizeof(params) / sizeof(params[0]));
    }
}

-(void)iRateUserDidAttemptToRateApp
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
    {
        TrackerService::Parameter params[] = {
            { "rate_action", VariantType(std::string("Attempt")) },
        };

        tracker->sendEvent("app_rate", params, sizeof(params) / sizeof(params[0]));
    }
}

-(void)iRateUserDidDeclineToRateApp
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
    {
        TrackerService::Parameter params[] = {
            { "rate_action", VariantType(std::string("Decline")) },
        };

        tracker->sendEvent("app_rate", params, sizeof(params) / sizeof(params[0]));
    }
}

-(void)iRateUserDidRequestReminderToRateApp
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
    {
        TrackerService::Parameter params[] = {
            { "rate_action", VariantType(std::string("Remind")) },
        };

        tracker->sendEvent("app_rate", params, sizeof(params) / sizeof(params[0]));
    }
}

@end

#endif
#endif

#ifdef __ANDROID__
#include <android_native_app_glue.h>
extern struct android_app* __state;
#endif


#ifdef __EMSCRIPTEN__
extern "C"
{
    void onBeforeUnload()
    {
        static_cast<DfgGameAdvanced *>(gameplay::Game::getInstance())->onBeforeUnload();
    }
}
#endif





DfgGameAdvanced::DfgGameAdvanced(const char * emscriptenDbName, const char * analyticsApiSecret)
    : _firstTimeUser(false)
    , _emscriptenDbName(emscriptenDbName)
    , _previousLaunchWasUnsuccessful(false)
    , _needToDeleteWatchDogFile(false)
    , _analyticsApiSecret(analyticsApiSecret)
    , _readyToRun(false)
{
#ifdef __APPLE__
#if TARGET_OS_IPHONE
    [iRate sharedInstance].daysUntilPrompt = 4;
    [iRate sharedInstance].usesUntilPrompt = 10;
    [iRate sharedInstance].promptAtLaunch = false;
    //[iRate sharedInstance].previewMode = true;
    [iRate sharedInstance].delegate = [[iRateTrackingDelegate alloc] init];
#endif
#endif
}

void DfgGameAdvanced::initialize()
{
    DfgGame::initialize();

    Settings::getInstance()->set("app.version", (int)1);
    Settings::getInstance()->set("app.uuid", (std::string)Utils::generateUUID());
    Settings::getInstance()->set("app.language", std::string(getGameLocale()));

#if defined (__EMSCRIPTEN__)
    
    // subscribe to beforeunload event to save settings
    EM_ASM({
        window.onbeforeunload = function(e) {
            Module.ccall('onBeforeUnload', null,[],[]);
        };
    });

    ZipPackagesCache::findOrOpenPackage("resources.data");

#elif defined (__ANDROID__)

    // get installer package name
    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    jclass android_content_Context = env->FindClass("android/content/Context");
    jmethodID midGetPackageName = env->GetMethodID(android_content_Context, "getPackageName", "()Ljava/lang/String;");
    jstring packageName = (jstring)env->CallObjectMethod(app->activity->clazz, midGetPackageName);

    // context.getPackageManager()
    jmethodID midGetPackageManager = env->GetMethodID(android_content_Context, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject packageManager = env->CallObjectMethod(app->activity->clazz, midGetPackageManager);
    jclass packageManagerClass = env->GetObjectClass(packageManager);

    //packageManager.getInstallerPackageName()
    jmethodID midGetInstallerPackageName = env->GetMethodID(packageManagerClass, "getInstallerPackageName", "(Ljava/lang/String;)Ljava/lang/String;");

    jstring installerPackageName = (jstring)env->CallObjectMethod(packageManager, midGetInstallerPackageName, packageName);

    const char * pkg = installerPackageName ? env->GetStringUTFChars(installerPackageName, NULL) : NULL;

    if (pkg)
        _installerId = pkg;

    if (installerPackageName)
        env->ReleaseStringUTFChars(installerPackageName, pkg);

    vm->DetachCurrentThread();

#endif

    createServices();
    loadSettings();
#ifndef __EMSCRIPTEN__
    updateSettings();
#endif
}

void DfgGameAdvanced::createServices()
{
    ServiceManager::getInstance()->registerService< TrackerService >(NULL);
}

void DfgGameAdvanced::settingsLoadCallback(void * arg, void * buffer, int bufferSize)
{
    DfgGameAdvanced * _this = reinterpret_cast<DfgGameAdvanced*>(arg);
    std::unique_ptr<MemoryStream> stream(MemoryStream::create(buffer, bufferSize));

    _this->loadSettings(stream.get());

    // continue execution
    _this->updateSettings();
}

void DfgGameAdvanced::settingsLoadErrorCallback(void * arg)
{
    DfgGameAdvanced * _this = reinterpret_cast<DfgGameAdvanced*>(arg);
    //GP_WARN("Can't read from persistent DB!");

    // continue execution
    _this->updateSettings();
}

void DfgGameAdvanced::loadSettings()
{
    _firstTimeUser = true;

#ifdef __EMSCRIPTEN__

    emscripten_idb_async_load(_emscriptenDbName.c_str(), "settings.arch", this, &DfgGameAdvanced::settingsLoadCallback, &DfgGameAdvanced::settingsLoadErrorCallback);

#else

    std::string watchDogFile = std::string(getUserDataFolder()) + "/watchdog.dat";
    _previousLaunchWasUnsuccessful = gameplay::FileSystem::fileExists(watchDogFile.c_str());

    // create a watch dog file before loading settings
    // the presence of this file indicates that app didn't manage to transit to the RUNNING state last time
    // this file will be deleted when ServiceManager's state become RUNNING, which means
    // that all services have been initialized properly
    gameplay::Stream * stream(gameplay::FileSystem::open(watchDogFile.c_str(), gameplay::FileSystem::WRITE));
    SAFE_DELETE(stream);
    _needToDeleteWatchDogFile = true;

    std::string filename = std::string(getUserDataFolder()) + "/settings.arch";
    std::unique_ptr<gameplay::Stream> stream2(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::READ));
    loadSettings(stream2.get());

#endif
}

void DfgGameAdvanced::onBeforeUnload()
{
    saveSettings();
}

void DfgGameAdvanced::loadSettings(gameplay::Stream * stream)
{
    if (!stream)
        return;

    std::unique_ptr<Archive> arch(Archive::create());

    bool settingsLoaded = arch->deserialize(stream);
    if (settingsLoaded)
    {
        if (getConfig()->getBool("firstRunTest"))
            return;

        mergeSettings(*arch);

        _firstTimeUser = false;
    }
    else
    {
        // save settings file just to not lose any information
        // this file can later be processed by future versions of the app
        // rename(filename.c_str(), (filename + "_error").c_str());
        saveSettings();
    }
}

void DfgGameAdvanced::mergeSettings(Archive& other)
{
    // merge the settings, update only keys that are present in the default settings dictionary
    std::vector<std::string> commonKeys;
    Settings::getInstance()->getCommonKeys(other, &commonKeys);
    int sourceVersion = Settings::getInstance()->get<int>("app.version", 0);
    int destVersion = other.get<int>("app.version", 0);
    for (const std::string& key : commonKeys)
    {
        if (key == "app.version")
            continue;

        // do not update "arg.XXX" settings, they are for internal use only
        if (strncmp(key.c_str(), "arg.", 4) == 0)
            continue;

        VariantType * sourceValue = Settings::getInstance()->get(key.c_str());
        const VariantType& destValue = other.get<VariantType>(key.c_str());
        GP_ASSERT(sourceValue);

        // do not update the values if they have different types
        if (sourceValue->getType() != destValue.getType())
            continue;

        if (sourceValue->getType() == VariantType::TYPE_BYTE_ARRAY)
        {
            uint32_t sourceSize, destSize;
            sourceValue->getBlob(&sourceSize);
            destValue.getBlob(&destSize);

            // do not update the blobs if they have different sizes or different versions
            if (sourceSize != destSize || sourceVersion != destVersion)
                continue;
        }

        sourceValue->set(destValue);
    }
}

void DfgGameAdvanced::saveSettings()
{
#ifdef __EMSCRIPTEN__
    std::unique_ptr<MemoryStream> stream(MemoryStream::create());
#else
    std::string filename = std::string(getUserDataFolder()) + "/settings.arch";
    std::unique_ptr<gameplay::Stream> stream(gameplay::FileSystem::open(filename.c_str(), gameplay::FileSystem::WRITE));
#endif

    if (!stream)
        return;

    Settings::getInstance()->serialize(stream.get());

#ifdef __EMSCRIPTEN__
    emscripten_idb_async_store(_emscriptenDbName.c_str(), "settings.arch", (void *)stream->getBuffer(), stream->length(), this, NULL, NULL);
#endif
}

void DfgGameAdvanced::updateSettings()
{
    // try to setup locale based on settings
    // if it fails, fallback to game's one
    const std::string& appLanguage = Settings::getInstance()->get<std::string>("app.language");
    setGameLocale(appLanguage.c_str());
    if (getGameLocale() != appLanguage)
        Settings::getInstance()->set("app.language", std::string(getGameLocale()));

    saveSettings();



    // all settings are updated
    // proceed to run

    gameplay::Properties * properties = gameplay::Properties::create("@dictionary.txt");
    gameDictionary.create(properties);
    SAFE_DELETE(properties);

    TrackerService * tracker = ServiceManager::getInstance()->findService<TrackerService>();

    tracker->setupTracking(_analyticsApiSecret.c_str());

#ifdef __EMSCRIPTEN__
    // get the domain name where the app is running
    int stringPointer = EM_ASM_INT_V({
        var url = (parent !== window) ? document.referrer : window.location.href;
        var referrer = '(null)';
        try
        {
            referrer = url.match(new RegExp('://(.[^/]+)'))[1];
        }
        catch (err)
        {
            referrer = 'exception';
        }
        var _ptr = _malloc(referrer.length+1);
        stringToUTF8(referrer, _ptr, referrer.length+1);
        return _ptr;
    });
    int urlPointer = EM_ASM_INT_V({
        var url = (parent !== window) ? document.referrer : window.location.href;
        var _ptr = _malloc(url.length+1);
        stringToUTF8(url, _ptr, url.length+1);
        return _ptr;
    });

    const char * referrer = reinterpret_cast<const char *>(stringPointer);
    const char * url = reinterpret_cast<const char *>(urlPointer);
    tracker->sendEvent("Domain", referrer, url);
    free((void *)url);
    free((void *)referrer);
#endif

    Settings::getInstance()->connect<std::string>("app.language", std::bind(&DfgGameAdvanced::onLanguageChanged, this, std::placeholders::_1));

    _readyToRun = true;
}

void DfgGameAdvanced::onLanguageChanged(const std::string& language)
{
    // reload the page for web version in case language is changed
#ifdef __EMSCRIPTEN__
    saveSettings();
    EM_ASM({ window.location.reload(); });
#endif
}

void DfgGameAdvanced::update(float elapsedTime)
{
    if (_readyToRun)
        DfgGame::update(elapsedTime);
}

void DfgGameAdvanced::finalize()
{
#if defined (__EMSCRIPTEN__)
    ZipPackagesCache::closePackage("resources.data");
#endif
    saveSettings();
    DfgGame::finalize();
}

void DfgGameAdvanced::pause()
{
    if (_needToDeleteWatchDogFile)
        deleteWatchDogFile();
    DfgGame::pause();
    saveSettings();
}

void DfgGameAdvanced::deleteWatchDogFile()
{
    _needToDeleteWatchDogFile = false;

    std::string watchDogFile = std::string(getUserDataFolder()) + "/watchdog.dat";
    GP_ASSERT(gameplay::FileSystem::fileExists(watchDogFile.c_str()));

    remove(watchDogFile.c_str());
}

void DfgGameAdvanced::confirmGameIsLoaded()
{
    if (_needToDeleteWatchDogFile)
        deleteWatchDogFile();
}
