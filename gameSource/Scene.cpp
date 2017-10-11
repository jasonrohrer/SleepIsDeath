#include "Scene.h"
#include "resourceManager.h"
#include "usageDatabase.h"
#include "Room.h"
#include "StateObject.h"
#include "SpriteResource.h"
#include "imageCache.h"
#include "packSaver.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"


static unsigned char sceneVersionNumber = 2;


// static inits
Scene *Scene::sBlankScene = NULL;

void Scene::staticInit() {
    sBlankScene = new Scene();
    // save once
    sBlankScene->finishEdit();
    }

void Scene::staticFree() {
    delete sBlankScene;
    }



void Scene::setupDefault() {
    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );

    mRoom = Room::sBlankRoom->getUniqueID();

    mRoomTrans = 255;
        
    mObjectZeroFrozen = false;
    
    // okay that there are ZERO objects in the default,
    // because when loaded against a blank game state, that game state
    // will contain the default object position for Obj-Zero anyway,
    // and we simply won't change it (because we have zero objects!)

    // No need to duplicate default object position and other attributes here.
    
    
    makeUniqueID();
    }




Scene::Scene() {
    setupDefault();
    }




// room loaded from file
Scene::Scene( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;
    
    unsigned char *data = loadResourceData( "scene", mID, &length,
                                            &fromNetwork );

    
    if( data != NULL ) {
        readFromBytes( data, length );
    
        delete [] data;

        if( fromNetwork ) {
            // room might not be present locally
            // force it to load
            Room loadedRoom( mRoom );
            


            // objects might not be present locally, either

            // more efficient to load them in a chunk instead of with separate
            // messages
            
            int numObjects = mObjects.size();
            

            if( numObjects > 0 ) {
                
                uniqueID *ids = mObjects.getElementArray();
            
                int *chunkLengths = new int[ numObjects ];
                char *fromNetworkFlags = new char[ numObjects ];
            
                
                unsigned char **chunks = loadResourceData( "object",
                                                           numObjects,
                                                           ids,
                                                           chunkLengths,
                                                           fromNetworkFlags );
                
            

                for( int i=0; i<numObjects; i++ ) {
                
                    if( chunks[i] != NULL ) {

                        if( fromNetworkFlags[i] ) {
                            // make an object
                            StateObject o( ids[i], 
                                           chunks[i], chunkLengths[i], true );
                        
                            // saves itself to disk, since fromNetwork flag
                            // passed to constructor
                            }
                    

                        delete [] chunks[i];
                        }
                
                    }

                delete [] ids;

                delete [] fromNetworkFlags;
                delete [] chunkLengths;
                delete [] chunks;
                
                }
            
            
            
            
            // save Scene to disk using the ID that we fetched it with
            finishEdit( false );
            }
        }
    else {
        setupDefault();
        }
    
    
    }



void Scene::saveToPack() {
    Room room( mRoom );
    
    room.saveToPack();
    
    for( int i=0; i<mObjects.size(); i++ ) {
        StateObject o( *( mObjects.getElement(i) ) );
    
        o.saveToPack();
        }
    
    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );
    
    addToPack( "scene", mID, mName, bytes, numBytes );
    delete [] bytes;
    }




void Scene::editSceneName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  Scene name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }


char *Scene::getSceneName() {
    return stringDuplicate( mName );
    }

    

// finishes the edit, generates a new unique ID, saves result
void Scene::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    
    if( !equal( oldID, mID ) || 
        ! resourceExists( "scene", mID ) ) {
        // change
    
        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "scene", mID, mName, bytes, numBytes );
        delete [] bytes;


        // track usages
        addUsage( mID, mRoom );

        int numObjects = mObjects.size();
        
        for( int j=0; j<numObjects; j++ ) {
            addUsage( mID, *( mObjects.getElementFast( j ) ) );
            }

        }
    }

        

uniqueID Scene::getUniqueID() {
    return mID;
    }



#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"


Sprite *Scene::getSprite( char inUseTrans, char inCacheOK ) {

    Image *cachedImage = NULL;
    
    if( inCacheOK ) {
        cachedImage = getCachedImage( mID, inUseTrans );
        }
    
    if( cachedImage == NULL ) {

        int imageW = P*G;
    
        // zero image
        Image fullImage( imageW, imageW, 4, true );
    
        //int imageCenter = (P*G)/2;
    

        // track largest rectangle area that is not transparent
        int lowX = imageW;
        int hiX = 0;
        int lowY = imageW;
        int hiY = 0;

        // first, lay room in

        Room room( mRoom );
        
        float roomTrans = mRoomTrans / 255.0f;

        
        for( int ty=0; ty<G; ty++ ) {
            
            for( int tx=0; tx<G; tx++ ) {
                
                uniqueID tID = room.getTile( tx, ty );
                
                // skip blanks
                if( !equal( tID, Room::sBlankTile->getUniqueID() ) ) {
                    
                    Tile t( tID );
                
                
                    Image *tImage = t.getImage();
                
                    for( int py=0; py<P; py++ ) {
                        int fullY = py + ty * P;
                        
                        for( int px=0; px<P; px++ ) {
                            int fullX = px + tx * P;
                            
                            Color c = tImage->getColor( py * P + px );
                            
                            // darken based on trans
                            c.r *= roomTrans;
                            c.g *= roomTrans;
                            c.b *= roomTrans;
                            
                            fullImage.setColor( fullY * imageW + fullX, 
                                                c );

                            if( fullX < lowX ) {
                                lowX = fullX;
                                }
                            if( fullX > hiX ) {
                                hiX = fullX;
                                }
                            if( fullY < lowY ) {
                                lowY = fullY;
                                }
                            if( fullY > hiY ) {
                                hiY = fullY;
                                }
                            }
                        }
                    delete tImage;
                    }
                    
                }
            }
        
        
        /*
        // FIXME lay tile sprites in... actually, should be okay to lay in
        // big 16x16 blocks of color (taken from room thumbnail sprite)
        // because that is what would happen with area mapping reduction anyway
        
        Image *roomImage = room.getImage();

        double *roomChannels[3] =
            { roomImage->getChannel( 0 ),
              roomImage->getChannel( 1 ),
              roomImage->getChannel( 2 ) };
         
        double *fullChannels[4] =
            { fullImage.getChannel( 0 ),
              fullImage.getChannel( 1 ),
              fullImage.getChannel( 2 ),
              fullImage.getChannel( 3 ) };


        double roomTrans = mRoomTrans / 255.0f;
        
        for( int y=0; y<imageW; y++ ) {
            int gy = y / P;
            
            for( int x=0; x<imageW; x++ ) {
                int gx = x / P;
                
                // image is PxP, even though room is only GxG
                // padded with black
                int gi = gy * P + gx;
                
                int fullI = y * imageW + x;
                
                // ignore black areas
                if( roomChannels[0][gi] > 0 ||
                    roomChannels[1][gi] > 0 ||
                    roomChannels[2][gi] > 0 ) {
                    
                    // track areas hit with color
                    if( x < lowX ) {
                        lowX = x;
                        }
                    if( x > hiX ) {
                        hiX = x;
                        }
                    if( y < lowY ) {
                        lowY = y;
                        }
                    if( y > hiY ) {
                        hiY = y;
                        }
                    
                    for( int c=0; c<3; c++ ) {
                        // darken based on trans
                        fullChannels[c][fullI] = 
                            roomTrans * roomChannels[c][gi];
                        }
                    fullChannels[3][fullI] = 1.0f;                    
                    }
                }
            }            
        
        delete roomImage;
        */
        
        
        int numObjects = mObjects.size();
        
        for( int j=0; j<numObjects; j++ ) {

            StateObject obj( *( mObjects.getElement( j ) ) );
            
            intPair objOffset = *( mObjectOffsets.getElement( j ) );
            
            float objTrans = *( mObjectTrans.getElement( j ) ) / 255.0f;
            

            int numLayers = obj.getNumLayers();
    
            for( int i=0; i<numLayers; i++ ) {
                
                SpriteResource r( obj.getLayerSprite( i ) );
                intPair layerOffset = obj.getLayerOffset( i );
        
                float layerTrans = obj.getLayerTrans( i ) / 255.0f;
                
                layerTrans *= objTrans;
                

                // force trans for sub-sprites, or else weird bg color overlap
                // results
                Image *spriteImage = r.getImage( true );
        
                int spriteCenter = P/2;
        
                for( int y=0; y<P; y++ ) {    
                    for( int x=0; x<P; x++ ) {
                
                        int fullY = y - spriteCenter - layerOffset.y - 
                            objOffset.y;
                        fullY += imageW - spriteCenter;
                        //fullY += imageCenter;
                        
                        int fullX = x - spriteCenter + layerOffset.x + 
                            objOffset.x;
                        fullX += spriteCenter;
                        //fullX += imageCenter;
                
                        if( fullY < imageW && fullY >= 0
                            && 
                            fullX < imageW && fullX >= 0 ) {
                
                            int index = y * P + x;
                            
                            Color c = spriteImage->getColor( index );
                    
                            float colorTrans = c.a * layerTrans;
                            

                            if( colorTrans > 0 ) {

                                int fullIndex = fullY * imageW + fullX;
                                Color oldColor = 
                                    fullImage.getColor( fullIndex );
                            
                                Color *blend;

                                if( oldColor.a > 0 ) {
                                
                                    blend = Color::linearSum(
                                        &c, &( oldColor ),
                                        colorTrans );
                                    }
                                else {
                                    blend = c.copy();
                                    blend->a = colorTrans;
                                    }
                                // overwrite with blend of ink from higher 
                                // layer

                                fullImage.setColor( fullIndex, *blend );

                                delete blend;
                                

                                if( fullX < lowX ) {
                                    lowX = fullX;
                                    }
                                if( fullX > hiX ) {
                                    hiX = fullX;
                                    }
                                if( fullY < lowY ) {
                                    lowY = fullY;
                                    }
                                if( fullY > hiY ) {
                                    hiY = fullY;
                                    }
                                }
                            }
                        
                        }
                    }
                

                delete spriteImage;
                }
            }
        
        /*
        PNGImageConverter png;
        
        File pngFile( NULL, "fullTest.png" );
        FileOutputStream out( &pngFile );
        png.formatImage( &fullImage, &out );
        */
    
        int subW = hiX - lowX + 1;
        int subH = hiY - lowY + 1;
    
        Image *subImage = fullImage.getSubImage( lowX, lowY, 
                                                 subW, subH );
    
    


        //printf( "subimage = x,y=(%d,%d), w,h=(%d,%d)\n", 
        //   lowX, lowY, subW, subH );
    
        // build up sums here
        Image *shrunkImage = new Image( P, P, 4, true );
        double *subChannels[4];
        double *shrunkChannels[4];
        int hitCount[P*P];
        memset( hitCount, 0, P*P*sizeof(int) );
    
        for( int c=0; c<4; c++ ) {
            subChannels[c] = subImage->getChannel( c );
            shrunkChannels[c] = shrunkImage->getChannel( c );
            }

        double scaleFactor;
    
        if( subW > subH ) {
            scaleFactor = P / (double)subW;
            }
        else {
            scaleFactor = P / (double)subH;
            }

        if( scaleFactor > 1 ) {
            // P big enough to contain whole scene
            scaleFactor = 1;
            }
    

        // center in shrunk image
        int xExtra = (int)( P - ( scaleFactor * subW ) );
        int yExtra = (int)( P - ( scaleFactor * subH ) );
    
        int xOffset = xExtra / 2;    
        int yOffset = yExtra / 2;
    


        for( int y=0; y<subH; y++ ) {
            int shrunkY = (int)( y * scaleFactor ) + yOffset;
        
            for( int x=0; x<subW; x++ ) {
                int shrunkX = (int)( x * scaleFactor ) + xOffset;
            
                int shrunkIndex = shrunkY * P + shrunkX;
                int subIndex = y * subW + x;
            

                // skip transparent pixels
                if( subChannels[3][subIndex] > 0 ) {
                
                    hitCount[shrunkIndex] ++;
            
                    for( int c=0; c<3; c++ ) {
                        shrunkChannels[c][shrunkIndex] +=
                            subChannels[c][subIndex];
                        }
                    // anything hit by non-trans color is opaque
                    shrunkChannels[3][shrunkIndex] = 1;
                    }
                }
            }
    


        for( int c=0; c<3; c++ ) {
            for( int i=0; i<P*P; i++ ) {
                // transparent pixels hit 0 times
                if( hitCount[i] > 0 ) {
                    shrunkChannels[c][i] /= hitCount[i];
                    }
                }
            }
    
        // sharp transparency already computed above 
        // (skipped trans pixels in sums)
    


        delete subImage;

        cachedImage = shrunkImage;
        
        if( inCacheOK ) {
            addCachedImage( mID, inUseTrans, cachedImage );
            }
        }
    
    
    
    Sprite *sprite = new Sprite( cachedImage );

    if( !inCacheOK ) {
        // image not in cache... we must destroy it
        delete cachedImage;
        }

    return sprite;
    }



const char *Scene::getResourceType() {
    return "scene";
    }



Scene Scene::getDefaultResource() {
    return *sBlankScene;
    }






#include "minorGems/util/SimpleVector.h"

unsigned char *Scene::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> buffer;

    buffer.push_back( (unsigned char *)SID_MAGIC_CODE, 
                      strlen( SID_MAGIC_CODE ) );
    
    buffer.push_back( sceneVersionNumber );
    
    
    
    buffer.push_back( mRoom.bytes, U );

    buffer.push_back( mRoomTrans );

    buffer.push_back( (unsigned char)mObjectZeroFrozen );




    unsigned char numObjects = (unsigned char)mObjects.size();
    
    // one byte for num objects (max 255 objects)
    buffer.push_back( numObjects );
    
    for( int i=0; i<numObjects; i++ ) {
        buffer.push_back( mObjects.getElement(i)->bytes, U );
        
        intPair *p = mObjectOffsets.getElement( i );
        
        // full precision
        buffer.push_back( getChars( *p ), 8 );
        
        p = mSpeechOffsets.getElement( i );

        buffer.push_back( getChars( *p ), 8 );

        buffer.push_back( *( mSpeechFlipFlags.getElement( i ) ) );
        buffer.push_back( *( mSpeechBoxFlags.getElement( i ) ) );
        buffer.push_back( *( mLockedFlags.getElement( i ) ) );
        buffer.push_back( *( mObjectTrans.getElement( i ) ) );
        }
    
    buffer.push_back( (unsigned char *)mName, strlen( mName ) + 1 );
    
    *outLength = buffer.size();
    return buffer.getElementArray();
    }



void Scene::readFromBytes( unsigned char *inBytes, int inLength ) {

    unsigned char version = 1;
    
    // check for magic code before version number
    const char *code = SID_MAGIC_CODE;
    int codeLength = strlen( code );
    
    if( inLength >= codeLength ) {
        char codeFound = true;
        
        
        for( int i=0; i<codeLength && codeFound; i++ ) {
            
            if( inBytes[i] != code[i] ) {
                codeFound = false;
                }
            }
        
        if( codeFound ) {
            // next byte is file version number
        
            version = inBytes[codeLength];
            // skip them
            inBytes = &( inBytes[ codeLength + 1 ] );
            inLength -= ( codeLength + 1 );
            }

        // else no version number, data starts right at beginning
        }



    int numUsed;
    mRoom = readUniqueID( inBytes, inLength, &numUsed );
        
    if( numUsed < 0 ) {
        AppLog::error( "Error: reading Scene's room uniqueID failed.\n" );
        return;
        }
        
    inBytes = &( inBytes[ numUsed ] );
    inLength -= numUsed;


    if( inLength < 0 ) {
        return;
        }

    mRoomTrans = inBytes[0];
    
    inBytes = &( inBytes[ 1 ] );
    inLength --;



    if( inLength < 0 ) {
        return;
        }

    mObjectZeroFrozen = inBytes[0];
    
    inBytes = &( inBytes[ 1 ] );
    inLength --;


    if( inLength < 0 ) {
        return;
        }

    
    int numObjects = inBytes[0];
    
    inBytes = &( inBytes[ 1 ] );
    inLength --;
    
    
    int objDataLength = U+8+8+1+1+1+1;
    
    if( version < 2 ) {
        objDataLength = U+8+8+1+1;
        }
    

    
    if( inLength < objDataLength * numObjects ) {
        AppLog::error( "Error: Data stream too short to contain scene.\n" );
        return;
        }
    
    for( int i=0; i<numObjects; i++ ) {
        uniqueID id = readUniqueID( inBytes, inLength, &numUsed );
        
        if( numUsed < 0 ) {
            AppLog::error( "Error: reading Scene's object uniqueID failed.\n" );
            return;
            }
        
        mObjects.push_back( id );
        
        inBytes = &( inBytes[ numUsed ] );
        inLength -= numUsed;
        

        // full precision
        intPair p = readIntPair( inBytes, inLength,
                                 &numUsed );        
        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse Scene from message\n" );
            return;
            }
        inLength -= numUsed;
        inBytes = &( inBytes[ numUsed ] );


        mObjectOffsets.push_back( p );




        // full precision
        p = readIntPair( inBytes, inLength,
                         &numUsed );        
        if( numUsed == -1 ) {
            AppLog::error( "Failed to parse Scene from message\n" );
            return;
            }
        inLength -= numUsed;
        inBytes = &( inBytes[ numUsed ] );

        mSpeechOffsets.push_back( p );


        mSpeechFlipFlags.push_back( inBytes[0] );

        inBytes = &( inBytes[ 1 ] );
        inLength -= 1;


        if( version >= 2 ) {
            
            mSpeechBoxFlags.push_back( inBytes[0] );

            inBytes = &( inBytes[ 1 ] );
            inLength -= 1;
        
            mLockedFlags.push_back( inBytes[0] );
            
            inBytes = &( inBytes[ 1 ] );
            inLength -= 1;
            }
        else {
            // fill in with dummy data
            mSpeechBoxFlags.push_back( 0 );
            mLockedFlags.push_back( 0 );
            }
        

        mObjectTrans.push_back( inBytes[0] );

        inBytes = &( inBytes[ 1 ] );
        inLength -= 1;
        }
    
    // remainder is name
    if( inLength <= 11 ) {    
        memcpy( mName, inBytes, inLength );
        }
    }



void Scene::makeUniqueID() {

    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );

    mID = ::makeUniqueID( bytes, numBytes );
    delete [] bytes;
    }



void Scene::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "Scene %s:\n", idString );
    
    delete [] idString;
    

    for( int i=0; i<mObjects.size(); i++ ) {
        idString = getHumanReadableString( *( mObjects.getElement(i) ) );

        intPair *p = mObjectOffsets.getElement( i );

        printf( "   Object: %s, (%d,%d)\n",
                idString, p->x, p->y );
        delete [] idString;
        }
    printf( "\n" );
    }
