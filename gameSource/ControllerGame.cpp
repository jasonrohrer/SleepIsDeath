

#include "ControllerGame.h"
#include "common.h"
#include "ColorEditor.h"
#include "TileEditor.h"
#include "StateObjectEditor.h"
#include "MusicEditor.h"
#include "TimbreEditor.h"
#include "ScaleEditor.h"
#include "SongEditor.h"
#include "PaletteEditor.h"

#include "ColorWells.h"
#include "TilePicker.h"
#include "RoomPicker.h"
#include "SpritePicker.h"
#include "StateObjectPicker.h"
#include "TimbrePicker.h"
#include "ScalePicker.h"
#include "MusicPicker.h"
#include "SongPicker.h"
#include "ScenePicker.h"
#include "PalettePicker.h"

#include "TimerDisplay.h"
#include "WarningDisplay.h"
#include "ToolTipManager.h"

#include "DragAndDropManager.h"

#include "packSaver.h"
#include "resourceImporter.h"


#include "minorGems/system/Time.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/SettingsManager.h"


#include "Connection.h"


extern Connection *connection;

extern TimerDisplay *mainTimerDisplay;

extern WarningDisplay *mainWarningDisplay;


ColorWells *mainColorStack;
TilePicker *mainTilePicker;

ColorEditor *mainColorEditor;

RoomPicker *mainRoomPicker;

TileEditor *mainTileEditor;

SpritePicker *mainSpritePicker;
SpritePicker *mainSpritePickerLower;

StateObjectPicker *mainStateObjectPicker;

TimbrePicker *mainTimbrePicker;
ScalePicker *mainScalePicker;
MusicPicker *mainMusicPicker;
SongPicker *mainSongPicker = NULL;

ScenePicker *mainScenePicker;
PalettePicker *mainPalettePicker;


DragAndDropManager *mainDragAndDrop;


RoomEditor *mainRoomEditor;

SpriteEditor *mainSpriteEditor;

StateObjectEditor *mainStateObjectEditor;

MusicEditor *mainMusicEditor;
TimbreEditor *mainTimbreEditor;
ScaleEditor *mainScaleEditor;
SongEditor *mainSongEditor = NULL;

PaletteEditor *mainPaletteEditor;


PlayerMoveEditor *mainPracticePlayerEditor;



char practiceMode = false;
char practiceStop = false;
char practicePlayerTurn = false;


//#include "fixOldResources.h"




ControllerGame::ControllerGame( ScreenGL *inScreen ) {
    setScreen( inScreen );


    // don't fetch resources from network, since we're the only
    // one using them
    setUseNetwork( false );
    
    
    //printf( "**** RUNNING resource fixing script\n" );
    //fixOldResources();


    
    char turnLengthFound = false;
    mSecondsPerMove = SettingsManager::getIntSetting( "timeLimit", 
                                                      &turnLengthFound );

    if( !turnLengthFound ) {
        mSecondsPerMove = 30;
        }
    

    mStepsPerSecond = 30;
    resetTimer();
        



    mainColorStack = new ColorWells( 255, 44 );
    mainTilePicker = new TilePicker( 245, 48 + 128 );
    
    mainColorEditor = new ColorEditor( inScreen );
    

    mainRoomPicker = new RoomPicker( 245, 48 );

    mainSpritePicker = new SpritePicker( 245, 48 + 128 );
    mainSpritePickerLower = new SpritePicker( 245, 48 );
    
    mainStateObjectPicker = new StateObjectPicker( 245, 48 + 128 );

    mainTimbrePicker = new TimbrePicker( 245, 48 + 128 );

    mainScalePicker = new ScalePicker( 245, 48 + 128 );


    mainMusicPicker = new MusicPicker( 245, 48 + 128 );

    mainSongPicker = new SongPicker( 245, 48 );


    mainScenePicker = new ScenePicker( 245, 48 );

    mainPalettePicker = new PalettePicker( 245, 48 + 128 );


    AppLog::info( "Importing resource cache" );
    importResources();
    AppLog::info( "Done importing resource cache" );


    AppLog::info( "Loading packs from loading bay" );
    loadPacks();
    AppLog::info( "Done loading packs" );

    
    mainTilePicker->forceNewSearch();
    mainRoomPicker->forceNewSearch();
    mainSpritePicker->forceNewSearch();
    mainSpritePickerLower->forceNewSearch();
    mainStateObjectPicker->forceNewSearch();
    mainTimbrePicker->forceNewSearch();
    mainScalePicker->forceNewSearch();
    mainMusicPicker->forceNewSearch();
    mainSongPicker->forceNewSearch();
    mainScenePicker->forceNewSearch();
    mainPalettePicker->forceNewSearch();

    mainDragAndDrop = new DragAndDropManager( 48, 48, 320, 240 );
    

    mainTileEditor = new TileEditor( inScreen );

    mainRoomEditor = new RoomEditor( inScreen );
    
    mainSpriteEditor = new SpriteEditor( inScreen );
    
    mainStateObjectEditor = new StateObjectEditor( inScreen );
    
    mainMusicEditor = new MusicEditor( inScreen );
    mainTimbreEditor = new TimbreEditor( inScreen );
    mainScaleEditor = new ScaleEditor( inScreen );
    mainSongEditor = new SongEditor( inScreen );
    mainPaletteEditor = new PaletteEditor( inScreen );

    mGameStateEditor = new GameStateEditor( inScreen );

    mainPracticePlayerEditor = new PlayerMoveEditor( inScreen );
    
    
    
    
    

    //mCurrentEditor = mRoomEditor;
    //mCurrentEditor = mTileEditor;
    //mCurrentEditor = mColorEditor;
    //mCurrentEditor = mSpriteEditor;
    mCurrentEditor = mGameStateEditor;
    

    mCurrentEditor->setVisible( true );
    mCurrentEditor->addActionListener( this );
    
    mainPracticePlayerEditor->addActionListener( this );
    mainPracticePlayerEditor->setMovesDisabled( true );


    practiceMode = false;
    practicePlayerTurn = false;
    }



ControllerGame::~ControllerGame() {
    mCurrentEditor->setVisible( false );

    delete mGameStateEditor;
    
    delete mainColorEditor;
    delete mainTileEditor;
    delete mainRoomEditor;
    delete mainSpriteEditor;
    delete mainStateObjectEditor;
    
    delete mainSongEditor;
    // re-NULL, in case a Player game started later
    mainSongEditor = NULL;

    delete mainMusicEditor; 
    delete mainTimbreEditor;
    delete mainScaleEditor;
    delete mainPaletteEditor;
    
    delete mainPracticePlayerEditor;

    delete mainDragAndDrop;
    

    delete mainColorStack;
    delete mainTilePicker;        
    delete mainRoomPicker;
    
    delete mainSpritePicker;
    delete mainSpritePickerLower;

    delete mainStateObjectPicker;
    
    delete mainSongPicker;
    
    delete mainTimbrePicker;

    delete mainScalePicker;

    delete mainPalettePicker;
    
    delete mainMusicPicker;

    delete mainScenePicker;
    
    }



void ControllerGame::resetTimer() {
    mainTimerDisplay->setTime( mSecondsPerMove );
    
    mStepsLeft = mStepsPerSecond * mSecondsPerMove;
    mLastStepTime = Time::getCurrentTime();
    }



void ControllerGame::step() {

    if( ( connection != NULL && connection->isConnected() )
        || practiceMode ) {
        // display and update timer

        decrementStepCount();
        
        
        if( mStepsLeft == mStepsPerSecond * 6 - 1) {
            mainWarningDisplay->show( "warning_sending_soon", mStepsLeft );
            }
        

        mainTimerDisplay->freeze( false );
        mainTimerDisplay->setTime( mStepsLeft / mStepsPerSecond );
        
        if( mStepsLeft > 0 ) {
            mGameStateEditor->enableSend( true );
            }
        }
    else {
        // switch tip on timer
        mainTimerDisplay->freeze( true );
        
        // no sending
        mGameStateEditor->enableSend( false );
        }
    
        
    if( mStepsLeft == 0 ) {
        // force a step here, regardless of system time
        // we NEVER want to run step 0 more than once (send same
        // state more than once)
        mStepsLeft --;


        if( connection != NULL  && ! practiceMode ) {
            
            AppLog::info( "Sending game state" );

            

            mGameStateEditor->aboutToSend();
            
            // pack last-saved music ID into this game state
            // (to share this saved music with the player's database)
            mGameStateEditor->mGameStateToEdit->mMusicID = 
                mainMusicPicker->getSelectedResourceID();

            // same for last-selected scene ID
            mGameStateEditor->mGameStateToEdit->mSceneID = 
                mainScenePicker->getSelectedResourceID();



            int numBytes;
            unsigned char *message = 
                mGameStateEditor->mGameStateToEdit->getStateAsMessage( 
                    &numBytes );
            
            // stale now, because sent to player, unless we edit it
            mGameStateEditor->mGameStateToEdit->markNonPlayerSpeechStale();
            
            

            connection->sendMessage( message, numBytes );

            delete [] message;        
            }
        else if( practiceMode && 
                 ! mainPracticePlayerEditor->isVisible() ) {

            mGameStateEditor->aboutToSend();
            
            mGameStateEditor->mGameStateToEdit->markNonPlayerSpeechStale();

            GameState *copyState = mGameStateEditor->mGameStateToEdit->copy();
            copyState->setSelectedObject( 0 );

            mainPracticePlayerEditor->setGameStateToEdit( copyState );
            

            mainPracticePlayerEditor->mGameStateToEdit
                ->markNonPlayerSpeechStale();

            practicePlayerTurn = true;
            // do this *after* taking flip book frame
            //mainPracticePlayerEditor->setMovesDisabled( false );

            // "player" (in practice mode) gets to move again
            mStepsLeft = mStepsPerSecond * mSecondsPerMove;
            mLastStepTime = Time::getCurrentTime();
            
            // clear waiting tip
            ToolTipManager::freeze( false );
            ToolTipManager::setTip( NULL );
                
            mainPracticePlayerEditor->enableSend( true );

            mGameStateEditor->showPracticePlayerEditor();            
            }
        else if( practiceMode &&
                 mainPracticePlayerEditor->isVisible() ) {
            
            // "player" (in practice mode) is sending
            mainPracticePlayerEditor->clearNonPlayerSpeech();
            
            mainPracticePlayerEditor->setMovesDisabled( true );
            
            // preserve selected object
            int oldSelected = 
                mGameStateEditor->mGameStateToEdit->getSelectedObject();

            GameState *copyState = 
                mainPracticePlayerEditor->mGameStateToEdit->copy();
            copyState->setSelectedObject( oldSelected );

            mGameStateEditor->setGameStateToEdit( copyState );


            practicePlayerTurn = false;
            // do this *after* taking flip book frame
            //mGameStateEditor->hidePracticePlayerEditor();
            
            mGameStateEditor->enableSend( true );

            mGameStateEditor->mGameStateToEdit->deleteAllNonPlayerSpeech();

            // controller gets to move again
            mStepsLeft = mStepsPerSecond * mSecondsPerMove;
            mLastStepTime = Time::getCurrentTime();
            
            if( practiceStop ) {
                // end of practice mode
                practiceStop = false;
                practiceMode = false;
                
                mainTimerDisplay->setTime( mStepsLeft / mStepsPerSecond );

                // switch tip on timer
                mainTimerDisplay->freeze( true );
                
                // no sending
                mGameStateEditor->enableSend( false );
                mGameStateEditor->hidePracticePlayerEditor();
                }
            else {
                // simulate player move received
                mainWarningDisplay->show( "warning_move_received", 
                                          mStepsPerSecond * 3 );
                }
            }
        }
    else if( mStepsLeft < 0 ) {
        // try to receive response
        
        if( connection != NULL ) {
        
            int numBytes;
            unsigned char *message = connection->receiveMessage( &numBytes );
        
            if( message != NULL ) {
                AppLog::info( "Got message" );
            
                GameState *state = new GameState( message, numBytes );

                delete [] message;
                                
                
                // only pay attention to speech, action/pos, and position for 
                // obj0 as sent by player
                StateObjectInstance *playerObjZero = 
                    *( state->mObjects.getElement( 0 ) );

                GameState *ourState = mGameStateEditor->mGameStateToEdit;
                
                // first, clear all other speech (it's stale after player sees
                // it one time)
                ourState->deleteAllNonPlayerSpeech();
                

                StateObjectInstance *ourObjZero = 
                    *( ourState->mObjects.getElement( 0 ) );
                
                ourObjZero->mAnchorPosition = playerObjZero->mAnchorPosition;
                
                delete [] ourObjZero->mSpokenMessage;
                ourObjZero->mSpokenMessage = 
                    stringDuplicate( playerObjZero->mSpokenMessage );
                
                ourObjZero->mActionOffset = playerObjZero->mActionOffset;
                
                memcpy( ourObjZero->mAction, playerObjZero->mAction, 11 );
                
                delete state;
                
                
                
                // don't update pickers, since controller might be in the
                // middle of editing something else
                mGameStateEditor->setGameStateToEdit( ourState->copy(),
                                                      false );
                mGameStateEditor->enableSend( true );
                

                // we get to move again
                mStepsLeft = mStepsPerSecond * mSecondsPerMove;
                mLastStepTime = Time::getCurrentTime();

                mainWarningDisplay->show( "warning_move_received", 
                                          mStepsPerSecond * 3 );
                }
            }
        }
    

    if( connection != NULL ) {
        
        // check for any resource requests (channel 1)
        int numBytes;
        unsigned char *message = connection->receiveMessage( &numBytes, 1 );
        if( message != NULL ) {
            // got one!
            unsigned long sec, ms;
            Time::getCurrentTime( &sec, &ms );
            AppLog::getLog()->logPrintf( 
                Log::DETAIL_LEVEL,
                "Got a resource request (%d:%d)...", (int)sec, (int)ms );
            
            int bytesLeft = numBytes;
            unsigned char *messageLeft = message;

            if( bytesLeft < 1 ) {
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "Error:  bad resource request received (l:%d)",
                    __LINE__ );
                delete [] message;
                return;
                }
        
            int numChunks = messageLeft[0];
        
            bytesLeft -= 1;
            messageLeft = &( messageLeft[1] );
        
            AppLog::getLog()->logPrintf( 
                Log::DETAIL_LEVEL, 
                "  Request contains %d chunks, %d bytes", numChunks,
                numBytes );
        
            SimpleVector<unsigned char> responseAccum;
        
        
            for( int i=0; i<numChunks; i++ ) {
                // decode chunk request and add data to response
            
                if( bytesLeft < 1 ) {
                    AppLog::getLog()->logPrintf( 
                        Log::ERROR_LEVEL,
                        "Error:  bad resource request received (l:%d)",
                        __LINE__ );
                    delete [] message;
                    return;
                    }
        
                int typeLength = messageLeft[0];
            
                bytesLeft -= 1;
                messageLeft = &( messageLeft[1] );
        
        
                if( bytesLeft < typeLength ) {
                    AppLog::getLog()->logPrintf( 
                        Log::ERROR_LEVEL,
                        "Error:  bad resource request received (l:%d),"
                        " error on chunk %d",
                        __LINE__, i );
                    delete [] message;
                    return;
                    }
        
                char *typeString = new char[ typeLength + 1];
                memcpy( typeString, messageLeft, typeLength );
                typeString[ typeLength ] = '\0';
        
                bytesLeft -= typeLength;
                messageLeft = &( messageLeft[ typeLength ] );
        

                int numUsed;
        
                uniqueID id = readUniqueID( messageLeft, bytesLeft,
                                            &numUsed );
                if( numUsed != U ) {
                    AppLog::error( "Error:  bad resource request received" );
                    delete [] message;
                    delete [] typeString;
                    return;
                    }
            
                bytesLeft -= numUsed;
                messageLeft = &( messageLeft[ numUsed ] );


                int dataLength;
                char fromNetwork;
                unsigned char *data = loadResourceData( typeString,
                                                        id,
                                                        &dataLength,
                                                        &fromNetwork );
                if( data == NULL ) {
                    char *idString = getHumanReadableString( id );
                    AppLog::getLog()->logPrintf( 
                        Log::ERROR_LEVEL,
                        "Error:  request for resource %s of type %s "
                        "that failed to load.", idString, typeString );
                    delete [] idString;

                    uniqueID defaultResourceID;
                    
                    char foundDefault = true;
                    
                    if( strcmp( typeString, "music" ) == 0 ) {
                        defaultResourceID = 
                            Music::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "scale" ) == 0 ) {
                        defaultResourceID = 
                            Scale::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "timbre" ) == 0 ) {
                        defaultResourceID = 
                            TimbreResource::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "song" ) == 0 ) {
                        defaultResourceID = 
                            Song::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "object" ) == 0 ) {
                        defaultResourceID = 
                            StateObject::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "room" ) == 0 ) {
                        defaultResourceID = 
                            Room::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "scene" ) == 0 ) {
                        defaultResourceID = 
                            Scene::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "sprite" ) == 0 ) {
                        defaultResourceID = 
                            SpriteResource::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "tile" ) == 0 ) {
                        defaultResourceID = 
                            Tile::getDefaultResource().getUniqueID();
                        }
                    else if( strcmp( typeString, "palette" ) == 0 ) {
                        defaultResourceID = 
                            Palette::getDefaultResource().getUniqueID();
                        }
                    else {
                        foundDefault = false;
                        AppLog::error( 
                            "Error, unknown resource type, cannot send "
                            "default" );
                        }

                    if( foundDefault ) {
                        AppLog::info( 
                            "Sending default resource data instead" );
                        data = loadResourceData( typeString,
                                                 defaultResourceID,
                                                 &dataLength,
                                                 &fromNetwork );
                        }
                    }
                
                
                if( data != NULL ) {
            
                    // chunk length
                    responseAccum.push_back( getChars( dataLength ), 4 );
                
                    // chunk data
                    responseAccum.push_back( data, dataLength );
             
                    delete [] data;
                    }
                else {
                    AppLog::criticalError( 
                        "CRITICAL ERROR:  failed to load any resource "
                        "data in response to client request" );
                    }
                
                    
                
                delete [] typeString;
                }
        
            delete [] message;
        
        
            unsigned char *responseMessage = responseAccum.getElementArray();
        
            // send the data back directly
            connection->sendMessage( responseMessage, responseAccum.size(), 
                                     1 );

            delete [] responseMessage;
        

            // step network to force send through
            char workLeft = true;
            while( workLeft ) {
                workLeft = connection->step();
                }
            }
        }
    

    }




void ControllerGame::drawScene() {

    }



void ControllerGame::postRedraw() {
    if( practiceMode ) {
        if( mStepsLeft == mStepsPerSecond * mSecondsPerMove ) {
    
            mainPracticePlayerEditor->saveFlipBookImage();
            
            if( practicePlayerTurn ) {
                // just received a move, and took a snapshot of it...
                // now enable player to move
                mainPracticePlayerEditor->setMovesDisabled( false );
                }
            else {
                // end of practice player turn
                mGameStateEditor->hidePracticePlayerEditor();
                }
            }
        }
    }




void ControllerGame::actionPerformed( GUIComponent *inTarget ) {    
    // close button
    if( inTarget == mCurrentEditor && mCurrentEditor != mGameStateEditor ) {
        mCurrentEditor->setVisible( false );
        mCurrentEditor->removeActionListener( this );
        mCurrentEditor = NULL;
        }
    else if( inTarget == mGameStateEditor || 
             inTarget == mainPracticePlayerEditor) {
        
        // event from this means "send"
        // from either StateEditor or practice player

        if( inTarget == mGameStateEditor ) {
            AppLog::info( "Send button pressed in GameStateEditor" );
            }
        else {
            AppLog::info( 
                "Send (or stop practice) button pressed in "
                "PracticePlayerEditor" );
            }
        

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

