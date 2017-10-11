#include "GameHalf.h"


#include "resourceManager.h"
#include "resourceDatabase.h"
#include "usageDatabase.h"
#include "speechHints.h"

#include "SpriteResource.h"
#include "Room.h"
#include "StateObject.h"
#include "TimbreResource.h"
#include "Scale.h"
#include "Music.h"
#include "Song.h"
#include "Scene.h"
#include "Palette.h"


#include "ToolTipManager.h"
#include "ToolTipDisplay.h"

#include "TimerDisplay.h"
#include "WarningDisplay.h"

#include "imageCache.h"

#include "packSaver.h"

#include "minorGems/system/Time.h"
#include "minorGems/util/TranslationManager.h"




ToolTipDisplay *mainTipDisplay;
TimerDisplay *mainTimerDisplay;

WarningDisplay *mainWarningDisplay;


GameHalf::GameHalf()
        : mQuitting( false ) {

    initImageCache();
    
    // init this first so it creates dir where stringDB can go.
    initResourceManager();
    initDatabase();
    
    initUsageDatabase();
    
    initSpeechHints();
    
    /*
    for( int i=0; i<5; i++ ) {    
        initUsageDatabase();
        freeUsageDatabase();
        }
    exit( 0 );
    */
    
    
    

    Room::staticInit();
    
    SpriteResource::staticInit();
    
    ToolTipManager::staticInit();
    
    StateObject::staticInit();

    TimbreResource::staticInit();

    Scale::staticInit();

    Music::staticInit();

    Song::staticInit();
    

    Scene::staticInit();

    Palette::staticInit();
    
    initPackSaver();
    

    mainTimerDisplay = new TimerDisplay( 16, 272 );

    mainTipDisplay = new ToolTipDisplay( 48, 272 );

    mainWarningDisplay = new WarningDisplay( 299, 150 ); 
    }

        
GameHalf::~GameHalf() {
    delete mainTipDisplay;
    delete mainTimerDisplay;
    delete mainWarningDisplay;
    
    ToolTipManager::staticFree();
    
    Room::staticFree();
    SpriteResource::staticFree();
    
    StateObject::staticFree();

    Song::staticFree();
    
    Music::staticFree();

    TimbreResource::staticFree();

    Scale::staticFree();

    Scene::staticFree();

    Palette::staticFree();
    
    freePackSaver();

    freeSpeechHints();
    freeUsageDatabase();
    freeDatabase();
    freeResourceManager();
    

    freeImageCache();
    }



char GameHalf::isQuitting() {
    return mQuitting;
    }




extern char hardToQuitMode;

static unsigned char lastKeyPressed = '\0';

extern char practiceMode;


static void cancelOurTip() {
    // is our tip still showing?
    char *tip = ToolTipManager::getTip();
    
    char *ourTip = (char *)TranslationManager::translate( 
        "quitQuestion" );
    
    if( tip != NULL && 
        strcmp( tip, ourTip ) == 0 ) {
                
        // unfreeze
        ToolTipManager::freeze( false );
        // clear it instantly (no fade)
        ToolTipManager::setTip( "" );
        }
    }


void GameHalf::keyPressed( unsigned char inKey, int inX, int inY ) {
    //if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
    if( !hardToQuitMode ) { 

        if( lastKeyPressed == 27 ) {
            // is our tip no longer showing?
            char *tip = ToolTipManager::getTip();
            
            char *ourTip = (char *)TranslationManager::translate( 
                "quitQuestion" );
            
            if( tip == NULL  || 
                strcmp( tip, ourTip ) != 0 ) {
                
                // reset last key to look for another ESC press
                lastKeyPressed = '\0';
                }
            }
        

        // ESC
        if( lastKeyPressed == 27 && ( inKey == 'y' || inKey == 'Y' ) ) {
            //exit( 0 );
            mQuitting = true;
            practiceMode = false;

            // block "y" event from going to other handlers and showing
            // up in a speech bubble or search box
            mEatEvent = true;
            }
        else if( lastKeyPressed == 27 && ( inKey != 'y' && inKey != 'Y' ) ) {
            // cancel it

            cancelOurTip();
            }
        else {
            if( inKey == 27 ) {
                // override freeze
                ToolTipManager::freeze( false );
                
                ToolTipManager::setTip(
                    (char *)TranslationManager::translate( 
                        "quitQuestion" ) );
                // re-freeze until Y or another key pressed
                ToolTipManager::freeze( true );
                }
            }
        lastKeyPressed = inKey;
        }
    else {
        // # followed by ESC
        if( lastKeyPressed == '#' && inKey == 27 ) {
            //exit( 0 );
            mQuitting = true;
            practiceMode = false;
            }
        lastKeyPressed = inKey;
        }
    
    }


void GameHalf::mouseDragged( int inX, int inY ) {
    if( lastKeyPressed == 27 ) {
        cancelOurTip();
        }
    }



void GameHalf::mousePressed( int inX, int inY ) {
    if( lastKeyPressed == 27 ) {
        cancelOurTip();
        }
    }


void GameHalf::mouseReleased( int inX, int inY ) {
    if( lastKeyPressed == 27 ) {
        cancelOurTip();
        }
    }





void GameHalf::decrementStepCount() {
    if( mStepsLeft == mStepsPerSecond * mSecondsPerMove ) {
        // first step, just take a single step regardless of time
        mStepsLeft --;
        mLastStepTime = Time::getCurrentTime();
        }        
    else if( Time::getCurrentTime() - mLastStepTime >= 
             1.0 / mStepsPerSecond ) {
            
        // subsequent steps based on time

        // at least one real-time step has passed
            
        double numPassed = ( Time::getCurrentTime() - mLastStepTime ) 
            * mStepsPerSecond ;
            
        // round down
        int wholeNumPassed = (int)numPassed;
            
        double extraTime = 
            ( numPassed - wholeNumPassed ) / mStepsPerSecond;
            
            
        char passingZero = false;
            
        if( mStepsLeft > 0 ) {
            passingZero = true;
            }

        mStepsLeft -= wholeNumPassed;
        mLastStepTime = Time::getCurrentTime();
            
        // add extra for next step
        mLastStepTime -= extraTime;
            

        if( mStepsLeft < 0  && passingZero) {
            mStepsLeft = 0;
            }
        }
    }

