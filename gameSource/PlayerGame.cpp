

#include "PlayerGame.h"

#include "TimerDisplay.h"
#include "Connection.h"

#include "ToolTipManager.h"

#include "musicPlayer.h"

#include "resourceManager.h"

#include "Song.h"




#include "common.h"

#include <stdlib.h>


#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/log/AppLog.h"



extern TimerDisplay *mainTimerDisplay;


extern Connection *connection;




PlayerGame::PlayerGame( ScreenGL *inScreen )
        : mConnected( true ) {

    setScreen( inScreen );


    // fetch failed resources from network
    setUseNetwork( true );



    char turnLengthFound = false;
    mSecondsPerMove = SettingsManager::getIntSetting( "timeLimit", 
                                                      &turnLengthFound );

    if( !turnLengthFound ) {
        mSecondsPerMove = 30;
        }


    mStepsPerSecond = 30;
    resetTimer();

    ToolTipManager::freeze( false );
    ToolTipManager::setTip( 
        (char*)TranslationManager::translate( "tip_waiting" ) );
    ToolTipManager::freeze( true );
    

    mEditor = new PlayerMoveEditor( inScreen );

    mEditor->setVisible( true );
    //mEditor->addActionListener( this );

    // wait for first move from player....
    mEditor->setMovesDisabled( true );
    mEditor->enableSend( false );

    // for send button
    mEditor->addActionListener( this );
    }

        
PlayerGame::~PlayerGame() {
    mEditor->setVisible( false );
    

    delete mEditor;
    }



// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];
extern int partLengths[PARTS];



void PlayerGame::resetTimer() {
    mainTimerDisplay->setTime( 0 );
    
    mStepsLeft = -1;
    mLastStepTime = Time::getCurrentTime();
    }




void PlayerGame::step() {

    if( connection != NULL && connection->isConnected() ) {
        
        if( ! mConnected ) {
            mConnected = true;
            
            if( mStepsLeft < 0 ) {
                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate( "tip_waiting" ) );
                ToolTipManager::freeze( true );
                }
            else {
                // clear no connection
                ToolTipManager::freeze( false );
                ToolTipManager::setTip( NULL );
                }
            }
        

        decrementStepCount();
        
        mainTimerDisplay->freeze( false );
        mainTimerDisplay->setTime( mStepsLeft / mStepsPerSecond );

        if( mStepsLeft > 0 ) {
            mEditor->enableSend( true );
            }
        }
    else {
        if( mConnected ) {
            mConnected = false;


            if( mStepsLeft < 0 ) {
                ToolTipManager::freeze( false );
                
                ToolTipManager::setTip( 
                    (char*)TranslationManager::translate(
                        "tip_noConnection" ) );

                ToolTipManager::freeze( true );
                }
            }
        
        mainTimerDisplay->freeze( true );
        mEditor->enableSend( false );
        }
    
    
    //    printf( "Waiting for message\n" );
    
    if( mStepsLeft == 0 ) {
        // force a step here, regardless of system time
        // we NEVER want to run step 0 more than once (send same
        // state more than once)
        mStepsLeft --;

        
        if( connection != NULL ) {
            
            AppLog::info( "Sending game state\n" );
            
            mEditor->enableSend( false );

            // first, clear speech that player is done reading
            mEditor->clearNonPlayerSpeech();
            

            int numBytes;
            unsigned char *message = 
                mEditor->mGameStateToEdit->getStateAsMessage( 
                    &numBytes );
            
            connection->sendMessage( message, numBytes );
            
            delete [] message;        

            ToolTipManager::setTip( 
                (char*)TranslationManager::translate( "tip_waiting" ) );
            ToolTipManager::freeze( true );
            }
        // wait for controler's response
        mEditor->setMovesDisabled( true );
        }
    else if( mStepsLeft < 0 ) {
        // try to receive message from controller
        if( connection != NULL ) {
        
            int numBytes;
            unsigned char *message = connection->receiveMessage( &numBytes );
        
            if( message != NULL ) {
                AppLog::info( "Got message\n" );
                
                GameState *state = new GameState( message, numBytes );

                delete [] message;



                if( ! state->hasSong() ) {
                    
                    // copy received note toggles into music player
                    // try to recreat v13 timbre bands
                    for( int y=0; y<N-1; y++ ) {
                        for( int x=0; x<N; x++ ) {
                            noteToggles[y/5][0][y%5][x] = 
                                GameState::sNoteToggleBuffer[y][x];
                            }
                        }
                    // top row goes in last timbre, too
                    for( int x=0; x<N; x++ ) {
                        noteToggles[2][0][5][x] = 
                            GameState::sNoteToggleBuffer[N-1][x];
                        }
                    partLengths[0] = 1;
                    partLengths[1] = 1;
                    partLengths[2] = 1;
                    
                    }
                else {
                    // probably coming from v14 Controller

                    // apply these changes to playing music
                    Song::setInPlayer( mLiveSong, state->mLiveSongReceived );
                    
                    mLiveSong = state->mLiveSongReceived;
                    }
                
                
                
                
                // do this *AFTER* snapshot is taken to avoid blank
                // action arrows in snapshot (if player position frozen)
                //mEditor->setMovesDisabled( false );


                mEditor->setGameStateToEdit( state );


                // stale now, because controller done editing it
                // thus, it will be cleared right as player sends move
                // (before taking flipbook screenshot, to avoid redundant
                //  speech in screenshots)
                mEditor->mGameStateToEdit->markNonPlayerSpeechStale();

                // player gets to move again
                mStepsLeft = mStepsPerSecond * mSecondsPerMove;
                mLastStepTime = Time::getCurrentTime();

                // clear waiting tip
                ToolTipManager::freeze( false );
                ToolTipManager::setTip( NULL );
                
                mEditor->enableSend( true );
                }
            }
        }
    
    
    }



void PlayerGame::drawScene() {

    // right after we receive their move
    //  OR
    // right after we sent our move
    if( mStepsLeft == mStepsPerSecond * mSecondsPerMove
        ||
        mStepsLeft == -1 ) {
        
        mEditor->saveFlipBookImage();

        if( mStepsLeft == mStepsPerSecond * mSecondsPerMove ) {
            // just received a move, and took a snapshot of it...
            // now enable player to move
            mEditor->setMovesDisabled( false );
            }
        }
    
    }



void PlayerGame::actionPerformed( GUIComponent *inTarget ) {    
    if( inTarget == mEditor ) {
        // event from this means "send"

        AppLog::info( "Send button pressed\n" );
        
        if( mStepsLeft > 0 ) {
            // will send on very next step
            mStepsLeft = 1;
            mainTimerDisplay->setTime( 0 );
            }
        else {
            AppLog::error( 
                "ERROR: Send button pressed when mStepsLeft not positive" );
            }
        
    
        }
    
    }

