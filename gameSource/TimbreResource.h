#ifndef TIMBRE_RESOURCE_INCLUDED
#define TIMBRE_RESOURCE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"
#include "musicPlayer.h"


// number of harmonics
#define F   16
// number of levels for a harmonic, including 0
#define FL  16


class TimbreResource {
    public:

        // blank music
        TimbreResource();
        

        // timbre loaded from resource manager
        TimbreResource( uniqueID inID );
        
        // timbre loaded from data string
        TimbreResource( uniqueID inID, unsigned char *inData, int inLength );
        

        // changes the level of a harmonic
        // harmonic from 0..15
        // level from 0..16
        void editHarmonic( int inHarmonic, int inLevel );
        
        
        int getHarmonicLevel( int inHarmonic );
        

        // values in 0..255
        void editEnvelope( int inAttack, int inHold,
                           int inRelease );
        
        void editOctavesDown( int inOctavesDown );
        

        int getAttack();
        int getHold();
        int getRelease();
        
        int getOctavesDown();
        

        // this timbre as a color, for easy differentiation in displays
        rgbaColor getTimbreColor();
        

        
        // name has at most 10 chars
        void editTimbreName( const char *inName );


        // destroyed by caller
        char *getTimbreName();
        

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
        static TimbreResource getDefaultResource();
        
        char *getName() {
            return getTimbreName();
            }


        // sets this timbre into music player
        void setInPlayer( int inTimbreNumber,
                          char inUpdatePlayerTimbre,
                          char inUpdatePlayerEnvelope );


        
        
        void print();
        

                // blank timbre (pure sine)
        static TimbreResource *sBlankTimbre;

        static void staticInit();
        static void staticFree();

    protected:
        
        void setupDefault();
        

        char initFromData( unsigned char *inData, int inLength );
        

        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );


        void makeUniqueID();

        
        unsigned char mHarmonicLevels[F];
        
        unsigned char mAttack;
        unsigned char mHold;
        unsigned char mRelease;
        
        // 0 to use base frequency
        // positive numbers to go down by octaves
        unsigned char mOctavesDown;
        
        
        const static int mDataLength = F + 4;
        
        char mName[11];

        
        uniqueID mID;
        
        
    };


#endif
