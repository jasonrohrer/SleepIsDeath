#include "TimbreResource.h"
#include "resourceManager.h"

#include "imageCache.h"

#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"



Color harmonicLowColor( 0, 85.0/255, 128.0/255 );
Color harmonicHighColor( 0, 170.0/255, 1.0 );




// static inits
TimbreResource *TimbreResource::sBlankTimbre = NULL;

void TimbreResource::staticInit() {
    sBlankTimbre = new TimbreResource();
    // ensure that blank timbre is saved at least once
    sBlankTimbre->finishEdit();
    }

void TimbreResource::staticFree() {
    delete sBlankTimbre;
    }




void TimbreResource::setupDefault() {
    memset( (void *)mHarmonicLevels, 0, F );
   
    mAttack = 0;
    mHold = 255;
    mRelease = 0;
    
    mOctavesDown = 1;
    

    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );

    makeUniqueID();
    }



TimbreResource::TimbreResource() {
    setupDefault();
    }



char TimbreResource::initFromData( unsigned char *inData, int inLength ) {
    if( inLength >= mDataLength ) {
        
        memcpy( (void *)mHarmonicLevels, inData, F );
        
        inLength -= F;
        
        mAttack = inData[ F ];
        mHold = inData[ F + 1 ];
        mRelease = inData[ F + 2 ];
        mOctavesDown = inData[ F + 3 ];
        
        inLength -= 4;


        // remainder is name
        if( inLength <= 11 ) {    
            memcpy( mName, &inData[mDataLength], inLength );
            return true;
            }
        }
    return false;
    }



TimbreResource::TimbreResource( uniqueID inID, 
                                unsigned char *inData, int inLength )
        : mID( inID ) {
    
    if( !initFromData( inData, inLength ) ) {
        // fail
        setupDefault();
        }
    }




TimbreResource::TimbreResource( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;

    unsigned char *data = loadResourceData( "timbre", mID, &length, 
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
        // music failed to load
        setupDefault();
        }

    if( data != NULL ) {
        delete [] data;
        }
    }



void TimbreResource::editHarmonic( int inHarmonic, int inLevel ) {
    mHarmonicLevels[inHarmonic] = (unsigned char)inLevel;
    }



int TimbreResource::getHarmonicLevel( int inHarmonic ) {
    return mHarmonicLevels[inHarmonic];
    }



void TimbreResource::editEnvelope( int inAttack, int inHold,
                                   int inRelease ) {
    mAttack = (unsigned char)inAttack;
    mHold = (unsigned char)inHold;
    mRelease = (unsigned char)inRelease;
    }


void TimbreResource::editOctavesDown( int inOctavesDown ) {
    mOctavesDown = inOctavesDown;
    }


        
int TimbreResource::getAttack() {
    return mAttack;
    }

int TimbreResource::getHold() {
    return mHold;
    }

int TimbreResource::getRelease() {
    return mRelease;
    }


int TimbreResource::getOctavesDown() {
    return mOctavesDown;
    }



rgbaColor TimbreResource::getTimbreColor() {
    
    float r, g, b;
    r = 0;
    g = 0;
    b = 0;
    
    for( int y=0; y<F; y++ ) {
        float h;
        
        if( y % 2 == 0 ) {
            
            // lower half of spectrum
            h = ( y / 2 ) / (float)F;
            }
        else {
            // upper half
            h = ( y / 2 ) / (float)F;
            h += 0.5;
            }
                
        Color *c = Color::makeColorFromHSV( h, 1, 1 );
        
        r += mHarmonicLevels[y] * c->r;
        g += mHarmonicLevels[y] * c->g;
        b += mHarmonicLevels[y] * c->b;
        
        delete c;
        }

    // normalize
    float max = 0;
    if( r>max ) {
        max = r;
        }
    if( g>max ) {
        max = g;
        }
    if( b>max ) {
        max = b;
        }
    if( max == 0 ) {
        // default color for no harmonics
        r = 1.0;
        g = 0;
        b = 0;
        }
    else {    
        r /= max;
        g /= max;
        b /= max;
        }
    
    
    double octaveFade = 1.0 - mOctavesDown * 0.05;
    
    r *= octaveFade;
    g *= octaveFade;
    b *= octaveFade;

    rgbaColor returnVal = { { (unsigned char)(r * 255),  
                              (unsigned char)(g * 255),
                              (unsigned char)(b * 255) } };
    
    return returnVal;
    }

        




void TimbreResource::editTimbreName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  TimbreResource name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }


#include "minorGems/util/stringUtils.h"


char *TimbreResource::getTimbreName() {
    return stringDuplicate( mName );
    }



#include "minorGems/util/SimpleVector.h"


unsigned char *TimbreResource::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> dataAccum;
    
    dataAccum.push_back( mHarmonicLevels, F );

    dataAccum.push_back( mAttack );
    dataAccum.push_back( mHold );
    dataAccum.push_back( mRelease );
    dataAccum.push_back( mOctavesDown );
    
    dataAccum.push_back( (unsigned char *)mName, 
                         strlen( mName ) + 1 );
    
        
    *outLength = dataAccum.size();
    
    return dataAccum.getElementArray();
    }

    


void TimbreResource::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    
    if( ! equal( oldID, mID ) || 
        ! resourceExists( "timbre", mID ) ) {
        
        // change
    
        int dataSize;
        unsigned char *data = makeBytes( &dataSize );
        
        saveResourceData( "timbre", mID,
                          mName,
                          data, dataSize );
                
        delete [] data;
        }
    }


    
void TimbreResource::saveToPack() {
    int dataSize;
    unsigned char *data = makeBytes( &dataSize );
    
    addToPack( "timbre", mID,
               mName,
               data, dataSize );
    
    delete [] data;
    }

    
        

uniqueID TimbreResource::getUniqueID() {
    return mID;
    }



const char *TimbreResource::getResourceType() {
    return "timbre";
    }

TimbreResource TimbreResource::getDefaultResource() {
    return *( TimbreResource::sBlankTimbre );
    }




void TimbreResource::makeUniqueID() {

    partialUniqueID p = startUniqueID();
    
    
    p = addToUniqueID( p, mHarmonicLevels, F );
    
    p = addToUniqueID( p, &mAttack, 1 );
    p = addToUniqueID( p, &mHold, 1 );
    p = addToUniqueID( p, &mRelease, 1 );
    p = addToUniqueID( p, &mOctavesDown, 1 );
    

    p = addToUniqueID( p, (unsigned char*)mName, strlen( mName ) + 1 );
    
    mID = ::makeUniqueID( p );
    }



Image *TimbreResource::getImage() {
    Image *image = new Image( FL, F, 3, true );
    
    double *channels[3];
    
    int i;
    
    for( i=0; i<3; i++ ) {
        channels[i] = image->getChannel( i );
        }

    //Color *drawColorA = &harmonicLowColor; 
    //Color *drawColorB = &harmonicHighColor;
    
    rgbaColor fullHotC = getTimbreColor();
    rgbaColor fullC = fullHotC;
    fullC.comp.r = (unsigned char)( fullC.comp.r * 0.50 );
    fullC.comp.g = (unsigned char)( fullC.comp.g * 0.50 );
    fullC.comp.b = (unsigned char)( fullC.comp.b * 0.50 );


    Color drawColorA( fullC.comp.r / 255.0f,
                      fullC.comp.g / 255.0f,
                      fullC.comp.b / 255.0f );
    Color drawColorB( fullHotC.comp.r / 255.0f,
                      fullHotC.comp.g / 255.0f,
                      fullHotC.comp.b / 255.0f );
    
                      

    for( int y=0; y<F; y++ ) {

        int pixY = F - y - 1;
        
        for( int x=0; x<FL; x++ ) {
            int pixel = pixY * F + x;
            
            if( mHarmonicLevels[y] > x ) {
                Color *levelColor = Color::linearSum( &drawColorB, &drawColorA,
                                                      x / (float)FL );
                for( i=0; i<3; i++ ) {    
                    channels[i][pixel] = (*levelColor)[i];
                    }
                delete levelColor;
                }
            
            }
        }

    return image;
    }




Sprite *TimbreResource::getSprite( char inUseTrans, char inCacheOK ) {
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



void TimbreResource::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "TimbreResource %s:\n", idString );
    
    delete [] idString;

    printf( "  name: %s\n", mName );
    
    printf( "  " );
    for( int h=0; h<F; h++ ) {
        printf( "%c ", mHarmonicLevels[h] );    
        }
    printf( "\n" );

    }




void TimbreResource::setInPlayer( int inTimbreNumber,
                                  char inUpdatePlayerTimbre,
                                  char inUpdatePlayerEnvelope ) {
    if( inUpdatePlayerEnvelope ) {
        
        setEnvelope( inTimbreNumber, 
                     getAttack() / 255.0, 
                     getHold() / 255.0,
                     getRelease() / 255.0 );
        }
    

    if( inUpdatePlayerTimbre ) {
        

        // pass new coeffs to music player
        // loop through set harmonics three times to get extra, 
        //   high-order harmonics, plus one coeff for fundamental
        #define NUM_COEFFS 3*FL + 1
    
        double coeffs[NUM_COEFFS];
        int numUsed = 1;
        coeffs[0] = 1.0;
        //printf( "coef %d = %f\n", 0, coeffs[0] );

        for( int i=1; i<NUM_COEFFS; i++ ) {
            coeffs[i] = 0;
            }
    

        for( int i=1; i<49; i++ ) {
        
            int x = (i-1) % FL;
            //int y = (i-1)/16;
        


            coeffs[i] = getHarmonicLevel( x ) / (double)(i+1);

            if( coeffs[i] > 0 ) {
                numUsed = i+1;
                }
            //printf( "coef %d = %f\n", i, coeffs[i] );
        
            }
       
        setTimbre( inTimbreNumber, coeffs, numUsed, mOctavesDown );
        }
    }



