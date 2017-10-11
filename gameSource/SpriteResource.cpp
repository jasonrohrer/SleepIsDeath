#include "SpriteResource.h"
#include "resourceManager.h"
#include "imageCache.h"
#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"



// static inits
SpriteResource *SpriteResource::sBlankSprite = NULL;

void SpriteResource::staticInit() {
    sBlankSprite = new SpriteResource();
    // ensure that blank sprite is saved at least once
    sBlankSprite->finishEdit();


    /*
    printf( "Generating random sprites...\n" );
    
    // code to generate lots of random sprites
    for( int i=0; i<50000; i++ ) {
        if( i%1000 == 0 ) {
            printf( "...%d...\n", i );
            }
        
        SpriteResource r;
        r.editSpriteName( "manymany" );
        
        // use uniqueID as a random num generator
        uniqueID randID = ::makeUniqueID( (unsigned char*)(&i), 
                                          sizeof( int ) );

        

        rgbaColor c;
        c.comp.r = randID.bytes[0];
        c.comp.g = randID.bytes[1];
        c.comp.b = randID.bytes[2];
        c.comp.a = 255;
        
        r.editSprite( randID.bytes[3] % P, 
                      randID.bytes[4] % P, 
                      c );
        
        r.finishEdit();
        }        
    printf( "...done\n" );
    */
    }

void SpriteResource::staticFree() {
    delete sBlankSprite;
    }



void SpriteResource::setupDefault() {
    
    memset( (void *)mPixelColors, 0, mPixelDataLength );
    
    // default sprite is fully trans
    memset( (void *)mTransFlags, true, mTransDataLength );

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



SpriteResource::SpriteResource() {
    setupDefault();
    }



char SpriteResource::initFromData( unsigned char *inData, int inLength ) {
    if( inLength >= mPixelDataLength ) {
        
        memcpy( (void *)mPixelColors, inData, mPixelDataLength );
        
        inLength -= mPixelDataLength;
        inData = &( inData[ mPixelDataLength ] );
        
        
        if( inLength >= mTransDataLength ) {
            memcpy( (void*)mTransFlags, inData, mTransDataLength );
            
            inLength -= mTransDataLength;
            
            inData = &( inData[mTransDataLength] );
            
            // remainder is name
            if( inLength <= 11 ) {    
                memcpy( mSetName, inData, inLength );

                return true;
                }
            }
        }
    return false;
    }



SpriteResource::SpriteResource( uniqueID inID, 
                                unsigned char *inData, int inLength )
        : mID( inID ) {
    
    if( !initFromData( inData, inLength ) ) {
        // fail
        setupDefault();
        }
    }



SpriteResource::SpriteResource( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;
    
    unsigned char *data = loadResourceData( "sprite", mID, &length,
                                            &fromNetwork );
    if( data != NULL ) {

        if( !initFromData( data, length ) ) {
            // fail
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
        // load failed
        setupDefault();
        }
    
    if( data != NULL ) {
        delete [] data;
        }
    
    }



void SpriteResource::editSprite( int inX, int inY, rgbaColor inNewColor ) {
    mPixelColors[inY][inX] = inNewColor;
    }


rgbaColor SpriteResource::getColor( int inX, int inY ) {
    return mPixelColors[inY][inX];
    }



void SpriteResource::editSpriteName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Sprite name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mSetName, inName, newLength + 1 );
        }
    }


void SpriteResource::editTrans( int inX, int inY, char inIsTrans ){
    mTransFlags[inY][inX] = inIsTrans;
    }


char SpriteResource::getTrans( int inX, int inY ) {
    return mTransFlags[inY][inX];
    }


#include "minorGems/util/stringUtils.h"


char *SpriteResource::getSpriteName() {
    return stringDuplicate( mSetName );
    }



#include "minorGems/util/SimpleVector.h"

void SpriteResource::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    if( ! equal( oldID, mID ) || 
        ! resourceExists( "sprite", mID ) ) {
        
        // change

        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "sprite", mID,
                          mSetName,
                          bytes, numBytes );
                
        delete [] bytes;
        }
    }



unsigned char *SpriteResource::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> dataAccum;
    
    dataAccum.push_back( (unsigned char *)mPixelColors, 
                         mPixelDataLength );

    dataAccum.push_back( (unsigned char *)mTransFlags, 
                         mTransDataLength );
        
    dataAccum.push_back( (unsigned char *)mSetName, 
                         strlen( mSetName ) + 1 );        

    *outLength = dataAccum.size();
    return dataAccum.getElementArray();
    }



void SpriteResource::saveToPack() {
    
    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );

    addToPack( "sprite", mID,
               mSetName,
               bytes, numBytes );
                
    delete [] bytes;
    }


    
        

uniqueID SpriteResource::getUniqueID() {
    return mID;
    }



const char *SpriteResource::getResourceType() {
    return "sprite";
    }

SpriteResource SpriteResource::getDefaultResource() {
    return *( SpriteResource::sBlankSprite );
    }




void SpriteResource::makeUniqueID() {

    partialUniqueID p = startUniqueID();
    
    
    p = addToUniqueID( p,
                       (unsigned char*)mPixelColors, mPixelDataLength );
    p = addToUniqueID( p,
                       (unsigned char*)mTransFlags, mTransDataLength );
    
    p = addToUniqueID( p, (unsigned char*)mSetName, strlen( mSetName ) + 1 );
    
    mID = ::makeUniqueID( p );
    }



Image *SpriteResource::getImage( char inUseTrans ) {
    Image *image = new Image( P, P, 4, false );
    
    double *channels[4];
    
    int i;
    
    for( i=0; i<4; i++ ) {
        channels[i] = image->getChannel( i );
        }
    
    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            int pixel = y * P + x;
            
            rgbaColor c = mPixelColors[y][x];
            
            for( i=0; i<3; i++ ) {

                channels[i][pixel] = c.bytes[i] / 255.0;
                }

            if( mTransFlags[y][x] && inUseTrans ) {
                channels[3][pixel] = 0;
                }
            else {
                channels[3][pixel] = 1;
                }
            }
        }

    return image;
    }




Sprite *SpriteResource::getSprite( char inUseTrans, char inCacheOK ) {
    if( inCacheOK ) {
        
        Image *cachedImage = getCachedImage( mID, inUseTrans );
        
        if( cachedImage == NULL ) {
            cachedImage = getImage();
            
            addCachedImage( mID, inUseTrans, cachedImage );
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


Sprite *SpriteResource::getHighlightSprite() {
    Image *cachedImage = getCachedImage( mID, true );
    
    if( cachedImage == NULL ) {
        cachedImage = getImage();
        
        addCachedImage( mID, true, cachedImage );
        }
    
    
    Image *highlight = cachedImage->copy();
    

    double *channels[4];
    
    int i;

    int numPixels = P * P;
    
    int opaqueCount = 0;
    
    double aveComponent = 0;


    channels[3] = highlight->getChannel( 3 );

    for( i=0; i<3; i++ ) {
        channels[i] = highlight->getChannel( i );
        
        for( int p=0; p<numPixels; p++ ) {
            if( channels[3][p] > 0 ) {
                
                double val = channels[i][p];
            
                aveComponent += val;
                opaqueCount += 1;
                }
            }
        }
    
    aveComponent /= opaqueCount;
    
    double opposite = 0;
    
    if( aveComponent < 0.5 ) {
        opposite = 1;
        }
    
    double same = 1 - opposite;
    

    /*
    // leave alpha alone
    for( i=0; i<3; i++ ) {
        channels[i] = highlight->getChannel( i );
        
        for( int p=0; p<numPixels; p++ ) {
            double val = channels[i][p];
            
            if( val > 0.5 ) {
                val = 0;
                }
            else {
                val = 1.0;
                }
            
            channels[i][p] = val;
            }
        }
    */

    // make an outline based on alpha

    char outline[P*P];
    
    char edge[P*P];
    

    for( int y=0; y<P; y++ ) {
        for( int x=0; x<P; x++ ) {
            
            int index = y * P + x;
            
            outline[index] = 0;
            edge[index] = 0;
            
            if( channels[3][ index ] == 0 ) {
                // trans spot
                
                // does it have non-trans neighbor?
                if( y > 0 && channels[3][ index - P ] != 0 ) {
                    outline[index] = 1;
                    }
                else if( y < P-1 && channels[3][ index + P ] != 0 ) {
                    outline[index] = 1;
                    }
                else if( x > 0 && channels[3][ index - 1 ] != 0 ) {
                    outline[index] = 1;
                    }
                else if( x < P-1 && channels[3][ index + 1 ] != 0 ) {
                    outline[index] = 1;
                    }
                }
            else {
                // opaque
            
                // does it touch edge?
                if( y == 0 || y == P-1 ||
                    x == 0 || x == P-1 ) {
                
                    edge[index] = 1;
                    }
                
    
                }
            
            }
        }
    

    // fill in outline with same color, edges with opposite
    for( i=0; i<3; i++ ) {
        channels[i] = highlight->getChannel( i );
        
        for( int p=0; p<numPixels; p++ ) {
            if( outline[p] ) {
                channels[i][p] = same;
                channels[3][p] = 1;
                }
            else if( edge[p] ) {
                channels[i][p] = opposite;
                channels[3][p] = 1;
                }
            else {
                channels[3][p] = 0;
                }            
            }
        }
    

    Sprite *s = new Sprite( highlight );
    delete highlight;
    
    return s;
    }



void SpriteResource::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "SpriteResource %s:\n", idString );
    
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


