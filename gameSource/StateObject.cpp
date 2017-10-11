#include "StateObject.h"
#include "resourceManager.h"
#include "usageDatabase.h"
#include "uniqueID.h"
#include "SpriteResource.h"
#include "imageCache.h"
#include "packSaver.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"


static unsigned char objectVersionNumber = 3;



// static inits
StateObject *StateObject::sBlankObject = NULL;

void StateObject::staticInit() {
    sBlankObject = new StateObject();
    // save once
    sBlankObject->finishEdit();
    }

void StateObject::staticFree() {
    delete sBlankObject;
    }



void StateObject::setupDefault() {
    const char *defaultName = "default";
    memcpy( mName, defaultName, strlen( defaultName ) + 1 );
    
    
    makeUniqueID();
    }




StateObject::StateObject() {
    setupDefault();
    }


void StateObject::initFromData( unsigned char *inData, int inLength,
                           char inFromNetwork ) {

    readFromBytes( inData, inLength );

    if( inFromNetwork ) {

        // sprites might not be present locally, either

        // more efficient to load them in a chunk instead of with separate
        // messages
            
        int numSprites = mSpriteLayers.size();
            

        if( numSprites > 0 ) {
                
            uniqueID *ids = mSpriteLayers.getElementArray();
            
            int *chunkLengths = new int[ numSprites ];
            char *fromNetworkFlags = new char[ numSprites ];
            
                
            unsigned char **chunks = loadResourceData( "sprite",
                                                       numSprites,
                                                       ids,
                                                       chunkLengths,
                                                       fromNetworkFlags );
                
            

            for( int i=0; i<numSprites; i++ ) {
                
                if( chunks[i] != NULL ) {

                    if( fromNetworkFlags[i] ) {
                        // make a sprite
                        SpriteResource r( ids[i], chunks[i], chunkLengths[i] );
                        
                        // force into disk cache, don't update ID
                        r.finishEdit( false );
                        }
                    

                    delete [] chunks[i];
                    }
                
                }

            delete [] ids;

            delete [] fromNetworkFlags;
            delete [] chunkLengths;
            delete [] chunks;
                
            }
            
            
            
            
        // save Object to disk using the ID that we fetched it with
        finishEdit( false );
        }
    }



StateObject::StateObject( uniqueID inID, unsigned char *inData, int inLength,
                          char inFromNetwork )
        : mID( inID ) {
    
    initFromData( inData, inLength, inFromNetwork );
    
    }



// room loaded from file
StateObject::StateObject( uniqueID inID )
        : mID( inID ) {
    
    int length;
    char fromNetwork;
    
    unsigned char *data = loadResourceData( "object", mID, &length,
                                            &fromNetwork );

    
    if( data != NULL ) {
        initFromData( data, length, fromNetwork );
        delete [] data;
        }
    else {
        setupDefault();
        }
    
    
    }
    


int StateObject::getNumLayers() {
    return mSpriteLayers.size();
    }



char StateObject::canAdd( int inNumToAdd ) {
    if( mSpriteLayers.size() + inNumToAdd <= 255 ) {
        return true;
        }
    return false;
    }




char StateObject::addLayer( uniqueID inSpriteID, int inLayerBelow ) {
    if( mSpriteLayers.size() < 255 ) {
        int thisIndex = inLayerBelow + 1;
        
        SimpleVector<uniqueID> displacedIDs;
        SimpleVector<intPair> displacedOffsets;
        SimpleVector<unsigned char> displacedTrans;
        SimpleVector<unsigned char> displacedGlow;
        
        int oldSize = mSpriteLayers.size();
        for( int i=thisIndex; i<oldSize; i++ ) {
            displacedIDs.push_back( *( mSpriteLayers.getElement( i ) ) );
            displacedOffsets.push_back( *( mLayerOffsets.getElement( i ) ) );
            displacedTrans.push_back( *( mLayerTrans.getElement( i ) ) );
            displacedGlow.push_back( *( mLayerGlow.getElement( i ) ) );
            }
        for( int i=oldSize-1; i>=thisIndex; i--) {
            mSpriteLayers.deleteElement( i );
            mLayerOffsets.deleteElement( i );
            mLayerTrans.deleteElement( i );
            mLayerGlow.deleteElement( i );
            }
        

        // displaced layers removed
        // add new layer to end
        mSpriteLayers.push_back( inSpriteID );
        
        intPair p = { 0, 0 };
        mLayerOffsets.push_back( p );

        mLayerTrans.push_back( 255 );
        mLayerGlow.push_back( 0 );
        

        // add displaced after that.
        int numDisplaced = displacedIDs.size();
        for( int i=0; i<numDisplaced; i++ ) {
            mSpriteLayers.push_back( *( displacedIDs.getElement( i ) ) );
            mLayerOffsets.push_back( *( displacedOffsets.getElement( i ) ) );
            mLayerTrans.push_back( *( displacedTrans.getElement( i ) ) );
            mLayerGlow.push_back( *( displacedGlow.getElement( i ) ) );
            }

        return true;
        }
    else {
        AppLog::error( "Error:  256th layer added to full StateObject\n" );
        return false;
        }
    }


void StateObject::deleteLayer( int inLayer ) {
    mSpriteLayers.deleteElement( inLayer );
    
    mLayerOffsets.deleteElement( inLayer );

    mLayerTrans.deleteElement( inLayer );

    mLayerGlow.deleteElement( inLayer );
    }


        
void StateObject::editLayerSprite( int inLayer, uniqueID inSpriteID ) {
    *( mSpriteLayers.getElement( inLayer ) ) = inSpriteID;
    }


        
void StateObject::editLayerOffset( int inLayer, intPair inOffset ) {
    *( mLayerOffsets.getElement( inLayer ) ) = inOffset;
    }


void StateObject::editLayerTrans( int inLayer, unsigned char inTrans ) {
    *( mLayerTrans.getElement( inLayer ) ) = inTrans;
    }


void StateObject::editLayerGlow( int inLayer, char inGlow ) {
    *( mLayerGlow.getElement( inLayer ) ) = (unsigned char)inGlow;
    }



uniqueID StateObject::getLayerSprite( int inLayer ) {
    return *( mSpriteLayers.getElement( inLayer ) );
    }


        
intPair StateObject::getLayerOffset( int inLayer ) {
    return *( mLayerOffsets.getElement( inLayer ) );
    }


unsigned char StateObject::getLayerTrans( int inLayer ) {
    return *( mLayerTrans.getElement( inLayer ) );
    }


char StateObject::getLayerGlow( int inLayer ) {
    return (char)( *( mLayerGlow.getElement( inLayer ) ) );
    }



void StateObject::moveLayerUp( int inLayer ) {

    uniqueID oldID = getLayerSprite( inLayer );
    intPair oldOffset = getLayerOffset( inLayer );
    unsigned char oldTrans = getLayerTrans( inLayer );
    char oldGlow = getLayerGlow( inLayer );
    
    deleteLayer( inLayer );
    

    addLayer( oldID, inLayer );
    
    editLayerOffset( inLayer + 1, oldOffset );
    editLayerTrans( inLayer + 1, oldTrans );
    editLayerGlow( inLayer + 1, oldGlow );
    }



void StateObject::moveLayerDown( int inLayer ) {
    uniqueID oldID = getLayerSprite( inLayer );
    intPair oldOffset = getLayerOffset( inLayer );
    unsigned char oldTrans = getLayerTrans( inLayer );
    char oldGlow = getLayerGlow( inLayer );
    
    deleteLayer( inLayer );
    

    addLayer( oldID, inLayer - 2 );
    
    editLayerOffset( inLayer - 1, oldOffset );
    editLayerTrans( inLayer - 1, oldTrans );
    editLayerGlow( inLayer - 1, oldGlow );
    }



        

void StateObject::editStateObjectName( const char *inName ) {
    int newLength = strlen( inName );
    if( newLength > 10 ) {
        printf( "Error:  StateObject name %s is too long (10 max)\n", inName );
        }
    else {
        memcpy( mName, inName, newLength + 1 );
        }
    }



char *StateObject::getStateObjectName() {
    return stringDuplicate( mName );
    }

    

// finishes the edit, generates a new unique ID, saves result
void StateObject::finishEdit( char inGenerateNewID ) {
    uniqueID oldID = mID;
    
    if( inGenerateNewID ) {
        makeUniqueID();
        }
    
    if( !equal( oldID, mID ) || 
        ! resourceExists( "object", mID ) ) {
        // change
    
        int numBytes;
        unsigned char *bytes = makeBytes( &numBytes );

        saveResourceData( "object", mID, mName, bytes, numBytes );
        delete [] bytes;


        // track usages
        int numSprites = mSpriteLayers.size();
        
        for( int j=0; j<numSprites; j++ ) {
            addUsage( mID, *( mSpriteLayers.getElementFast( j ) ) );
            }
        }
    }


void StateObject::saveToPack() {
    for( int i=0; i<mSpriteLayers.size(); i++ ) {
        SpriteResource s( *( mSpriteLayers.getElement(i) ) );
    
        s.saveToPack();
        }
    
    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );
    
    addToPack( "object", mID, mName, bytes, numBytes );
    delete [] bytes;
    }


        

uniqueID StateObject::getUniqueID() {
    return mID;
    }





Sprite *StateObject::getSprite( char inUseTrans, char inCacheOK ) {

    Image *cachedImage = NULL;
    
    if( inCacheOK ) {
        cachedImage = getCachedImage( mID, inUseTrans );
        }
    
    if( cachedImage == NULL ) {

        int imageW = P*G;
    
        // zero image
        Image fullImage( imageW, imageW, 4, true );
    
        int imageCenter = (P*G)/2;
    

        // track largest rectangle area that is not transparent
        int lowX = imageW;
        int hiX = 0;
        int lowY = imageW;
        int hiY = 0;
    

        int numLayers = mSpriteLayers.size();
    
        for( int i=0; i<numLayers; i++ ) {
        
            SpriteResource r( *( mSpriteLayers.getElement(i) ) );
            intPair layerOffset = *( mLayerOffsets.getElement(i) );
        
            float layerTrans = *( mLayerTrans.getElement(i) ) / 255.0f;
            char layerGlow = *( mLayerGlow.getElement(i) );

            // force trans for sub-sprites, or else weird bg color overlap
            // results
            Image *spriteImage = r.getImage( true );
        
            int spriteCenter = P/2;
        
            for( int y=0; y<P; y++ ) {    
                for( int x=0; x<P; x++ ) {
                
                    int fullY = y - spriteCenter - layerOffset.y;
                    fullY += imageCenter;
                
                    int fullX = x - spriteCenter + layerOffset.x;
                    fullX += imageCenter;
                
                    if( fullY < imageW && fullY >= 0
                        && 
                        fullX < imageW && fullX >= 0 ) {
                
                        int index = y * P + x;
                    
                        Color c = spriteImage->getColor( index );

                        float colorTrans = c.a * layerTrans;
                        
                        if( colorTrans > 0 ) {
                            int fullIndex = fullY * imageW + fullX;
                            Color oldColor = fullImage.getColor( fullIndex );
                            
                            Color *blend;

                            if( oldColor.a > 0 ) {
                                
                                if( ! layerGlow ) {
                                    blend = Color::linearSum(
                                        &c, &( oldColor ),
                                        colorTrans );
                                    }
                                else {
                                    // additive
                                    blend = oldColor.copy();
                                    blend->r += c.r * colorTrans;
                                    blend->g += c.g * colorTrans;
                                    blend->b += c.b * colorTrans;
                                    
                                    // cap
                                    if( blend->r > 1 ) {
                                        blend->r = 1;
                                        }
                                    if( blend->g > 1 ) {
                                        blend->g = 1;
                                        }
                                    if( blend->b > 1 ) {
                                        blend->b = 1;
                                        }
                                    }
                                }
                            else {
                                blend = c.copy();
                                blend->a = colorTrans;
                                }
                            

                            // overwrite with blend of ink from higher layer
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
            // P big enough to contain whole object
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
            
                    for( int c=0; c<4; c++ ) {
                        shrunkChannels[c][shrunkIndex] +=
                            subChannels[c][subIndex];
                        }
                    // anything hit by non-trans color is opaque
                    //shrunkChannels[3][shrunkIndex] = 1;
                    }
                }
            }
    
        for( int c=0; c<4; c++ ) {
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



const char *StateObject::getResourceType() {
    return "object";
    }



StateObject StateObject::getDefaultResource() {
    return *sBlankObject;
    }






#include "minorGems/util/SimpleVector.h"

unsigned char *StateObject::makeBytes( int *outLength ) {
    SimpleVector<unsigned char> buffer;
    
    buffer.push_back( (unsigned char *)SID_MAGIC_CODE, 
                      strlen( SID_MAGIC_CODE ) );
    
    buffer.push_back( objectVersionNumber );



    unsigned char numLayers = (unsigned char)mSpriteLayers.size();
    
    // one byte for num layers (max 255 layers)
    buffer.push_back( numLayers );
    
    for( int i=0; i<numLayers; i++ ) {
        buffer.push_back( mSpriteLayers.getElement(i)->bytes, U );
        
        intPair *p = mLayerOffsets.getElement( i );

        // one byte each coordinate
        buffer.push_back( (unsigned char)p->x );
        buffer.push_back( (unsigned char)p->y );
        
        buffer.push_back( *( mLayerTrans.getElement( i ) ) );

        buffer.push_back( *( mLayerGlow.getElement( i ) ) );
        }
    
    buffer.push_back( (unsigned char *)mName, strlen( mName ) + 1 );
    
    *outLength = buffer.size();
    return buffer.getElementArray();
    }



void StateObject::readFromBytes( unsigned char *inBytes, int inLength ) {
    

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





    if( inLength < 0 ) {
        return;
        }
    
    int numLayers = inBytes[0];
    
    inBytes = &( inBytes[ 1 ] );
    inLength --;
    
    
    int layerDataLength = U+2+1;
    
    if( version < 2 ) {
        layerDataLength = U+2;
        }


    if( inLength < layerDataLength * numLayers ) {
        AppLog::error( 
            "Error: Data stream too short to contain state object.\n" );
        return;
        }
    
    for( int i=0; i<numLayers; i++ ) {
        int numUsed;
        uniqueID id = readUniqueID( inBytes, inLength, &numUsed );
        
        if( numUsed < 0 ) {
            AppLog::error( "Error: reading StateObject's sprite uniqueID failed.\n" );
            return;
            }
        
        mSpriteLayers.push_back( id );
        
        inBytes = &( inBytes[ numUsed ] );
        inLength -= numUsed;
        
        // convert to signed to unpack potentially-negative values
        intPair p = { (char)( inBytes[0] ), (char)( inBytes[1] ) };

        mLayerOffsets.push_back( p );

        inBytes = &( inBytes[ 2 ] );
        inLength -= 2;

        if( version >= 2 ) {
            mLayerTrans.push_back( inBytes[0] );
            inBytes = &( inBytes[ 1 ] );
            inLength -= 1;

            if( version >= 3 ) {
                mLayerGlow.push_back( inBytes[0] );
                inBytes = &( inBytes[ 1 ] );
                inLength -= 1;
                }
            else {
                // fill in with dummy data
                mLayerGlow.push_back( 0 );
                }
            }
        else {
            // fill in with dummy data
            mLayerTrans.push_back( 255 );
            }
        }
    
    // remainder is name
    if( inLength <= 11 ) {    
        memcpy( mName, inBytes, inLength );
        }
    }



void StateObject::makeUniqueID() {

    int numBytes;
    unsigned char *bytes = makeBytes( &numBytes );

    mID = ::makeUniqueID( bytes, numBytes );
    delete [] bytes;
    }



void StateObject::print() {
    char *idString = getHumanReadableString( mID );
    
    printf( "StateObject %s:\n", idString );
    
    delete [] idString;
    

    for( int i=0; i<mSpriteLayers.size(); i++ ) {
        idString = getHumanReadableString( *( mSpriteLayers.getElement(i) ) );

        intPair *p = mLayerOffsets.getElement( i );

        printf( "   Sprite: %s, (%d,%d) [%d] [Glow=%d]\n",
                idString, p->x, p->y, *(mLayerTrans.getElement( i )),
                *(mLayerGlow.getElement( i )) );
        delete [] idString;
        }
    printf( "\n" );
    }
