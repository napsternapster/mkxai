#include "Application.h"

#include "Event.h"
#include "Utility.h"
#include "Keycodes.h"

Application::Application()
    try : m_screenCapture( ScreenCapture( 1280, 720 ) )
    , m_ioGamepad()
    , m_pause( true )
    , m_timeCount( 0 )
    , m_updateCount( 0 )
    , m_player()
{
    m_matBitmap = cv::Mat( 720, 1280, CV_8UC4, m_screenCapture.GetPixelDataPointer(), 0 );
    m_dispatcher.connect<IOInterface>( &m_ioGamepad, &IOInterface::ProcessInput, m_player.m_SendInput );
    m_dispatcher.connect( &m_imageProcessor, &ImageProcessor::GetHealthValues, m_player.m_GetPlayersHealth );
    m_dispatcher.connect( &m_imageProcessor, &ImageProcessor::GetDistance, m_player.m_GetDistance );
    m_dispatcher.connect( &m_imageProcessor, &ImageProcessor::GetSuggestedState, m_player.m_GetSuggestedState );
    m_dispatcher.connect( &m_pyWrapper, &PythonWrapper::Eval, m_imageProcessor.m_EvalNetwork );

    posA.resize( 2 );
    posB.resize( 2 );
}
catch( std::exception& e )
{
    OutputDebugString( e.what() );
}

Application::~Application()
{
}

bool Application::Run()
{
    std::cout << "Ready" << std::endl;
    auto time = std::chrono::high_resolution_clock::now();
    for(;;)
    {
        auto newTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> timeDiff = newTime - time;
        time = newTime;

        m_ioGamepad.Update( timeDiff );
        if( !ProcessUserInput() )
        {
            break;
        }
        if( !m_pause )
        {
            Update( timeDiff );
            Render();
        }
    }
    return false;
}

bool Application::ProcessUserInput()
{
    if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::Z ) )
    {
        return false;
    }
    if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::X ) )
    {
        m_pause = !m_pause;
    }
    if( m_pause )
    {
        if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::C ) )
        {
            m_ioGamepad.ProcessInput( Keycodes::GPKEY::A );
        }
        else if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::V ) )
        {
            m_ioGamepad.ProcessInput( Keycodes::GPKEY::B );
        }
        else if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::COMMA ) )
        {
            m_ioGamepad.ProcessInput( Keycodes::GPKEY::LEFT );
        }
        else if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::DOT ) )
        {
            m_ioGamepad.ProcessInput( Keycodes::GPKEY::RIGHT );
        }
        else if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::B ) )
        {
            m_ioGamepad.ProcessInput( Keycodes::GPKEY::DOWN );
        }
        else if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::N ) )
        {
            m_ioGamepad.ProcessInput( Keycodes::GPKEY::UP );
        }
    }
    else
    {
        if( m_ioKeyboard.WasKeyPressed( Keycodes::KBKEY::S ) )
        {
            // not working?
            if( m_player.GetState().compare( "Attack" ) == 0 )
            {
                m_player.SetState( StateMachine<Player>::STATE::APPROACH );
            }
            else
            {
                m_player.SetState( StateMachine<Player>::STATE::ATTACK );
            }
        }
    }
    return true;
}

void Application::Update( std::chrono::duration<double> timeDiff )
{
    m_timeCount += timeDiff;
    m_screenCapture.Capture();
    m_imageProcessor.Update( m_matBitmap );
    m_player.Update( timeDiff );
}

void Application::Render()
{
    #if RENDER_WINDOW
        // Displaying the results through Mat drops fps -- waitKey is slow
        cv::circle( m_matBitmap, cv::Point( posA[0] * m_matBitmap.cols, posA[1] * m_matBitmap.rows ), 20, cv::Scalar( 0, 0, 255 ), 5 );
        cv::circle( m_matBitmap, cv::Point( posB[0] * m_matBitmap.cols, posB[1] * m_matBitmap.rows ), 20, cv::Scalar( 0, 0, 255 ), 5 );
        cv::imshow( "MKX AI - Debug", m_matBitmap );
        cv::waitKey( 1 );
    #endif
    m_updateCount += 1;
    if( m_timeCount > std::chrono::duration<double>( 2 ) )
    {
        system( "cls" );
        std::cout << "Average Update Time: " << m_timeCount.count() / m_updateCount << std::endl;
        double playerHP, enemyHP;
        m_imageProcessor.GetHealthValues( playerHP, enemyHP );
        std::cout << "Player 1 HP: " << playerHP << " Player 2 HP: " << enemyHP << std::endl;
        std::cout << "Current State: " << m_player.GetState() << std::endl;
        double distance;
        m_imageProcessor.GetDistance( distance );
        //posA[0] = pos[0];
        //posA[1] = pos[1];
        //posB[0] = pos[2];
        //posB[1] = pos[3];
        std::cout << "Distance: " << /*Utility::Distance( posA, posB )*/ distance << std::endl;
        m_updateCount = 0;
        // hack for long updates
        if( m_timeCount > std::chrono::duration<double>( 2 * 2 ) )
        {
            m_timeCount = std::chrono::duration<double>( 0 );
        }
        else
        {
            m_timeCount -= std::chrono::duration<double>( 2 );
        }
    }
}

void Application::SaveFrame()
{
    static int imgNum = 0;
    auto path = "D:/repos/NNP/NNProject/imageData/else/";
    cv::imwrite( std::string( path ) + std::to_string( imgNum ) + std::string( ".bmp" ), m_matBitmap );
    imgNum++;
}
