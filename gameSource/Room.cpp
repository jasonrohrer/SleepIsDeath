#include "Room.h"
#include "resourceManager.h"
#include "usageDatabase.h"

#include "uniqueID.h"

#include "imageCache.h"
#include "packSaver.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"





// static inits
Tile *Room::sBlankTile = NULL;
Room *Room::sBlankRoom = NULL;

void Room::staticInit() {
    sBlankTile = new Tile();
    // ensure that blank tile is saved at least once
    sBlankTile->finishEdit();

    sBlankRoom = new Room();
    // save once
    sBlankRoom->finishEdit();
    }

void Room::staticFree() {
    delete sBlankTile;
    delete sBlankRoom;
    }



void Room::setupDefault() {
    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );
    

    uniqueID blankID = sBlankTile->getUniqueID();
    
    int x;
    int y;
    for( y=0; y<G; y++ ) {
        for( x=0; x<G; x++ ) {
            mTiles[y][x] = blankID;
            mWallFlags[y][x] = false;
            }
        }
    
    makeUniqueID();
    }




Room::Room() {
    setupDefault();
    }




// room loaded from file
Room::Room( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;
    
    unsigned char *data = loadResourceData( "room", mID, &length,
                                            &fromNetwork );

    
    if( data != NULL ) {
        readFromBytes( data, length );
    
        delete [] data;

        if( fromNetwork ) {
            
            // tiles might not be present locally, either

            // more efficient to load them in a chunk instead of with separate
            // messages
            
            
            uniqueID ids[G*G];
            
            int i=0;
            
            for( int ty=0; ty<G; ty++ ) {
                for( int tx=0; tx<G; tx++ ) {
                    ids[i] = mTiles[ty][tx];
                    i++;
                    }
                }
            
            int chunkLengths[G*G];
            char fromNetworkFlags[G*G];
            

            unsigned char **chunks = loadResourceData( "tile",
                                                       G*G,
                                                       ids,
                                                       chunkLengths,
                                                       fromNetworkFlags );
            
            for( int i=0; i<G*G; i++ ) {
                
                if( chunks[i] != NULL ) {

                    if( fromNetworkFlags[i] ) {
                        // make a tile
                        Tile t( ids[i], chunks[i], chunkLengths[i] );
                        
                        // force into disk cache, don't remake ID
                        t.finishEdit( false );
                        }
                    

                    delete [] chunks[i];
                    }
                
                }
            delete [] chunks;
            


            // save room to disk using the ID that we fetched it with
            finishEdit( false );
            }
        }
    else {
        setupDefault();
        }
    
    
    }
    


void Room::editRoomTile( int inX, int inY, uniqueID inTileID ) {
    mTiles[inY][inX] = inTileID;
    }

        
void Room::editRoomWall( int inX, int inY, char inIsWall ){
    mWallFlags[inY][inX] = inIsWall;
    }

        

void Room::editRoomName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Room name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }



uniqueID Room::getTile( int inX, int inY ) {
    return mTiles[inY][inX];
    }


char Room::getWall( int inX, int inY ) {
    return mWallFlags[inY][inX];
    }


char *Room::getRoomName() {
    return stringDuplicate( mName );
    }

    

// finishes the edit, generates a new unique ID, saves result
void Room::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    
    if( !equal( oldID, mID ) || 
        ! resourceExists( "room", mID ) ) {
        // change
    
        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "room", mID, mName, bytes, numBytes );
        delete [] bytes;


        // track usages
        for( int y=0; y<G; y++ ) {
            for( int x=0; x<G; x++ ) {
                addUsage( mID, mTiles[y][x] );
                }
            }
        
        }
    }



void Room::saveToPack() {
    for( int ty=0; ty<G; ty++ ) {
        
        for( int tx=0; tx<G; tx++ ) {
    
            Tile t( mTiles[ty][tx] );
            t.saveToPack();
            }
        }
    

    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );
    
    addToPack( "room", mID, mName, bytes, numBytes );
    delete [] bytes;
    }

        

uniqueID Room::getUniqueID() {
    return mID;
    }


Image *Room::getImage() {
    // compose all tile images into a sprite with 1 pixel per tile
    
    // zero to prepare for average
    Image *fullImage = new Image( P, P, 3, true );

    double *fullC[3] = 
        { fullImage->getChannel( 0 ), 
          fullImage->getChannel( 1 ), 
          fullImage->getChannel( 2 ) };
    
    

    for( int ty=0; ty<G; ty++ ) {
        
        for( int tx=0; tx<G; tx++ ) {
            
            int fullP = (ty + 1) * P + (tx + 1);

            Tile t( mTiles[ty][tx] );
            
            Image *tImage = t.getImage();
            
            
            double *tC[3] = 
                { tImage->getChannel( 0 ), 
                  tImage->getChannel( 1 ), 
                  tImage->getChannel( 2 ) };

            // compute average of all tile pixels

            for( int c=0; c<3; c++ ) {
                // sum first
                for( int i=0; i<P*P; i++ ) {
                    fullC[c][fullP] += tC[c][i];
                    }
                fullC[c][fullP] /= P*P;
                }
            delete tImage;
            }
        }
    
    return fullImage;
    }



Sprite *Room::getSprite( char inUseTrans, char inCacheOK ) {
    // ignore inUseTrans

    Image *cachedImage = NULL;
    
    if( inCacheOK ) {    
        cachedImage = getCachedImage( mID, false );
        }
    
    if( cachedImage == NULL ) {
        


        cachedImage = getImage();
        
        if( inCacheOK ) {
            addCachedImage( mID, false, cachedImage );
            }
        }
    
    
    Sprite *sprite = new Sprite( cachedImage );
    if( !inCacheOK ) {
        // image not in cache... we must destroy it
        delete cachedImage;
        }
    return sprite;
    }



const char *Room::getResourceType() {
    return "room";
    }



Room Room::getDefaultResource() {
    return *sBlankRoom;
    }






#include "minorGems/util/SimpleVector.h"

unsigned char *Room::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> buffer;
    
    int y, x;
    
    for( y=0; y<G; y++ ) {
        for( x=0; x<G; x++ ) {
            buffer.push_back( mTiles[y][x].bytes, U );
            }
        }


    for( y=0; y<G; y++ ) {
        for( x=0; x<G; x++ ) {
            buffer.push_back( mWallFlags[y][x] );
            }
        }
    
    buffer.push_back( (unsigned char *)mName, strlen( mName ) + 1 );
    
    *outLength = buffer.size();
    return buffer.getElementArray();
    }



void Room::readFromBytes( unsigned char *inBytes, int inLength ) {

    int numBytesUsed;
    char success = readUniqueIDs( (uniqueID *)mTiles, G*G, 
                                  inBytes, inLength,
                                  &numBytesUsed );
    if( success ) {
        inBytes = &( inBytes[ numBytesUsed ] );
        inLength -= numBytesUsed;
        }
    
    if( inLength >= G*G ) {
        
        memcpy( (void *)mWallFlags, inBytes, G*G );

        inBytes = &( inBytes[ G*G ] );
        inLength -= G*G;
        }
    
    // remainder is name
    if( inLength <= 11 ) {    
        memcpy( mName, inBytes, inLength );
        }
    }



void Room::makeUniqueID() {

    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );

    mID = ::makeUniqueID( bytes, numBytes );
    delete [] bytes;
    }
