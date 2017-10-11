#ifndef STATE_OBJECT_INCLUDED
#define STATE_OBJECT_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"

#include "minorGems/util/SimpleVector.h"


class StateObject {
    public:

        // empty object with default name
        StateObject();
        

        // object loaded from file
        StateObject( uniqueID inID );

        // object loaded from data string
        StateObject( uniqueID inID, unsigned char *inData, int inLength, 
                     char inFromNetwork );
        
        
        int getNumLayers();
        

        // true if there's room for more layers
        char canAdd( int inNumToAdd );
        

        // add a new layer with zero offset above layer number "inLayerBelow"
        // returns true on success, or false if full
        char addLayer( uniqueID inSpriteID, int inLayerBelow = -1 );
        void deleteLayer( int inLayer );
        
        void editLayerSprite( int inLayer, uniqueID inSpriteID );
        
        void editLayerOffset( int inLayer, intPair inOffset );
        
        void editLayerTrans( int inLayer, unsigned char inTrans );
        
        void editLayerGlow( int inLayer, char inGlowMode);
        

        void moveLayerUp( int inLayer );
        void moveLayerDown( int inLayer );
        

        // name has at most 10 chars
        void editStateObjectName( const char *inName );


        uniqueID getLayerSprite( int inLayer );
        
        intPair getLayerOffset( int inLayer );
        
        unsigned char getLayerTrans( int inLayer );

        char getLayerGlow( int inLayer );
        

        char *getStateObjectName();
        


        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        

        // recursively saves to current resource pack
        void saveToPack();



        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static StateObject getDefaultResource();
        
        char *getName() {
            return getStateObjectName();
            }

        


        // blank object
        static StateObject *sBlankObject;
        
        static void staticInit();
        static void staticFree();
        
        

        void print();
        
        
        // not part of state that is saved to disk
        // used only by editor
        int mSelectedLayer;
        

    protected:
        void setupDefault();
        
        void initFromData( unsigned char *inData, int inLength, 
                           char inFromNetwork );


        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );
        
        void readFromBytes( unsigned char *inBytes, int inLength );
        

        void makeUniqueID();

        
        SimpleVector<uniqueID> mSpriteLayers;
        SimpleVector<intPair> mLayerOffsets;
        SimpleVector<unsigned char> mLayerTrans;
        SimpleVector<unsigned char> mLayerGlow;
        
        char mName[11];
                
        uniqueID mID;
        
    };



#endif
