#ifndef __DFG_GAME_H__
#define __DFG_GAME_H__




#ifdef __ANDROID__
extern "C" void Java_com_dreamfarmgames_util_DFGActivity_receiptReceived(JNIEnv* env, jobject thiz, jstring textObject);
extern "C" void Java_com_dreamfarmgames_util_TextViewActivity_textEntered(JNIEnv* env, jobject thiz, jstring textObject);
#endif


/**
 * Base class for your game. Extends the gameplay::Game class and provides
 * additional cross platform functionality like localization, notification and
 * simplifies some other tasks. Integrates with other input and rendering
 * services provided by the lib.
 *
 * For more feature-rich base class which integrates with Google Analytics,
 * iOS rating lib, provides basic integration with Emscipten which helps the game 
 * run in the web and supports serialization and deserialization of game's settings
 * use DfgGameAdvanced.
 *
 * @see DfgGameAdvanced
 */

class DfgGame : public gameplay::Game
{
public:
    DfgGame();

    static DfgGame * getInstance() { return static_cast<DfgGame *>(gameplay::Game::getInstance()); };

    /**
     * Get path to the folder where the app can save its files.
     * The folder is placed under the platform-dependend storage suited for app's files.
     * App's name is used as a part of the path.
     *
     * \return Path to folder where the game can put its private files (not user docs).
     */
    const char * getUserDataFolder() const { return _userFolder.c_str(); };

    /**
     * Get the locale used by the game.
     *
     * Available locales are specified in the game.config files.
     * Config file should have a section named 'aliases_XX, where XX is the local name,
     * which defines aliases for resources dependend on language.
     *
     * For example, if you put 'aliases_fr' section into the config put, you could set
     * the locale 'fr', otherwise locale won't be changed.
     *
     * \return Game's locale.
     */
    const char * getGameLocale() const { return _gameLocale.c_str(); };

    /**
     * Schedule local notification.
     *
     * @param[in]  datetime            Date in time (UNIX timestamp).
     * @param[in]  utf8Body            Message text (UTF8).
     * @param[in]  utf8ActionButton    Action button's name (UTF8).
     * @param[in]  badgeNumber         App icon badge number.
     */
    virtual void scheduleLocalNotification(time_t datetime, const char * utf8Body, const char * utf8ActionButton, int badgeNumber = 1, const std::unordered_map< std::string, std::string >& userDictionary = std::unordered_map< std::string, std::string >());

    /**
     * Cancel all local notification.
     */
    virtual void cancelAllLocalNotifications();

    /**
     * Prevent device from sleeping.
     */
    virtual void preventFromSleeping(bool prevent);

    /**
     * Copy text to clipboard.
     *
     * @param textUTF8 Text to be copied to clipboard (UTF-8).
     */
    virtual void copyToClipboard(const char * textUTF8) const;

#ifdef __EMSCRIPTEN__
    /**
     * Whether or not browser supports persistent IndexedDB storage.
     */
    bool hasIndexedDB() const { return _hasIndexedDB; };
#endif

protected:
    virtual void initialize() override;
    virtual void finalize() override;
    virtual void update(float elapsedTime) override;
    virtual void render(float elapsedTime) override;
    virtual void pause() override;
    virtual void resume() override;

    /**
     * Set game locale, or use system default one.
     *
     * @param newLocale Locale to be set, NULL to use system-wide locale.
     */
    void setGameLocale(const char * newLocale = NULL);

    virtual void keyEvent(gameplay::Keyboard::KeyEvent evt, int key, bool processed) override;
    virtual void touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex, bool processed) override;
    virtual bool mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, float wheelDelta, bool processed) override;
    virtual void gesturePinchEvent(int x, int y, float scale, int numberOfTouches) override;
    virtual void gestureRotationEvent(int x, int y, float rotation, int numberOfTouches) override;
    virtual void gesturePanEvent(int x, int y, int numberOfTouches) override;

    /**
     * @see Game::resizeEvent;
     */
    virtual void resizeEvent(unsigned int width, unsigned int height) override;

    /**
     * Gesture callback on Gesture::SWIPE events.
     *
     * @param x The x-coordinate of the start of the swipe.
     * @param y The y-coordinate of the start of the swipe.
     * @param direction The direction of the swipe
     *
     * @see Gesture::SWIPE_DIRECTION_UP
     * @see Gesture::SWIPE_DIRECTION_DOWN
     * @see Gesture::SWIPE_DIRECTION_LEFT
     * @see Gesture::SWIPE_DIRECTION_RIGHT
     */
    virtual void gestureSwipeEvent(int x, int y, int direction) override;

    virtual void reportError(bool isFatal, const char * errorMessage, ...) override;

#ifdef __ANDROID__
    friend void Java_com_dreamfarmgames_util_DFGActivity_receiptReceived(JNIEnv* env, jobject thiz, jstring textObject);
    friend void Java_com_dreamfarmgames_util_TextViewActivity_textEntered(JNIEnv* env, jobject thiz, jstring textObject);
    virtual void onReceiptReceived(const char * receiptJSON) {};
    virtual void onTextViewTextEntered(const char * textUTF8) {};
#endif

    class RenderService * _renderService;
    class InputService * _inputService;

private:
    std::string _userFolder;
    std::string _gameLocale;
    bool _hyperKeyPressed;  // WinKey (win), Command (osx)

#ifdef __EMSCRIPTEN__
    bool _hasIndexedDB;
#endif
};


#endif // __DFG_GAME_H__
