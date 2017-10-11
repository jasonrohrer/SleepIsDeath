#include "Scale.h"
#include "resourceManager.h"

#include "imageCache.h"

#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"



Color toneLowColor( 0, 85.0/255, 128.0/255 );
Color toneHighColor( 0, 170.0/255, 1.0 );




// static inits
Scale *Scale::sBlankScale = NULL;

void Scale::staticInit() {
    sBlankScale = new Scale();
    // ensure that blank timbre is saved at least once
    sBlankScale->finishEdit();
    }

void Scale::staticFree() {
    delete sBlankScale;
    }




void Scale::setupDefault() {
    memset( (void *)mHalftonesOn, 0, HT );

    // default to minor pentatonic
    mHalftonesOn[0] = 1;
    mHalftonesOn[3] = 1;
    mHalftonesOn[5] = 1;
    mHalftonesOn[7] = 1;
    mHalftonesOn[10] = 1;
    
    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );

    makeUniqueID();
    }



Scale::Scale() {
    setupDefault();
    }



char Scale::initFromData( unsigned char *inData, int inLength ) {
    if( inLength >= mDataLength ) {
        
        memcpy( (void *)mHalftonesOn, inData, HT );
        
        inLength -= HT;

        // remainder is name
        if( inLength <= 11 ) {    
            memcpy( mName, &inData[mDataLength], inLength );
            return true;
            }
        }
    return false;
    }



Scale::Scale( unsigned char *inData, int inLength )
        : mID( defaultID ) {
    
    if( !initFromData( inData, inLength ) ) {
        // fail
        setupDefault();
        }
    }




Scale::Scale( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;

    unsigned char *data = loadResourceData( "scale", mID, &length, 
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
        // scale failed to load
        setupDefault();
        }

    if( data != NULL ) {
        delete [] data;
        }
    }



void Scale::editScale( int inHalftone, char inToneOn ) {
    mHalftonesOn[inHalftone] = (unsigned char)inToneOn;
    }



char Scale::getToneOn( int inHalftone ) {
    return mHalftonesOn[inHalftone];
    }



int Scale::getNumOn() {
    int sum = 0;
    for( int y=0; y<HT; y++ ) {
        if( mHalftonesOn[y] ) {
            sum++;
            }
        }
    
    return sum;
    }



void Scale::editScaleName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Scale name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }


#include "minorGems/util/stringUtils.h"


char *Scale::getScaleName() {
    return stringDuplicate( mName );
    }



#include "minorGems/util/SimpleVector.h"


unsigned char *Scale::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> dataAccum;
    
    dataAccum.push_back( mHalftonesOn, HT );
    
    dataAccum.push_back( (unsigned char *)mName, 
                         strlen( mName ) + 1 );
    
        
    *outLength = dataAccum.size();
    
    return dataAccum.getElementArray();
    }

    


void Scale::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    
    if( ! equal( oldID, mID ) || 
        ! resourceExists( "scale", mID ) ) {
        
        // change
    
        int dataSize;
        unsigned char *data = makeBytes( &dataSize );
        
        saveResourceData( "scale", mID,
                          mName,
                          data, dataSize );
                
        delete [] data;
        }
    }


    
void Scale::saveToPack() {
    int dataSize;
    unsigned char *data = makeBytes( &dataSize );
    
    addToPack( "scale", mID,
               mName,
               data, dataSize );
    
    delete [] data;
    }

    
        

uniqueID Scale::getUniqueID() {
    return mID;
    }



const char *Scale::getResourceType() {
    return "scale";
    }

Scale Scale::getDefaultResource() {
    return *( Scale::sBlankScale );
    }




void Scale::makeUniqueID() {

    partialUniqueID p = startUniqueID();
    
    
    p = addToUniqueID( p, mHalftonesOn, HT );    

    p = addToUniqueID( p, (unsigned char*)mName, strlen( mName ) + 1 );
    
    mID = ::makeUniqueID( p );
    }



Image *Scale::getImage() {
    Image *image = new Image( P, P, 3, true );
    
    double *channels[3];
    
    int i;
    
    for( i=0; i<3; i++ ) {
        channels[i] = image->getChannel( i );
        }

    Color *drawColorA = &toneLowColor; 
    Color *drawColorB = &toneHighColor;
       
    for( int y=0; y<HT; y++ ) {

        int pixY = HT - y - 1;
        
        pixY += 2;
        
        // diagonal
        int x = y;
        
        x += 2;
        
        int pixel = pixY * P + x;
            
        if( mHalftonesOn[y] ) {
            Color *levelColor = Color::linearSum( drawColorB, drawColorA,
                                                  y / (float)HT );
            for( i=0; i<3; i++ ) {    
                channels[i][pixel] = (*levelColor)[i];
                }
            delete levelColor;
            }
        }

    return image;
    }




Sprite *Scale::getSprite( char inUseTrans, char inCacheOK ) {
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



void Scale::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "Scale %s:\n", idString );
    
    delete [] idString;

    printf( "  name: %s\n", mName );
    
    printf( "  " );
    for( int h=0; h<HT; h++ ) {
        printf( "%c ", mHalftonesOn[h] );    
        }
    printf( "\n" );

    }




void Scale::setInPlayer( ) {
    setScale( (char*)mHalftonesOn );
    }



