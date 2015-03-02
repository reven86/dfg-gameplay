#ifndef DfgGame_H_
#define DfgGame_H_



class DfgGame : public gameplay::Game
{
    std::string _userFolder;
    std::string _gameLocale;
    bool _hyperKeyPressed;  // WinKey (win), Command (osx)

protected:
	class RenderService * _renderService;
    class InputService * _inputService;

public:
    DfgGame();

	void keyEvent(gameplay::Keyboard::KeyEvent evt, int key);
    void touchEvent(gameplay::Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);
    bool mouseEvent(gameplay::Mouse::MouseEvent evt, int x, int y, int wheelDelta);

    const char * GetUserDataFolder( ) const { return _userFolder.c_str( ); };
    const char * GetGameLocale( ) const { return _gameLocale.c_str( ); };

    /** Schedule local notification.
     *
     *  \param[in]  datetime            Date in time (UNIX timestamp).
     *  \param[in]  utf8Body            Message text (UTF8).
     *  \param[in]  utf8ActionButton    Action button's name (UTF8).
     *  \param[in]  badgeNumber         App icon badge number.
     */
    virtual void ScheduleLocalNotification( time_t datetime, const char * utf8Body, const char * utf8ActionButton, int badgeNumber = 1, const std::unordered_map< std::string, std::string >& userDictionary = std::unordered_map< std::string, std::string >( ) );

    /** Cancel all local notification.
     */
    virtual void CancelAllLocalNotifications( );

protected:
    void initialize();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);
    void pause();
    void resume();

    void reportError( bool isFatal, const char * errorMessage, ... );
};

#endif
