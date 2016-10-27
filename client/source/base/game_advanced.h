#pragma once


#ifndef __DFG_GAME_ADVANCED_H__
#define __DFG_GAME_ADVANCED_H__

#include "main.h"
#include "main/zip_package.h"





/**
 * This is more feature-rich base class for a game. It supports Google Analytics
 * through TrackerService, uses basic integration with Emscripten and provides
 * basic method to serialize and deserialize settings.
 */

class DfgGameAdvanced : public DfgGame
{
public:
    /**
     * Constructor.
     *
     * \param emscriptenDbName Name of the indexed database where settings is supposed to be put for web version.
     * \param analyticsId Google Analytics ID.
     */
    DfgGameAdvanced(const char * emscriptenDbName, const char * analyticsId);

    /**
     * Whether this is a first run for a user (no settings file found or loaded).
     *
     * \return True if it's the first run.
     */
    bool isFirstTimeUser() const { return _firstTimeUser; };

    /**
     * Emscripten-specific event called when web page is about to be closed.
     * By default app saves settings at this moment.
     */
    virtual void onBeforeUnload();

    /**
     * Pauses the game after being run.
     * Automatically saves the settings.
     */
    virtual void pause() override;

protected:
    /**
     * Initialize callback that is called just before the first frame when the game starts.
     * Calls the following methods: createServices, loadSettings, updateSettings.
     */
    virtual void initialize() override;

    /**
     * Finalize callback that is called when the game on exits.
     */
    virtual void finalize() override;

    /**
     * Update callback for handling update routines.
     *
     * Called just before render, once per frame when game is running.
     * Ideal for non-render code and game logic such as input and animation.
     *
     * @param elapsedTime The elapsed game time.
     */
    virtual void update(float elapsedTime) override;

    /**
     * Create the services used by the game. Override this method
     * to create your own services.
     */
    virtual void createServices();

    /**
     * Tries to open the stream (either file or indexed db for web version) with 
     * settings data and load from it.
     */
    virtual void loadSettings();

    /**
     * Loads settings from the stram.
     *
     * \param stream Stream to load from.
     */
    virtual void loadSettings(gameplay::Stream * stream);

    /**
     * This method merges the settings, loaded from the file with current ones.
     * Here you can define what to do if other archive has different version and
     * how to merge or update the data from older settings file.
     *
     * By default, only the values that are present in current settings dictionary
     * and have the same type are update, except blobs. Blobs are only updated if
     * they have the same size and the app's version is same in both archives.
     *
     * \param other Archive that contains settings dictionary, loaded from the storage.
     */
    virtual void mergeSettings(class Archive& other);

    /**
     * Save settings either to file or indexed db (for web version).
     */
    virtual void saveSettings();

    /**
     * Here you have last chance to update the settings before the services become active.
     * You can override here some settings based on command line and other sources.
     */
    virtual void updateSettings();

    /**
     * Whether or not previous run was unsuccessfull.
     */
    bool wasGameLoadingFailedLastTime() const { return _previousLaunchWasUnsuccessful; };

    /**
     * Confirm that the game is loaded and delete watch dog file.
     */
    void confirmGameIsLoaded();



    // zip package with game's data for web version
    std::unique_ptr<class ZipPackage> _resourcePackage;


private:
    void deleteWatchDogFile();

    static void settingsLoadCallback(void * arg, void * buffer, int bufferSize);
    static void settingsLoadErrorCallback(void * arg);

    void onLanguageChanged(const std::string& language);


    bool _readyToRun;
    bool _firstTimeUser;
    bool _previousLaunchWasUnsuccessful;
    bool _needToDeleteWatchDogFile;

    std::string _emscriptenDbName;
    std::string _analyticsId;
};




#endif // __DFG_GAME_ADVANCED_H__