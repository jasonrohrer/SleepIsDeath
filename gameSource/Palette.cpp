#include "Palette.h"
#include "resourceManager.h"
#include "Room.h"

#include "imageCache.h"
#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"



// static inits
Palette *Palette::sBlankPalette = NULL;

void Palette::staticInit() {
    sBlankPalette = new Palette();
    // ensure that blank palette is saved at least once
    sBlankPalette->finishEdit();
    }

void Palette::staticFree() {
    delete sBlankPalette;
    }



void Palette::setupDefault() {
    memset( (void *)mWellColors, 0, mWellDataLength );
    

    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );

    makeUniqueID();
    }



Palette::Palette() {
    setupDefault();
    }



char Palette::initFromData( unsigned char *inData, int inLength ) {
    if( inLength >= mWellDataLength ) {
        
        memcpy( (void *)mWellColors, inData, mWellDataLength );
        
        inLength -= mWellDataLength;
        
        // remainder is name
        if( inLength <= 11 ) {    
            memcpy( mName, &inData[mWellDataLength], inLength );
            return true;
            }
        }
    return false;
    }



Palette::Palette( uniqueID inID, unsigned char *inData, int inLength )
        : mID( inID ) {
    
    if( !initFromData( inData, inLength ) ) {
        // fail
        setupDefault();
        }
    }




Palette::Palette( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;

    unsigned char *data = loadResourceData( "palette", mID, &length, 
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
        // palette failed to load
        setupDefault();
        }

    if( data != NULL ) {
        delete [] data;
        }
    }



void Palette::editPalette( int inIndex, rgbaColor inNewColor ) {
    mWellColors[inIndex] = inNewColor;
    }


rgbaColor Palette::getColor( int inIndex ) {
    return mWellColors[inIndex];
    }



void Palette::editPaletteName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Palette set name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }


#include "minorGems/util/stringUtils.h"


char *Palette::getPaletteName() {
    return stringDuplicate( mName );
    }



#include "minorGems/util/SimpleVector.h"

void Palette::finishEdit(  char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    if( ! equal( oldID, mID ) || 
        ! resourceExists( "palette", mID ) ) {
        
        // change
    
        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "palette", mID,
                          mName,
                          bytes, numBytes );
                        
        delete [] bytes;
        }
    }



unsigned char *Palette::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> dataAccum;

    dataAccum.push_back( (unsigned char *)mWellColors, 
                         mWellDataLength );
    
    dataAccum.push_back( (unsigned char *)mName, 
                         strlen( mName ) + 1 );

    *outLength = dataAccum.size();
    return dataAccum.getElementArray();
    }



void Palette::saveToPack() {
    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );
    
    addToPack( "palette", mID,
               mName,
               bytes, numBytes );
    
    delete [] bytes;
    }

    
        

uniqueID Palette::getUniqueID() {
    return mID;
    }



const char *Palette::getResourceType() {
    return "palette";
    }

Palette Palette::getDefaultResource() {
    return *( Palette::sBlankPalette );
    }




void Palette::makeUniqueID() {

    partialUniqueID p = startUniqueID();
    
    
    p = addToUniqueID( p,
                       (unsigned char*)mWellColors, mWellDataLength );
    
    p = addToUniqueID( p, (unsigned char*)mName, strlen( mName ) + 1 );
    
    mID = ::makeUniqueID( p );
    }



Image *Palette::getImage() {
    Image *image = new Image( P, P, 3, false );
    
    double *channels[3];
    
    int i;
    
    for( i=0; i<3; i++ ) {
        channels[i] = image->getChannel( i );
        }
    
    rgbaColor black = { { 0, 0, 0, 255 } };
    

    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            int pixel = y * P + x;
            
            int wellRow = y / 2;
            
            // invert
            wellRow = 8 - wellRow - 1;
            int wellColumn = (x-1)/2 - 1;
            
            rgbaColor c = black;
            
            if( wellColumn >=0 && wellColumn < 5 ) {    
                c = mWellColors[ wellRow * 5 + wellColumn ];
                }
            
            
            for( i=0; i<3; i++ ) {

                channels[i][pixel] = c.bytes[i] / 255.0;
                }
            }
        }

    return image;
    }




Sprite *Palette::getSprite( char inUseTrans, char inCacheOK ) {
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



void Palette::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "Palette %s:\n", idString );
    
    delete [] idString;

    printf( "  name: %s\n", mName );
    
    for( int i=0; i<C; i++ ) {
        printf( "  " );
        ::print( mWellColors[i] );
        }
    printf( "\n" );
    }


