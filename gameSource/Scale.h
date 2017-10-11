#ifndef SCALE_INCLUDED
#define SCALE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"
#include "musicPlayer.h"


// number of halftones
#define HT   12


class Scale {
    public:

        // blank music
        Scale();
        

        // music loaded from resource manager
        Scale( uniqueID inID );
        
        // music loaded from data string
        Scale( unsigned char *inData, int inLength );
        

        // changes halftone on/off
        // halftone from 0..11
        void editScale( int inHalftone, char inToneOn );
        
        
        char getToneOn( int inHalftone );
        
        int getNumOn();
        

        
        // name has at most 10 chars
        void editScaleName( const char *inName );


        // destroyed by caller
        char *getScaleName();
        

        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );

        // recursively saves to current resource pack
        void saveToPack();


        // get an image of this timbre
        Image *getImage();


        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Scale getDefaultResource();
        
        char *getName() {
            return getScaleName();
            }


        // sets this scale into music player
        void setInPlayer( );


        
        
        void print();
        

                // blank timbre (pure sine)
        static Scale *sBlankScale;

        static void staticInit();
        static void staticFree();

    protected:
        
        void setupDefault();
        

        char initFromData( unsigned char *inData, int inLength );
        

        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );


        void makeUniqueID();


        unsigned char mHalftonesOn[HT];        
        
        const static int mDataLength = HT;
        
        char mName[11];

        
        uniqueID mID;
        
        
    };


#endif
