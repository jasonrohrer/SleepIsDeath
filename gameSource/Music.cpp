#include "Music.h"
#include "resourceManager.h"

#include "imageCache.h"

#include "packSaver.h"

#include "minorGems/util/log/AppLog.h"


// for grid display, color subsequent octaves
Color lowColor( 0, 146.0/255, 219.0/255 );
Color medColor( 146.0/255, 219.0/255, 109/255 );
Color hiColor( 219.0/255, 146.0/255, 0 );


// for sprites
// from Scale.cpp
extern Color toneLowColor;
extern Color toneHighColor;




// static inits
Music *Music::sBlankMusic = NULL;

void Music::staticInit() {
    sBlankMusic = new Music();
    // ensure that blank music is saved at least once
    sBlankMusic->finishEdit();
    }

void Music::staticFree() {
    delete sBlankMusic;
    }




void Music::setupDefault() {
    memset( (void *)mNotes, 0, mNoteDataLength );
    

    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );

    makeUniqueID();
    }



Music::Music() {
    setupDefault();
    }



char Music::initFromData( unsigned char *inData, int inLength ) {
    if( inLength >= mNoteDataLength ) {
        
        memcpy( (void *)mNotes, inData, mNoteDataLength );
        
        inLength -= mNoteDataLength;
        
        // remainder is name
        if( inLength <= 11 ) {    
            memcpy( mName, &inData[mNoteDataLength], inLength );
            return true;
            }
        }
    return false;
    }



Music::Music( uniqueID inID, unsigned char *inData, int inLength )
        : mID( inID ) {
    
    if( !initFromData( inData, inLength ) ) {
        // fail
        setupDefault();
        }
    }




Music::Music( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;

    unsigned char *data = loadResourceData( "music", mID, &length, 
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



void Music::editMusic( int inX, int inY, char inNoteOn ) {
    mNotes[inY][inX] = inNoteOn;
    }



char Music::getNoteOn( int inX, int inY ) {
    return mNotes[inY][inX];
    }



void Music::editMusicName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Music set name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }


#include "minorGems/util/stringUtils.h"


char *Music::getMusicName() {
    return stringDuplicate( mName );
    }



#include "minorGems/util/SimpleVector.h"


unsigned char *Music::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> dataAccum;
    
    dataAccum.push_back( (unsigned char *)mNotes, 
                         mNoteDataLength );
    
    dataAccum.push_back( (unsigned char *)mName, 
                         strlen( mName ) + 1 );
    
        
    *outLength = dataAccum.size();
    
    return dataAccum.getElementArray();
    }

    


void Music::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    
    if( ! equal( oldID, mID ) || 
        ! resourceExists( "music", mID ) ) {
        
        // change
    
        int dataSize;
        unsigned char *data = makeBytes( &dataSize );
        
        saveResourceData( "music", mID,
                          mName,
                          data, dataSize );
                
        delete [] data;
        }
    }


    
void Music::saveToPack() {
    int dataSize;
    unsigned char *data = makeBytes( &dataSize );
    
    addToPack( "music", mID,
               mName,
               data, dataSize );
    
    delete [] data;
    }

    
        

uniqueID Music::getUniqueID() {
    return mID;
    }



const char *Music::getResourceType() {
    return "music";
    }

Music Music::getDefaultResource() {
    return *( Music::sBlankMusic );
    }




void Music::makeUniqueID() {

    partialUniqueID p = startUniqueID();
    
    
    p = addToUniqueID( p,
                       (unsigned char*)mNotes, mNoteDataLength );
    
    p = addToUniqueID( p, (unsigned char*)mName, strlen( mName ) + 1 );
    
    mID = ::makeUniqueID( p );
    }



Image *Music::getImage() {
    Image *image = new Image( N, N, 4, true );
    
    double *channels[4];
    
    int i;
    
    for( i=0; i<4; i++ ) {
        channels[i] = image->getChannel( i );
        }

    Color *drawColor = &lowColor;

    //Color *drawColorA = &lowColor;//&toneLowColor; 
    //Color *drawColorB = &medColor;//&toneHighColor;
    Color drawColorA( 0.50, 0.50, 0.50 );
    Color drawColorB( 1.0, 1.0, 1.0 );
    
        
    for( int y=0; y<N; y++ ) {
        if( y>4 ) {
            drawColor = &medColor;
            }
        if( y>9 ) {
            drawColor = &hiColor;
            }

        Color *levelColor = Color::linearSum( &drawColorB, &drawColorA,
                                              y / (float)N );

        int pixY = N - y - 1;
        
        for( int x=0; x<N; x++ ) {
            int pixel = pixY * N + x;
            
            if( mNotes[y][x] ) {
                for( i=0; i<3; i++ ) {    
                    channels[i][pixel] = (*levelColor)[i];
                    }
                channels[3][pixel] = 1.0;
                }
            
            }
        delete levelColor;
        }

    return image;
    }




Sprite *Music::getSprite( char inUseTrans, char inCacheOK ) {
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



void Music::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "Music %s:\n", idString );
    
    delete [] idString;

    printf( "  set name: %s\n", mName );
    
    for( int y=0; y<N; y++ ) {
        printf( "  " );
        
        for( int x=0; x<N; x++ ) {
            printf( "%c ", mNotes[y][x] );
            }
        printf( "\n" );
        }
    }


