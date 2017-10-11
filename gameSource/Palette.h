#ifndef PALETTE_INCLUDED
#define PALETTE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"


class Palette {
    public:

        // blank palette
        Palette();
        

        // palette loaded from resource manager
        Palette( uniqueID inID );
        
        // palette loaded from data string
        Palette( uniqueID inID, unsigned char *inData, int inLength );
        

        // changes the color of a palette pixel
        void editPalette( int inIndex, rgbaColor inNewColor );
        
        
        rgbaColor getColor( int inIndex );
        
        
        // name has at most 10 chars
        void editPaletteName( const char *inName );


        // destroyed by caller
        char *getPaletteName();
        

        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        

        // recursively saves to current resource pack
        void saveToPack();


        // get an image of this palette
        Image *getImage();
        

        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Palette getDefaultResource();
        
        char *getName() {
            return getPaletteName();
            }
        
        
        void print();
        
        
        // blank sprite
        static  Palette *sBlankPalette;

        static void staticInit();
        static void staticFree();


    protected:
        
        void setupDefault();
        

        char initFromData( unsigned char *inData, int inLength );
        
        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );


        void makeUniqueID();

        
        // all palettes are exactly one grid square in size
        rgbaColor mWellColors[C];
        
        const static int mWellDataLength = C * 4;
        
        char mName[11];

        
        uniqueID mID;
        
        
    };


#endif
