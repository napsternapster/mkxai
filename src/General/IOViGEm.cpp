#include "IOVigem.h"

#include <vector>
#include "Keycodes.h"

IOVigem::IOVigem()
    : m_report( {0} )
{
    if( !VIGEM_SUCCESS( vigem_init() ) )
    {
        throw std::runtime_error( "Cannot initialize gamepad.\n" );
        return;
    }
    VIGEM_TARGET_INIT( &m_x360 );
    if( !VIGEM_SUCCESS( vigem_target_plugin( Xbox360Wired, &m_x360 ) ) )
    {
        throw std::runtime_error( "Cannot initialize gamepad.\n" );
        return;
    }
}

IOVigem::~IOVigem()
{
    vigem_target_unplug( &m_x360 );
    vigem_shutdown();
}

void IOVigem::ProcessInput( int key )
{
    if( key < 4 || key == 20 )
    {
        PressKey( key );
    }
    else
    {
        HoldKey( key, std::chrono::duration<double>( 0.55 ) );
    }
    m_report = { 0 };
    for( auto& keyPressed : m_releaseTimers )
    {
        SetOutput( keyPressed.first );
    }
    vigem_xusb_submit_report( m_x360, m_report );
}

// Detects if key was pressed between function executions
bool IOVigem::WasKeyPressed( int key )
{
    // pure virtual gamepad
    return false;
}

void IOVigem::ReleaseAllKeys()
{
    SetOutput( 0 );
    m_releaseTimers.clear();
}

void IOVigem::Update( std::chrono::duration<double> timeDiff )
{
    bool released = false;
    std::vector<int> keys;
    for( auto& keyPressed : m_releaseTimers )
    {
        keyPressed.second -= timeDiff;
        if( keyPressed.second <= std::chrono::duration<double>::zero() )
        {
            keys.push_back( keyPressed.first );
            released = true;
        }
    }
    for( auto& key : keys )
    {
        ReleaseKey( key );
    }
    keys.clear();
    if( released )
    {
        m_report = { 0 };
        for( auto& keyPressed : m_releaseTimers )
        {
            SetOutput( keyPressed.first );
        }
        vigem_xusb_submit_report( m_x360, m_report );
    }
}

void IOVigem::HoldKey( int key, std::chrono::duration<double> duration )
{
    m_releaseTimers[key] = duration;
}

void IOVigem::PressKey( int key )
{
    m_releaseTimers[key] = std::chrono::duration<double>(0.1);
}

void IOVigem::ReleaseKey( int key )
{
    if( m_releaseTimers.count( key ) )
    {
        m_releaseTimers.erase( key );
    }
}

void IOVigem::SetOutput( int key )
{
    switch( key )
    {
    case Keycodes::GPKEY::A:
        m_report.wButtons += XUSB_GAMEPAD_A;
        break;
    case Keycodes::GPKEY::B:
        m_report.wButtons += XUSB_GAMEPAD_B;
        break;
    case Keycodes::GPKEY::X:
        m_report.wButtons += XUSB_GAMEPAD_X;
        break;
    case Keycodes::GPKEY::Y:
        m_report.wButtons += XUSB_GAMEPAD_Y;
        break;
    case Keycodes::GPKEY::DOWN:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_DOWN;
        break;
    case Keycodes::GPKEY::LEFT:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_LEFT;
        break;
    case Keycodes::GPKEY::RIGHT:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_RIGHT;
        break;
    case Keycodes::GPKEY::UP:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_UP;
        break;
    case Keycodes::GPKEY::SELECT:
        m_report.wButtons += XUSB_GAMEPAD_BACK;
        break;
    default:
        m_report = { 0 };
        break;
    }
}
