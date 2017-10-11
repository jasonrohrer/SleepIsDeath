#ifndef TILE_INCLUDED
#define TILE_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"


class Tile {
    public:

        // blank tile
        Tile();
        

        // tile loaded from resource manager
        Tile( uniqueID inID );
        
        // tile loaded from data string
        Tile( uniqueID inID, unsigned char *inData, int inLength );
        

        // changes the color of a tile pixel
        void editTile( int inX, int inY, rgbaColor inNewColor );
        
        
        rgbaColor getColor( int inX, int inY );
        
        
        // name has at most 10 chars
        void editTileSetName( const char *inName );


        // destroyed by caller
        char *getTileSetName();
        

        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        

        // recursively saves to current resource pack
        void saveToPack();


        // get an image of this tile
        Image *getImage();
        

        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Tile getDefaultResource();
        
        char *getName() {
            return getTileSetName();
            }
        
        
        void print();
        

    protected:
        
        void setupDefault();
        

        char initFromData( unsigned char *inData, int inLength );
        
        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );


        void makeUniqueID();

        
        // all tiles are exactly one grid square in size
        rgbaColor mPixelColors[P][P];
        
        const static int mPixelDataLength = P * P * 4;
        
        char mSetName[11];

        
        uniqueID mID;
        
        
    };


#endif
