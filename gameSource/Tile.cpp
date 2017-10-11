#include "Tile.h"
#include "resourceManager.h"
#include "Room.h"

#include "imageCache.h"
#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"



void Tile::setupDefault() {
    memset( (void *)mPixelColors, 0, mPixelDataLength );
    
    /*
    mPixelColors[0][0].comp.r = 255;
    mPixelColors[0][0].comp.a = 255;
    
    mPixelColors[0][1].comp.g = 255;
    mPixelColors[0][1].comp.a = 255;
    */

    const char *defaultName = "default";
    memcpy( mSetName, defaultName, strlen( defaultName ) + 1 );

    makeUniqueID();
    }



Tile::Tile() {
    setupDefault();
    }



char Tile::initFromData( unsigned char *inData, int inLength ) {
    if( inLength >= mPixelDataLength ) {
        
        memcpy( (void *)mPixelColors, inData, mPixelDataLength );
        
        inLength -= mPixelDataLength;
        
        // remainder is name
        if( inLength <= 11 ) {    
            memcpy( mSetName, &inData[mPixelDataLength], inLength );
            return true;
            }
        }
    return false;
    }



Tile::Tile( uniqueID inID, unsigned char *inData, int inLength )
        : mID( inID ) {
    
    if( !initFromData( inData, inLength ) ) {
        // fail
        setupDefault();
        }
    }




Tile::Tile( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;

    unsigned char *data = loadResourceData( "tile", mID, &length, 
                                            &fromNetwork );

    if( data != NULL ) {

        if( ! initFromData( data, length ) ) {
            // failure
            setupDefault();
            }

        delete [] data;
        data = NULL;

        if( fromNetwork ) {
            // save to disk using the ID that we fetched it with
            finishEdit( false );
            }
        }
    else {
        // tile failed to load
        setupDefault();
        }

    if( data != NULL ) {
        delete [] data;
        }
    }



void Tile::editTile( int inX, int inY, rgbaColor inNewColor ) {
    mPixelColors[inY][inX] = inNewColor;
    }


rgbaColor Tile::getColor( int inX, int inY ) {
    return mPixelColors[inY][inX];
    }



void Tile::editTileSetName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Tile set name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mSetName, inName, newLength + 1 );
        }
    }


#include "minorGems/util/stringUtils.h"


char *Tile::getTileSetName() {
    return stringDuplicate( mSetName );
    }



#include "minorGems/util/SimpleVector.h"

void Tile::finishEdit(  char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    if( ! equal( oldID, mID ) || 
        ! resourceExists( "tile", mID ) ) {
        
        // change
    
        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "tile", mID,
                          mSetName,
                          bytes, numBytes );
                        
        delete [] bytes;
        }
    }



unsigned char *Tile::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> dataAccum;

    dataAccum.push_back( (unsigned char *)mPixelColors, 
                         mPixelDataLength );
    
    dataAccum.push_back( (unsigned char *)mSetName, 
                         strlen( mSetName ) + 1 );

    *outLength = dataAccum.size();
    return dataAccum.getElementArray();
    }



void Tile::saveToPack() {
    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );
    
    addToPack( "tile", mID,
               mSetName,
               bytes, numBytes );
    
    delete [] bytes;
    }

    
        

uniqueID Tile::getUniqueID() {
    return mID;
    }



const char *Tile::getResourceType() {
    return "tile";
    }

Tile Tile::getDefaultResource() {
    return *( Room::sBlankTile );
    }




void Tile::makeUniqueID() {

    partialUniqueID p = startUniqueID();
    
    
    p = addToUniqueID( p,
                       (unsigned char*)mPixelColors, mPixelDataLength );
    
    p = addToUniqueID( p, (unsigned char*)mSetName, strlen( mSetName ) + 1 );
    
    mID = ::makeUniqueID( p );
    }



Image *Tile::getImage() {
    Image *image = new Image( P, P, 3, false );
    
    double *channels[3];
    
    int i;
    
    for( i=0; i<3; i++ ) {
        channels[i] = image->getChannel( i );
        }
    
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            int pixel = y * P + x;
            
            rgbaColor c = mPixelColors[y][x];
            
            for( i=0; i<3; i++ ) {

                channels[i][pixel] = c.bytes[i] / 255.0;
                }
            }
        }

    return image;
    }




Sprite *Tile::getSprite( char inUseTrans, char inCacheOK ) {
    // ignore inUseTrans

    if( inCacheOK ) {
        
        Image *cachedImage = getCachedImage( mID, false );
        
        if( cachedImage == NULL ) {
            cachedImage = getImage();
            
            addCachedImage( mID, false, cachedImage );
            }
        return new Sprite( cachedImage );
        }
    else {
        // ignore cache
        Image *image = getImage();

        Sprite *sprite = new Sprite( image );
        delete image;
        return sprite;
        }
    }



void Tile::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "Tile %s:\n", idString );
    
    delete [] idString;

    printf( "  set name: %s\n", mSetName );
    
    for( int y=0; y<P; y++ ) {
        printf( "  " );
        
        for( int x=0; x<P; x++ ) {   
            ::print( mPixelColors[y][x] );
            printf( " " );
            }
        printf( "\n" );
        }
    }


