#ifndef ROOM_INCLUDED
#define ROOM_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"

#include "Tile.h"



class Room {
    public:

        // room filled with blank tiles and default name
        Room();
        

        // room loaded from file
        Room( uniqueID inID );
        

        // changes the tile in a given grid location
        void editRoomTile( int inX, int inY, uniqueID inTileID );
        
        void editRoomWall( int inX, int inY, char inIsWall );
        
        // name has at most 10 chars
        void editRoomName( const char *inName );
        
        // gets ID of tile on grid
        uniqueID getTile( int inX, int inY );
        
        char getWall( int inX, int inY );
        
        char *getRoomName();
        


        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        

        // recursively saves to current resource pack
        void saveToPack();


        // image version of getSprite, never fetched from cache
        Image *getImage();
        

        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Room getDefaultResource();
        
        char *getName() {
            return getRoomName();
            }

        

        // blank tile used as "edge" in a room
        static Tile *sBlankTile;
        
        // blank room
        static Room *sBlankRoom;
        
        static void staticInit();
        static void staticFree();
        

    protected:
        void setupDefault();
        

        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );
        
        void readFromBytes( unsigned char *inBytes, int inLength );
        

        void makeUniqueID();

        
        // all tiles are exactly one grid square in size
        uniqueID mTiles[G][G];
        
        char mWallFlags[G][G];
        
        char mName[11];
                
        uniqueID mID;
        
    };



#endif
