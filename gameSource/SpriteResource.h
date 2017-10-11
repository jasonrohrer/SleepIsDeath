#ifndef SPRITE_RESOURCE_INCLUDED
#define SPRITE_RESOURCE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"


class SpriteResource {
    public:

        // blank sprite
        SpriteResource();
        

        // sprite loaded from file
        SpriteResource( uniqueID inID );


        // sprite loaded from data string
        SpriteResource( uniqueID inID, unsigned char *inData, int inLength );

        

        // changes the color of a sprite pixel
        void editSprite( int inX, int inY, rgbaColor inNewColor );
        
        // changes the transparency of a pixel
        void editTrans( int inX, int inY, char inIsTrans );

        // name has at most 10 chars
        void editSpriteName( const char *inName );

        
        rgbaColor getColor( int inX, int inY );

        // destroyed by caller
        char *getSpriteName();
        
        char getTrans( int inX, int inY );


        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        
        // recursively saves to current resource pack
        void saveToPack();


        // get an image of this sprite
        Image *getImage( char inUseTrans=true );
        

        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=true, char inCacheOK=true );
        static const char *getResourceType();
        static SpriteResource getDefaultResource();
        
        char *getName() {
            return getSpriteName();
            }
        

        // a compliment of getSprite, for a blinking overlay
        Sprite *getHighlightSprite();

        
        void print();


        // blank sprite
        static SpriteResource *sBlankSprite;

        static void staticInit();
        static void staticFree();


    protected:
        void setupDefault();
        
        char initFromData( unsigned char *inData, int inLength );
        
        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );
        
        void makeUniqueID();

        
        // all sprites are exactly one grid square in size
        rgbaColor mPixelColors[P][P];

        char mTransFlags[P][P];
        
        const static int mPixelDataLength = P * P * 4;
        
        const static int mTransDataLength = P * P;
        

        char mSetName[11];

        
        uniqueID mID;
        
        
    };


#endif
