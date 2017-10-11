#ifndef GAME_STATE_INCLUDED
#define GAME_STATE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"

#include "Room.h"
#include "StateObject.h"
#include "Song.h"

#include "musicPlayer.h"


#include "minorGems/util/SimpleVector.h"



class StateObjectInstance {
    public:
        StateObjectInstance( StateObject inObject );

        ~StateObjectInstance();
        
        StateObjectInstance *copy();
        

        StateObject mObject;
        
        unsigned char mObjectTrans;
        

        intPair mAnchorPosition;
        intPair mSpeechOffset;
        intPair mActionOffset;
        
        char mSpeechFlip;
        
        char mSpeechBox;

        char mLocked;
        char mHeld;
        
        char *mSpokenMessage;
        
        char mAction[11];

        void print();


        char mSpeechStale;
    };



class GameState {
    public:

        // default empty state
        GameState();

        // load state from a message (destroyed by caller)
        GameState( unsigned char *inMessage, int inLength );
        
        ~GameState();
        
        GameState *copy();
        

        Room mRoom;
        
        unsigned char mRoomTrans;
        

        SimpleVector<StateObjectInstance*> mObjects;
        
        char mObjectZeroFrozen;
        
        // music ID to be sent along with this state when it is encoded
        // as a message
        uniqueID mMusicID;
        
        // scene ID to be sent along with this state when it is encoded
        // as a message---last selected scene from picker, even if it
        // doesn't match current scene state
        uniqueID mSceneID;
        
        
        char mLocksOn;
        

        unsigned char *getStateAsMessage( int *outLength );
        

        int getSelectedObject();
        
        void setSelectedObject( int inSelected );
        

        int getHitObject( int inGridX, int inGridY );

        void moveSelectedObject( int inGridX, int inGridY );

        void moveSelectedSpeechAnchor( int inPixelX, int inPixelY );

        void moveSelectedActionAnchor( int inPixelX, int inPixelY );
        
        void resetActionAnchor( int inObjectIndex );
        

        // room for one more?
        char canAdd();
        

        // becomes selected
        void newObject( int inGridX, int inGridY, StateObject inObject );
        
        void deleteSelectedObject();
        

        void changeSelectedObject( StateObject inObject );
        
        
        void adjustSelectedTrans( unsigned char inTrans );
        unsigned char getSelectedTrans();
        

        void addCharToSelectedSpeech( char inChar );
        void deleteCharFromSelectedSpeech();
        
        int getSelectedSpeechLength();
        void deleteAllCharsFromSelectedSpeech();
        
        void flipSelectedSpeech();

        void setSelectedSpeechBox( char inBox );

        void setSelectedLocked( char inLocked );
        
        char getSelectedLocked();


        void setObjectHeld( int inObject, char inHeld );
        
        char getObjectHeld( int inObject );
        

        // amount x position of speech should be shifted on a flip, 
        // based on a heuristic
        int getAutoOffsetOnFlip( int inObject );
        
        
        // deletes speech from every object but 0 
        void deleteAllNonPlayerSpeech();
        
        // sets as stale, for clear by deleteAllNonPlayerSpeech later
        // however, if speech is edited after this, it becomes non-stale again,
        // and won't be deleted by deleteAllNonPlayerSpeech
        void markNonPlayerSpeechStale();
        

        
        void addCharToSelectedAction( char inChar );
        void deleteCharFromSelectedAction();
        
        int getSelectedActionLength();
        void deleteAllCharsFromSelectedAction();
        

        void freezeObjectZero( char inFrozen );
        char isObjectZeroFrozen();


        // true if this received game state has a Song in it (v14)
        // or false if it uses the old (v13) note grid
        // So v14 and v13 can play together
        char hasSong();
        

        // where last received note toggles are stored
        // (from states parsed from messages)
        static char sNoteToggleBuffer[N][N];

        // where last received live song is stored
        Song mLiveSongReceived;
        

    protected:
        

        int mSelectedObject;

        char mHasSong;
        
        

    };



#endif
