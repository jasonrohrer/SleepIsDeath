#include "Song.h"
#include "Music.h"
#include "TimbreResource.h"
#include "Scale.h"
#include "resourceManager.h"
#include "usageDatabase.h"
#include "uniqueID.h"

#include "imageCache.h"
#include "packSaver.h"


#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"





// static inits
Song *Song::sBlankSong = NULL;

void Song::staticInit() {
    sBlankSong = new Song();
    // save once
    sBlankSong->finishEdit();
    }

void Song::staticFree() {
    delete sBlankSong;
    }



void Song::setupDefault() {
    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );
    

    uniqueID blankID = Music::sBlankMusic->getUniqueID();
    
    int x;
    int y;
    for( y=0; y<SI; y++ ) {
        mRowLengths[y] = 0;
        mTimbres[y] = TimbreResource::sBlankTimbre->getUniqueID();
        for( x=0; x<S; x++ ) {
            mPhrases[y][x] = blankID;
            }
        mRowLoudness[y] = 255;
        mRowStereo[y] = 128;
        }
    
    mScale = Scale::sBlankScale->getUniqueID();

    // normal, default
    mSpeed = 1;
    
    makeUniqueID();
    }




Song::Song() {
    setupDefault();
    }




// song loaded from file
Song::Song( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;
    
    unsigned char *data = loadResourceData( "song", mID, &length,
                                            &fromNetwork );

    if( data != NULL ) {
        // save this one
        initFromData( data, length, fromNetwork, true );
        
        
        delete [] data;
        data = NULL;
        }
    else {
        setupDefault();
        }
    }    


Song::Song( uniqueID inID, unsigned char *inData, int inLength,
            char inFromNetwork, char inSaveWholeSong )
        : mID( inID ) {
    
    initFromData( inData, inLength, inFromNetwork, inSaveWholeSong );
    }




void Song::initFromData( unsigned char *inData, int inLength, 
                         char inFromNetwork, char inSaveWholeSong ) {
        
    readFromBytes( inData, inLength );
    
    if( inFromNetwork ) {
            
        // phrases might not be present locally, either

        // more efficient to load them in a chunk instead of with separate
        // messages
            
            
        uniqueID ids[SI*S];
            
        int i=0;
            
        for( int ty=0; ty<SI; ty++ ) {
            for( int tx=0; tx<S; tx++ ) {
                ids[i] = mPhrases[ty][tx];
                i++;
                }
            }
            
        int chunkLengths[SI*S];
        char fromNetworkFlags[SI*S];
            

        unsigned char **chunks = loadResourceData( "music",
                                                   SI*S,
                                                   ids,
                                                   chunkLengths,
                                                   fromNetworkFlags );
            
        for( int i=0; i<SI*S; i++ ) {
                
            if( chunks[i] != NULL ) {

                if( fromNetworkFlags[i] ) {
                    // make a phrase
                    Music m( ids[i], chunks[i], chunkLengths[i] );
                        
                    // force into disk cache, don't remake ID
                    m.finishEdit( false );
                    }
                    

                delete [] chunks[i];
                }
                
            }
        delete [] chunks;
            


        i=0;
            
        for( int ty=0; ty<SI; ty++ ) {
            ids[i] = mTimbres[ty];
            i++;
            }
            

        chunks = loadResourceData( "timbre",
                                   SI,
                                   ids,
                                   chunkLengths,
                                   fromNetworkFlags );
            
        for( int i=0; i<SI; i++ ) {
                
            if( chunks[i] != NULL ) {

                if( fromNetworkFlags[i] ) {
                    // make a timbre
                    TimbreResource t( ids[i], chunks[i], chunkLengths[i] );
                        
                    // force into disk cache, don't remake ID
                    t.finishEdit( false );
                    }
                    

                delete [] chunks[i];
                }
                
            }
        delete [] chunks;
            

        // force scale to be saved
        Scale s( mScale );
            



        if( inSaveWholeSong ) {    
            // save song to disk using the ID that we fetched it with
            finishEdit( false );
            }
        
        }
        
    }




void Song::editSongPhrase( int inX, int inY, uniqueID inPhraseID ) {
    mPhrases[inY][inX] = inPhraseID;
    }

        
void Song::editRowLength( int inY, int inLength ){
    mRowLengths[inY] = (unsigned char)inLength;
    }


void Song::editTimbre( int inY, uniqueID inTimbreID ) {
    mTimbres[inY] = inTimbreID;
    }


void Song::editRowLoudness( int inY, int inLoudness ){
    mRowLoudness[inY] = (unsigned char)inLoudness;
    }


void Song::editRowStereo( int inY, int inStereo ){
    mRowStereo[inY] = (unsigned char)inStereo;
    }


void Song::editScale( uniqueID inScale ) {
    mScale = inScale;
    }


void Song::editSpeed( int inSpeed ) {
    mSpeed = (unsigned char)inSpeed;
    }


        

void Song::editSongName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Song name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }



uniqueID Song::getPhrase( int inX, int inY ) {
    return mPhrases[inY][inX];
    }


int Song::getRowLength( int inY ) {
    return mRowLengths[inY];
    }


int Song::getRowLoudness( int inY ) {
    return mRowLoudness[inY];
    }

int Song::getRowStereo( int inY ) {
    return mRowStereo[inY];
    }


uniqueID Song::getTimbre( int inY ) {
    return mTimbres[inY];
    }


uniqueID Song::getScale() {
    return mScale;
    }


int Song::getSpeed() {
    return mSpeed;
    }



char *Song::getSongName() {
    return stringDuplicate( mName );
    }

    

// finishes the edit, generates a new unique ID, saves result
void Song::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    
    if( !equal( oldID, mID ) || 
        ! resourceExists( "song", mID ) ) {
        // change
    
        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "song", mID, mName, bytes, numBytes );
        delete [] bytes;


        // track usages
        addUsage( mID, mScale );
        
        for( int y=0; y<SI; y++ ) {
            
            addUsage( mID, mTimbres[y] );
        
            for( int x=0; x<S; x++ ) {
                addUsage( mID, mPhrases[y][x] );
                }
            }

        }
    }



void Song::saveToPack() {
    for( int py=0; py<SI; py++ ) {
        
        for( int px=0; px<S; px++ ) {
    
            Music m( mPhrases[py][px] );
            m.saveToPack();
            }
        TimbreResource t( mTimbres[py] );
        t.saveToPack();
        }
    Scale s( mScale );
    s.saveToPack();
    

    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );
    
    addToPack( "song", mID, mName, bytes, numBytes );
    delete [] bytes;
    }

        

uniqueID Song::getUniqueID() {
    return mID;
    }


Image *Song::getImage() {
    // compose all phrase images into a sprite with 1 pixel per phrase
    
    // zero to prepare for average
    Image *fullImage = new Image( P, P, 3, true );

    double *fullC[3] = 
        { fullImage->getChannel( 0 ), 
          fullImage->getChannel( 1 ), 
          fullImage->getChannel( 2 ) };
    
    

    for( int py=0; py<SI; py++ ) {
        
        // color by timbre
        TimbreResource timbre( getTimbre( py ) );
        
        Color timbreColor = toColor( timbre.getTimbreColor() );
        

        for( int px=0; px<S; px++ ) {
            
            int fullP = (py + 1) * P + (px + 1);

            Music m( mPhrases[py][px] );
            
            Image *tImage = m.getImage();
            
            
            double *tC[4] = 
                { tImage->getChannel( 0 ), 
                  tImage->getChannel( 1 ), 
                  tImage->getChannel( 2 ),
                  tImage->getChannel( 3 ) };

            // compute average of all non-trans phrase pixels

            for( int c=0; c<3; c++ ) {
                // sum first
                int numNonTrans = 0;

                for( int i=0; i<P*P; i++ ) {
                    if( tC[3][i] > 0 ) {
                        fullC[c][fullP] += tC[c][i];
                        numNonTrans ++;
                        }
                    }
                if( numNonTrans > 0 ) {
                    fullC[c][fullP] /= numNonTrans;
                    fullC[c][fullP] *= (timbreColor)[c];
                    }
                else {
                    fullC[c][fullP] = 0;
                    }
                }
            delete tImage;
            }
        }
    
    return fullImage;
    }



Sprite *Song::getSprite( char inUseTrans, char inCacheOK ) {
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



const char *Song::getResourceType() {
    return "song";
    }



Song Song::getDefaultResource() {
    return *sBlankSong;
    }






#include "minorGems/util/SimpleVector.h"

unsigned char *Song::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> buffer;
    
    int y, x;
    
    for( y=0; y<SI; y++ ) {
        for( x=0; x<S; x++ ) {
            buffer.push_back( mPhrases[y][x].bytes, U );
            }
        }


    for( y=0; y<SI; y++ ) {
        buffer.push_back( mRowLengths[y] );
        }
 
    for( y=0; y<SI; y++ ) {
        buffer.push_back( mTimbres[y].bytes, U  );
        }

    for( y=0; y<SI; y++ ) {
        buffer.push_back( mRowLoudness[y] );
        }

    for( y=0; y<SI; y++ ) {
        buffer.push_back( mRowStereo[y] );
        }

    buffer.push_back( mScale.bytes, U );

    buffer.push_back( mSpeed );
    
   
    buffer.push_back( (unsigned char *)mName, strlen( mName ) + 1 );
    
    *outLength = buffer.size();
    return buffer.getElementArray();
    }



void Song::readFromBytes( unsigned char *inBytes, int inLength ) {

    int numBytesUsed;
    char success = readUniqueIDs( (uniqueID *)mPhrases, SI*S, 
                                  inBytes, inLength,
                                  &numBytesUsed );
    if( success ) {
        inBytes = &( inBytes[ numBytesUsed ] );
        inLength -= numBytesUsed;
        }
    
    if( inLength >= SI ) {
        
        memcpy( (void *)mRowLengths, inBytes, SI );

        inBytes = &( inBytes[ SI ] );
        inLength -= SI;
        }
    
    success = readUniqueIDs( (uniqueID *)mTimbres, SI, 
                             inBytes, inLength,
                             &numBytesUsed );

    if( success ) {
        inBytes = &( inBytes[ numBytesUsed ] );
        inLength -= numBytesUsed;
        }

    if( inLength >= SI ) {
        
        memcpy( (void *)mRowLoudness, inBytes, SI );

        inBytes = &( inBytes[ SI ] );
        inLength -= SI;
        }

    if( inLength >= SI ) {
        
        memcpy( (void *)mRowStereo, inBytes, SI );

        inBytes = &( inBytes[ SI ] );
        inLength -= SI;
        }

    mScale = readUniqueID( inBytes, inLength,
                           &numBytesUsed );
    if( numBytesUsed == U ) {
        inBytes = &( inBytes[ numBytesUsed ] );
        inLength -= numBytesUsed;
        }
    
    if( inLength > 0 ) {
        mSpeed = inBytes[0];
        inBytes = &( inBytes[ 1 ] );
        inLength -= 1;
        }
    
        

    // remainder is name
    if( inLength <= 11 ) {    
        memcpy( mName, inBytes, inLength );
        }
    }



void Song::makeUniqueID() {

    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );

    mID = ::makeUniqueID( bytes, numBytes );
    delete [] bytes;
    }





#include "musicPlayer.h"

extern char noteToggles[PARTS][S][N][N];

extern int partLengths[PARTS];
extern double partLoudness[PARTS];
extern double partStereo[PARTS];


void Song::setInPlayer( Song inOldSong, Song inNewSong, char inForceUpdate ) {
    
    // timbre updates are slow... only do the ones we need
    uniqueID oldTimbres[SI];
    for( int y=0; y<SI; y++ ) {
        oldTimbres[y] = inOldSong.getTimbre( y );
        }
    
    // update ALL on a scale change or speed change
    uniqueID oldScale = inOldSong.getScale();
    uniqueID newScale = inNewSong.getScale();
    
    char scaleChange = ( !equal( oldScale, newScale ) ) || inForceUpdate;
    
    if( scaleChange ) {
        Scale s( newScale );
        s.setInPlayer();
        }

    char speedChange = ( inOldSong.getSpeed() != inNewSong.getSpeed() ) 
        || inForceUpdate;
    if( speedChange ) {    
        setSpeed( inNewSong.getSpeed() );
        }
    
    // speed change forces everything to update

    for( int y=0; y<SI; y++ ) {
        partLengths[y] = inNewSong.getRowLength( y );


        TimbreResource t( inNewSong.getTimbre( y ) );
        
        for( int x=0; x<S; x++ ) {
            Music m( inNewSong.getPhrase( x, y ) );
            
            if( inNewSong.getRowLength( y ) > x ) {
                for( int py=0; py<N; py++ ) {
                    for( int px=0; px<N; px++ ) {   
                        noteToggles[y][x][py][px] = 
                            m.getNoteOn( px, py );
                        }
                    }
                }
            else {
                // turn rest off
                for( int py=0; py<N; py++ ) {
                    for( int px=0; px<N; px++ ) {   
                        noteToggles[y][x][py][px] = false;
                        }
                    }
                }
            }
        
        if( !equal( t.getUniqueID(), oldTimbres[y] ) ) {
            // a true timbre change, set both timbre and envelope
            t.setInPlayer( y, true, true );
            }
        else if( speedChange ) {
            // just a speed change, keep timbres
            t.setInPlayer( y, false, true );
            }
        else if( scaleChange ) {
            // just a scale change, keep envelopes
            t.setInPlayer( y, true, false );
            }

        
        partLoudness[y] = inNewSong.getRowLoudness( y ) / 255.0;
        partStereo[y] = inNewSong.getRowStereo( y ) / 255.0;
        }
    
    }
