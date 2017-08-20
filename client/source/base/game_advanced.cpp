#include "pch.h"
#include "game_advanced.h"
#include "main/settings.h"
#include "main/memory_stream.h"
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
        tracker->sendEvent("Rating", "Prompt", "");
}

-(void)iRateUserDidAttemptToRateApp
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
        tracker->sendEvent("Rating", "AttemptToRate", "");
}

-(void)iRateUserDidDeclineToRateApp
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
        tracker->sendEvent("Rating", "Decline", "");
}

-(void)iRateUserDidRequestReminderToRateApp
{
    TrackerService * tracker = ServiceManager::getInstance()->findService< TrackerService >();
    if (tracker)
        tracker->sendEvent("Rating", "Remind", "");
}

@end

#endif
#endif



#ifdef __EMSCRIPTEN__
#include <emscripten.h>
extern "C"
{
    void onBeforeUnload()
    {
        static_cast<DfgGameAdvanced *>(gameplay::Game::getInstance())->onBeforeUnload();
    }
}
#endif





DfgGameAdvanced::DfgGameAdvanced(const char * emscriptenDbName, const char * analyticsId)
    : _firstTimeUser(false)
    , _emscriptenDbName(emscriptenDbName)
    , _previousLaunchWasUnsuccessful(false)
    , _needToDeleteWatchDogFile(false)
    , _analyticsId(analyticsId)
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

    _resourcePackage.reset(ZipPackage::create("resources.data"));
    if (_resourcePackage)
        gameplay::FileSystem::registerPackage(_resourcePackage.get());
    else
        GP_WARN("Webapp resources are missed.");
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

    tracker->setTrackerEnabled(true);
    tracker->setupTracking(_analyticsId.c_str(), Settings::getInstance()->get<std::string>("app.uuid").c_str(), "Loading");

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
        return allocate(intArrayFromString(referrer), 'i8', ALLOC_STACK);
    });

    const char * referrer = reinterpret_cast<const char *>(stringPointer);
    tracker->sendEvent("Domain", referrer, "");
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
