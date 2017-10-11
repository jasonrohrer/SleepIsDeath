#include "GameState.h"
#include "Music.h"
#include "Song.h"
#include "Scene.h"
#include "SongPicker.h"
#include "SongEditor.h"
#include "speechHints.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"

#include <stdio.h>



extern SongPicker *mainSongPicker;
extern SongEditor *mainSongEditor;



char GameState::sNoteToggleBuffer[N][N];



GameState::GameState() {
    mRoomTrans = 255;
    
    mObjectZeroFrozen = false;
    
    mLocksOn = true;
    

    // must have object 0 always
    StateObject obj;
    
    newObject( G / 2, G / 2, obj );
    
    mSelectedObject = 0;
    }

        
GameState::~GameState() {
    for( int i=0; i<mObjects.size(); i++ ) {
        delete *( mObjects.getElement( i ) );
        }
    }

        
GameState *GameState::copy() {
    GameState *s = new GameState();
    
        

    s->mRoom = mRoom;
    s->mRoomTrans = mRoomTrans;
    
    s->mObjectZeroFrozen = mObjectZeroFrozen;
    
    s->mLocksOn = mLocksOn;
    
    
    // default s has one object in it
    // clear it
    int i;
    for( i=0; i<s->mObjects.size(); i++ ) {
        delete *( s->mObjects.getElement( i ) );
        }
    s->mObjects.deleteAll();


    for( i=0; i<mObjects.size(); i++ ) {
        StateObjectInstance *otherObject = *( mObjects.getElement( i ) );
        
        s->mObjects.push_back( otherObject->copy() );
        }
    
    s->mSelectedObject = mSelectedObject;
    
    return s;
    }



GameState::GameState( unsigned char *inMessage, int inLength ) {

    
    int numUsed;
    

    
    if( inLength < N * N ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }

    int i=0;
    for( int y=0; y<N; y++ ) {
        for( int x=0; x<N; x++ ) {
            sNoteToggleBuffer[y][x] = inMessage[i];
            i++;
            }
        }
    numUsed = N * N;
    inLength -= numUsed;
    inMessage = &( inMessage[ numUsed ] );



    // now music unique id:
    // do this *just* to fetch a copy of any saved remote music into the
    // local database.  Don't actually load it into our game state (because
    // live music grid loaded above)
    mMusicID = readUniqueID( inMessage, inLength, &numUsed );

    if( numUsed == -1 ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }
    inLength -= numUsed;
    inMessage = &( inMessage[ numUsed ] );

    Music m( mMusicID );



    // now scene unique id:
    // do this *just* to fetch a copy of any saved remote scene into the
    // local database.  Don't actually load it into our game state (because
    // we're loading the live state here, which may differ)
    mSceneID = readUniqueID( inMessage, inLength, &numUsed );

    if( numUsed == -1 ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }
    inLength -= numUsed;
    inMessage = &( inMessage[ numUsed ] );

    Scene s( mSceneID );




    uniqueID id = readUniqueID( inMessage, inLength, &numUsed );

    if( numUsed == -1 ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }
    inLength -= numUsed;
    inMessage = &( inMessage[ numUsed ] );
    

    Room r( id );
    
    mRoom = r;



    if( inLength <= 0 ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }
    mRoomTrans = (char)inMessage[0];
    
    inLength -= 1;
    inMessage = &( inMessage[ 1 ] );

    

    if( inLength <= 0 ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }
    mObjectZeroFrozen = (char)inMessage[0];
    
    inLength -= 1;
    inMessage = &( inMessage[ 1 ] );

    

    if( inLength <= 0 ) {
        AppLog::error( "Failed to parse GameState from message\n" );
        return;
        }
    int numObjects = inMessage[0];
    
    inLength -= 1;
    inMessage = &( inMessage[ 1 ] );


    AppLog::getLog()->logPrintf( 
        Log::DETAIL_LEVEL,
        "Parsing message for state with %d objects\n", numObjects );


    for( int i=0; i<numObjects; i++ ) {
        id = readUniqueID( inMessage, inLength, &numUsed );
        
        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        inLength -= numUsed;
        inMessage = &( inMessage[ numUsed ] );
        
        
        StateObject obj( id );
        StateObjectInstance *o = new StateObjectInstance( obj );
        
        mObjects.push_back( o );
        


        if( inLength <= 0 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        o->mObjectTrans = (char)inMessage[0];
    
        inLength -= 1;
        inMessage = &( inMessage[ 1 ] );


        
        o->mAnchorPosition = readIntPair( inMessage, inLength,
                                          &numUsed );        
        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        inLength -= numUsed;
        inMessage = &( inMessage[ numUsed ] );



        o->mSpeechOffset = readIntPair( inMessage, inLength,
                                        &numUsed );        
        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        inLength -= numUsed;
        inMessage = &( inMessage[ numUsed ] );



        o->mActionOffset = readIntPair( inMessage, inLength,
                                        &numUsed );        
        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        inLength -= numUsed;
        inMessage = &( inMessage[ numUsed ] );



        if( inLength < 4 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }

        o->mSpeechFlip = (char)( inMessage[0] );
        o->mSpeechBox = (char)( inMessage[1] );
        o->mLocked = (char)( inMessage[2] );

        int speechLength = inMessage[3];

        inLength -= 4;
        inMessage = &( inMessage[4] );


        setSpeechHint( id, o->mSpeechOffset, o->mSpeechFlip );

        
        if( inLength < speechLength ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        
        delete [] o->mSpokenMessage;
        o->mSpokenMessage = new char[ speechLength + 1 ];
        memcpy( o->mSpokenMessage, inMessage, speechLength );
        o->mSpokenMessage[speechLength] = '\0';
        
        inLength -= speechLength;
        inMessage = &( inMessage[ speechLength ] );


        if( inLength < 11 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        
        memcpy( o->mAction, inMessage, 11 );
        inLength -= 11;
        inMessage = &( inMessage[ 11 ] );

        //printf( "  " );
        //o->print();
        }
    

    mHasSong = false;
    
    // ignore the possibility of this being present if we are Controller
    // song should never be sent by Player, but is sometimes due to a v14
    // bug.
    if( inLength >= U && mainSongEditor == NULL ) {
        // song ID present?
        uniqueID id = readUniqueID( inMessage, inLength, &numUsed );

        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse GameState from message\n" );
            return;
            }
        inLength -= numUsed;
        inMessage = &( inMessage[ numUsed ] );

        // load this Song, to force it to be fetched from network
        Song s( id );
        
        // from network, but don't save it, because it was live on Controller
        // side (repeat same ID, even though it doesn't match---this 
        // live song has no ID, because it's never been saved)
        Song liveSong( id, inMessage, inLength, true, false );
        
        mLiveSongReceived = liveSong;  
    
        mHasSong = true;
        }
    

    /*
    AppLog::getLog()->logPrintf( 
        Log::DETAIL_LEVEL,
        "After parse, %d bytes left over\n", inLength );
    */
    mSelectedObject = 0;
    }



char GameState::hasSong() {
    // for now
    return mHasSong;
    }



// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];




unsigned char *GameState::getStateAsMessage( int *outLength ) {

    // NOTE:  music is a little inelegant, but that's because
    // it is *separate* from the game state (not undone, not bookmarked,
    //  etc, along with the game state)

    // format:

    // NxN note toggles (actual, live music state, maybe not yet saved)
    // Music ID (saved music selected in music picker)
    // Room ID
    // num objects
    // each object:
    //    StateObject ID
    //    object trans (0..255, 1 char)
    //    anchor pos  (as 8-byte int pair)
    //    speech offset (as 8-byte int pair)
    //    speech flip (1 char)
    //    speech length (1 char)
    //    speech chars
    // -- v14 stuff at end (so v13 can ignore it): --
    // NOTE:  these are ONLY present in messages sent FROM Controller
    // Song ID of last saved song
    // Inline version of live (perhaps not saved) Song object


    SimpleVector<unsigned char> messageAccum;
    
    
    // for v13 backwards compat
    // send across first bar of first instrument
    for( int y=0; y<N; y++ ) {
        for( int x=0; x<N; x++ ) {        
            messageAccum.push_back( 
                (unsigned char)( noteToggles[0][0][y][x] ) );
            }
        }


    messageAccum.push_back( mMusicID.bytes, U );

    messageAccum.push_back( mSceneID.bytes, U );


    uniqueID id = mRoom.getUniqueID();
    
    messageAccum.push_back( id.bytes, U );
    
    messageAccum.push_back( mRoomTrans );
    
    
    messageAccum.push_back( (unsigned char)mObjectZeroFrozen );


    int numObjects = mObjects.size();
    messageAccum.push_back( (unsigned char)( numObjects ) );

    AppLog::getLog()->logPrintf( 
        Log::DETAIL_LEVEL,
        "Generating message for state with %d objects\n", 
        numObjects );

    for( int i=0; i<numObjects; i++ ) {
        StateObjectInstance *o = *( mObjects.getElement( i ) );
        
        //printf( "  " );
        //o->print();
        
        id = o->mObject.getUniqueID();
        
        messageAccum.push_back( id.bytes, U );
        
        messageAccum.push_back( o->mObjectTrans );

        messageAccum.push_back( getChars( o->mAnchorPosition ), 8 );
        messageAccum.push_back( getChars( o->mSpeechOffset ), 8 );
        messageAccum.push_back( getChars( o->mActionOffset ), 8 );
        
        messageAccum.push_back( (unsigned char)( o->mSpeechFlip ) );
        messageAccum.push_back( (unsigned char)( o->mSpeechBox ) );
        messageAccum.push_back( (unsigned char)( o->mLocked ) );

        int speechLength = strlen( o->mSpokenMessage );
        
        messageAccum.push_back( (unsigned char)speechLength );

        messageAccum.push_back( (unsigned char *)( o->mSpokenMessage ),
                                speechLength );
        messageAccum.push_back( (unsigned char *)( o->mAction ),
                                11 );
        }


    if( mainSongEditor != NULL ) {
        
        // ID of selected, saved song
        uniqueID songID = mainSongPicker->getSelectedResourceID();

        messageAccum.push_back( songID.bytes, U );
        
        
        int numBytes;
        unsigned char *liveSongBytes = 
            mainSongEditor->getLiveSong().makeBytes( &numBytes );
        
        messageAccum.push_back( liveSongBytes, numBytes );
        delete [] liveSongBytes;
        }
    


    *outLength = messageAccum.size();


    return messageAccum.getElementArray();
    }








int GameState::getHitObject( int inGridX, int inGridY ) {
    // top objects first
    int numObjects = mObjects.size();

    // but consider 0 first
    StateObjectInstance *o = *( mObjects.getElement( 0 ) );
    
    if( o->mAnchorPosition.x / P == inGridX
        &&
        o->mAnchorPosition.y / P == inGridY ) {
        return 0;
        }
    


    
    // always consider unlocked before locked
    for( int i=numObjects-1; i>=0; i-- ) {
        StateObjectInstance *o = *( mObjects.getElement( i ) );
        
        if( ! o->mLocked ) {
            
            if( o->mAnchorPosition.x / P == inGridX
                &&
                o->mAnchorPosition.y / P == inGridY ) {
                return i;
                }
            }
        }
    
    if( !mLocksOn ) {
        
        // now consider locked, since no unlocked were hit
        for( int i=numObjects-1; i>=0; i-- ) {
            StateObjectInstance *o = *( mObjects.getElement( i ) );
        
            if( o->mLocked ) {
            
                if( o->mAnchorPosition.x / P == inGridX
                    &&
                    o->mAnchorPosition.y / P == inGridY ) {
                    return i;
                    }
                }
            }
        }
    
    return -1;
    }


int GameState::getSelectedObject() {
    return mSelectedObject;
    }


void GameState::setSelectedObject( int inSelected ) {
    mSelectedObject = inSelected;
    }


// to right a bit, centered vertically
static intPair defaultAnchorOffset = { P/2 + P,  P/2 };



void GameState::moveSelectedObject( int inGridX, int inGridY ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );


    intPair oldPos = o->mAnchorPosition;
    
    intPair newPos = { inGridX * P, inGridY * P };
    
    intPair deltaPos = subtract( newPos, oldPos );
    
    // subtract this delta from action offset so that it does NOT move
    // along with object (only if action has text in it AND anchor is not
    //  at starting pos [if at starting pos, it moves with object])
    if( strlen( o->mAction ) > 0 && 
        ! equals( o->mActionOffset, defaultAnchorOffset ) ) {

        o->mActionOffset = subtract( o->mActionOffset, deltaPos );
        }
        
    o->mAnchorPosition = newPos;
    }


void GameState::moveSelectedSpeechAnchor( int inPixelX, int inPixelY ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    
    intPair absPos = { inPixelX, inPixelY };
                
    o->mSpeechOffset = subtract( absPos, o->mAnchorPosition );

    setSpeechHint( o->mObject.getUniqueID(), o->mSpeechOffset, 
                   o->mSpeechFlip );
    }



void GameState::moveSelectedActionAnchor( int inPixelX, int inPixelY ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    
    intPair absPos = { inPixelX, inPixelY };
                
    o->mActionOffset = subtract( absPos, o->mAnchorPosition );
    }


void GameState::resetActionAnchor( int inObjectIndex ) {
    StateObjectInstance *o = *( mObjects.getElement( inObjectIndex ) );
    
    // to right a bit, centered vertically
    o->mActionOffset = defaultAnchorOffset;
    }




char GameState::canAdd() {
    return( mObjects.size() < 255 );
    }




void GameState::newObject( int inGridX, int inGridY, StateObject inObject ) {
    if( mObjects.size() >= 255 ) {
        AppLog::error( "Error:  adding another object to a full state!\n" );
        }
    else {
        
        StateObjectInstance *o = new StateObjectInstance( inObject );

        // centered
        o->mAnchorPosition.x = P * inGridX;
        o->mAnchorPosition.y = P * inGridY;
        
        o->mSpeechOffset = getSpeechHint( inObject.getUniqueID(),
                                          &( o->mSpeechFlip ) );
        //o->mSpeechOffset.y = P/2;

        // no sprites
        mObjects.push_back( o );
    
    
        mSelectedObject = mObjects.size() - 1;
        
        resetActionAnchor( mSelectedObject );
        }
    
    }



void GameState::deleteSelectedObject() {
    // never delete obj 0
    if( mSelectedObject > 0 ) {
        
        StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    
        delete o;
        mObjects.deleteElement( mSelectedObject );
       
        setSelectedObject( mSelectedObject - 1 );

        if( mLocksOn ) {
            // look for an unlocked object to make selected
            while( getSelectedLocked() ) {
                setSelectedObject( mSelectedObject - 1 );
                }
            }
        
        }
    }



void GameState::changeSelectedObject( StateObject inObject ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );

    o->mObject = inObject;
    }



void GameState::adjustSelectedTrans( unsigned char inTrans ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );

    o->mObjectTrans = inTrans;
    }



unsigned char GameState::getSelectedTrans() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );

    return o->mObjectTrans;
    }



void GameState::addCharToSelectedSpeech( char inChar ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    
    if( strlen( o->mSpokenMessage ) < 255 ) {
        
        char *newString = autoSprintf( "%s%c", o->mSpokenMessage, inChar );
        delete [] o->mSpokenMessage;
        o->mSpokenMessage = newString;
        
        o->mSpeechStale = false;
        
        //printf( "Adding %d char\n", strlen( o->mSpokenMessage ) );
        }
    else {
        AppLog::error( "Error:  adding 256th character to speech\n" );
        }
    }


void GameState::deleteCharFromSelectedSpeech() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    int oldLength = strlen( o->mSpokenMessage );
    
    if( oldLength > 0 ) {
        // terminate one char sooner
        o->mSpokenMessage[ oldLength - 1 ] = '\0';
        o->mSpeechStale = false;
        }
    }


int GameState::getSelectedSpeechLength() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    return strlen( o->mSpokenMessage );
    }


void GameState::deleteAllCharsFromSelectedSpeech() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    delete [] o->mSpokenMessage;
    o->mSpokenMessage = stringDuplicate( "" );
    o->mSpeechStale = false;
    }



void GameState::flipSelectedSpeech() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    o->mSpeechFlip = ! o->mSpeechFlip;
    
    setSpeechHint( o->mObject.getUniqueID(), 
                   o->mSpeechOffset, o->mSpeechFlip );
    }



void GameState::setSelectedSpeechBox( char inBox ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    o->mSpeechBox = inBox;
    }



void GameState::setSelectedLocked( char inLocked ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    o->mLocked = inLocked;
    }



char GameState::getSelectedLocked() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    return o->mLocked;
    }



void GameState::setObjectHeld( int inObject, char inHeld ) {
    StateObjectInstance *o = *( mObjects.getElement( inObject ) );
    o->mHeld = inHeld;
    }



char GameState::getObjectHeld( int inObject ) {
    StateObjectInstance *o = *( mObjects.getElement( inObject ) );
    return o->mHeld;
    }

    



int GameState::getAutoOffsetOnFlip( int inObject ) {
    // heuristic:
    // if offset is less that one gridspace away from object center, 
    // in direction of manual flip, reverse the offset on the other side
    // when we auto-flip
    
    StateObjectInstance *o = *( mObjects.getElement( inObject ) );

    int offset = o->mSpeechOffset.x;
        
    int autoFlipExtraOffset = 0;
    
    if( !o->mSpeechFlip && offset >=P/2 && offset <= P + P/2 ) {
        autoFlipExtraOffset = -2 * ( offset - P/2) + 1 - 2;
        }
    else if( o->mSpeechFlip && offset <=P/2 && offset >= P/2 - P ) {
        autoFlipExtraOffset = -2 * ( offset - P/2) + 1 - 2;
        } 
    
    return autoFlipExtraOffset;
    }




void GameState::deleteAllNonPlayerSpeech() {
    int numObjects = mObjects.size();
    
    for( int i=1; i<numObjects; i++ ) {
        StateObjectInstance *o = *( mObjects.getElement( i ) );
        if( o->mSpeechStale ) {
            delete [] o->mSpokenMessage;
            o->mSpokenMessage = stringDuplicate( "" );
            o->mSpeechStale = false;
            }
        }
    }



void GameState::markNonPlayerSpeechStale() {
    int numObjects = mObjects.size();
    
    for( int i=1; i<numObjects; i++ ) {
        StateObjectInstance *o = *( mObjects.getElement( i ) );
        
        o->mSpeechStale = true;
        }
    }



void GameState::addCharToSelectedAction( char inChar ) {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    
    int oldLen = strlen( o->mAction );
    if( oldLen < 10 ) {
        
        o->mAction[ oldLen ] = inChar;
        o->mAction[ oldLen + 1 ] = '\0';
        }
    else {
        //printf( "Error:  adding 11th character to action\n" );
        }
    }


void GameState::deleteCharFromSelectedAction() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    int oldLength = strlen( o->mAction );
    
    if( oldLength > 0 ) {
        // terminate one char sooner
        o->mAction[ oldLength - 1 ] = '\0';
        }
    }


int GameState::getSelectedActionLength() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    return strlen( o->mAction );
    }


void GameState::deleteAllCharsFromSelectedAction() {
    StateObjectInstance *o = *( mObjects.getElement( mSelectedObject ) );
    o->mAction[0] = '\0';
    }



void GameState::freezeObjectZero( char inFrozen ) {
    mObjectZeroFrozen = inFrozen;
    }


char GameState::isObjectZeroFrozen() {
    return mObjectZeroFrozen;
    }








    
                                


static intPair zeroPair = {0,0};


StateObjectInstance::StateObjectInstance( StateObject inObject )
        : mObject( inObject ), mObjectTrans( 255 ),
          mAnchorPosition( zeroPair ),
          mSpeechOffset( zeroPair ),
          mActionOffset( zeroPair ),
          mSpeechFlip( 0 ),
          mSpeechBox( 0 ),
          mLocked( 0 ),
          mHeld( 0 ),
          mSpokenMessage( stringDuplicate( "" ) ),
          mSpeechStale( false )  {
    
    mAction[0] = '\0';
    }



StateObjectInstance::~StateObjectInstance() {
    delete [] mSpokenMessage;
    }



StateObjectInstance *StateObjectInstance::copy() {
    StateObjectInstance *o = new StateObjectInstance( mObject );
    
    delete [] o->mSpokenMessage;
    
    o->mObjectTrans = mObjectTrans;
    o->mAnchorPosition = mAnchorPosition;
    o->mSpeechOffset = mSpeechOffset;
    o->mActionOffset = mActionOffset;
    
    o->mSpeechFlip = mSpeechFlip;
    o->mSpeechBox = mSpeechBox;
    o->mLocked = mLocked;
    o->mHeld = mHeld;
        
    o->mSpokenMessage = stringDuplicate( mSpokenMessage );
    
    o->mSpeechStale = mSpeechStale;
    

    // init mAction first so that we don't leave uninitialized values in
    // end of buffer
    memset( (void *)( o->mAction ), '\0', 11 );

    memcpy( o->mAction, mAction, strlen( mAction ) + 1 );

    return o;
    }


void StateObjectInstance::print() {
    printf( "State object at " );
    ::print( mAnchorPosition );
    printf( "\n" );
    }

    
    



