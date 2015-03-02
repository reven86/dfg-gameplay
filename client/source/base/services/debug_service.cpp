#include "pch.h"
#include "debug_service.h"
#include "services/service_manager.h"
#include "services/render_service.h"
#include "render/mem_fn_render_click.h"




DebugService::DebugService( const ServiceManager * manager )
    : Service( manager )
    , _systemFont( NULL )
    , _whiteSprite( NULL )
    , _debugRenderStep( NULL )
    , _fpsRenderClick( NULL )
    , _fontSize( 1 )
    , _showProfiler( false )
{
}

DebugService::~DebugService( )
{
}

void DebugService::Setup( gameplay::SpriteBatch * whiteSprite, gameplay::Font * font, float scale )
{
    _whiteSprite = whiteSprite;
    _systemFont.reset( font );
    _fontSize = font->getSize( ) * scale;
}

bool DebugService::OnPreInit( )
{
    return true;
}

bool DebugService::OnTick( )
{
#ifndef __DISABLE_PROFILER__
    unsigned framesCount = gameplay::Game::getInstance( )->getFrameRate( );
    Profiler::Instance( ).SaveProfilerResults( );
    Profiler::Instance( ).SetFrameTime( framesCount > 0.0f ? 1.0f / framesCount : 1.0f );
#endif
    return false;
}

bool DebugService::OnInit( )
{
    RenderService * rs = _manager->FindService< RenderService >( );

    _debugRenderStep.reset( rs->CreateRenderStep( "DebugRenderStep" ) );

    _fpsRenderClick.reset( new MemFnRenderClick< DebugService >( this, &DebugService::DrawFPS, "FPSRenderClick" ) );
    _debugRenderStep->AddRenderClick( _fpsRenderClick );

    return true;
}

bool DebugService::OnShutdown( )
{
    _systemFont.reset( );
    _fpsRenderClick.reset( );
    _debugRenderStep.reset( );
    return true;
}

void DebugService::DrawTextOutline( float x, float y, const gameplay::Vector4& color, const wchar_t * text ) const
{
    _systemFont->start( );
    _systemFont->drawText( text, x - 1, y, gameplay::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ), _fontSize );
    _systemFont->drawText( text, x + 1, y, gameplay::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ), _fontSize );
    _systemFont->drawText( text, x, y - 1, gameplay::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ), _fontSize );
    _systemFont->drawText( text, x, y + 1, gameplay::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ), _fontSize );
    _systemFont->drawText( text, x, y, color, _fontSize );
    _systemFont->finish();
}

void DebugService::DrawFPS( ) const
{
    DrawTextOutline( 1, 1, gameplay::Vector4::one( ), Format( L"FPS: {0, number, integer}", gameplay::Game::getInstance( )->getFrameRate( ) ) );

#ifndef __DISABLE_PROFILER__
    if( _showProfiler )
        Profiler::Instance( ).DrawPerformanceInfo( _systemFont, _whiteSprite, _fontSize * 0.5f );
#endif
}