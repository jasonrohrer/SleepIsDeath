#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"

#include "minorGems/util/SimpleVector.h"


class Scene {
    public:

        // room filled with blank tiles and default name
        Scene();
        

        // room loaded from file
        Scene( uniqueID inID );

        // name has at most 10 chars
        void editSceneName( const char *inName );
        
        char *getSceneName();
        


        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        
        // recursively saves to current resource pack
        void saveToPack();
        

        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Scene getDefaultResource();
        
        char *getName() {
            return getSceneName();
            }

        


        // blank object
        static Scene *sBlankScene;
        
        static void staticInit();
        static void staticFree();
        
        

        void print();
        
        
        // not part of state that is saved to disk
        // used only by editor
        int mSelectedLayer;
        

        // these are public to avoid needed to write a bunch of set/get
        // functions
        uniqueID mRoom;

        unsigned char mRoomTrans;
        
        char mObjectZeroFrozen;

        SimpleVector<uniqueID> mObjects;
        SimpleVector<intPair> mObjectOffsets;
        SimpleVector<intPair> mSpeechOffsets;
        SimpleVector<char> mSpeechFlipFlags;
        SimpleVector<char> mSpeechBoxFlags;
        SimpleVector<char> mLockedFlags;
        SimpleVector<unsigned char> mObjectTrans;
        

    protected:
        void setupDefault();
        

        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );
        
        void readFromBytes( unsigned char *inBytes, int inLength );
        

        void makeUniqueID();


        
        
        char mName[11];
                
        uniqueID mID;
        
    };



#endif
