#pragma once


#ifndef __DFG_GAME_ADVANCED_H__
#define __DFG_GAME_ADVANCED_H__

#include "main.h"



/**
 * This is more feature-rich base class for a game. It supports Google Analytics
 * through TrackerService, uses basic integration with Emscripten and provides
 * basic method to serialize and deserialize settings.
 */

class DfgGameAdvanced : public DfgGame
{
public:
    DfgGameAdvanced();

protected:
    virtual void initialize() override;
};




#endif // __DFG_GAME_ADVANCED_H__