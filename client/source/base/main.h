#ifndef __DFG_GAME_H__
#define __DFG_GAME_H__



class DfgGame : public gameplay::Game
{
public:
    DfgGame();

    static DfgGame * getInstance() { return static_cast<DfgGame *>(gameplay::Game::getInstance()); };

    const char * getUserDataFolder() const { return _userFolder.c_str(); };
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

protected:
    void initialize();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);
    void pause();
    void resume();

    /**
     * Set game locale, or use system default one.
     *
     * @param newLocale Locale to be set, NULL to use system-wide locale.
     */
    void setGameLocale(const char * newLocale = NULL);

    void keyEvent(gameplay::Keyboard::KeyEvent evt, int key, bool processed);
    void touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex, bool processed);
    bool mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, float wheelDelta, bool processed);
    void gesturePinchEvent(int x, int y, float scale, int numberOfTouches);

    /**
     * @see Game::resizeEvent;
     */
    void resizeEvent(unsigned int width, unsigned int height) override;

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
    virtual void gestureSwipeEvent(int x, int y, int direction);

    void reportError(bool isFatal, const char * errorMessage, ...);

    class RenderService * _renderService;
    class InputService * _inputService;

private:
    std::string _userFolder;
    std::string _gameLocale;
    bool _hyperKeyPressed;  // WinKey (win), Command (osx)
};


#endif // __DFG_GAME_H__
