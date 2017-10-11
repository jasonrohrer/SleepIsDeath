
#include "uniqueID.h"

#include "minorGems/crypto/hashes/sha1.h"
#include "minorGems/util/log/AppLog.h"


#include <string.h>
#include <stdio.h>


uniqueID defaultID = { { 0, 0, 0, 
                         0, 0, 0 } };


        


uniqueID makeUniqueID( unsigned char *inData, int inLength ) {
    
    uniqueID returnValue;
    

    unsigned char *hash = computeRawSHA1Digest( inData, inLength );
    
    // only first U bytes of hash used
    memcpy( returnValue.bytes, hash, U );
    

    delete [] hash;
    

    return returnValue;
    }



partialUniqueID startUniqueID() {
    SHA_CTX ctx;
    SHA1_Init( &ctx );
    return ctx;
    }


partialUniqueID addToUniqueID( partialUniqueID inPartial, 
                               unsigned char *inData, int inLength ) {
    // update overwites data
    unsigned char *buffer = new unsigned char[inLength];
    memcpy( buffer, inData, inLength );

    SHA1_Update( &inPartial, buffer, inLength );

    delete [] buffer;
    
    return inPartial;
    }


uniqueID makeUniqueID( partialUniqueID inPartial ) {
    uniqueID returnValue;

    unsigned char resultBuffer[ SHA1_DIGEST_LENGTH ];

    SHA1_Final( resultBuffer, &inPartial );

    memcpy( returnValue.bytes, resultBuffer, U );

    return returnValue;
    }




uniqueID readUniqueID( unsigned char *inBytes, int inLength,
                       int *outNumUsed ) {
    uniqueID returnValue;
    
    if( inLength >= U ) {
        memcpy( returnValue.bytes, inBytes, U );
        *outNumUsed = U;
        }
    else {
        AppLog::error( 
            "ERROR:  not enough bytes in data string for a uniqueID\n" );
        *outNumUsed = -1;
        }
    
    return returnValue;
    }



char readUniqueIDs( uniqueID *outDest, int inNumToRead,
                    unsigned char *inBytes, int inLength,
                    int *outNumUsed ) {
    
    *outNumUsed = 0;
    
    for( int i=0; i<inNumToRead; i++ ) {
        if( inLength < U ) {
            // falure
            *outNumUsed = -1;
            return false;
            }
        int numUsed = 0;
        
        outDest[i] = readUniqueID( inBytes, inLength, &numUsed );
        
        if( numUsed != -1 ) {
            
            // skip in string
            inLength -= numUsed;
            inBytes = &( inBytes[numUsed] );
            
            *outNumUsed += numUsed;
            }
        else {
            *outNumUsed = -1;
            return false;
            }        
        }
    
  
    return true;
    }





char equal( uniqueID inA, uniqueID inB ) {
    // opt (found with profiler:  
    // don't loop
    return
        inA.bytes[0] == inB.bytes[0]
        &&
        inA.bytes[1] == inB.bytes[1]
        &&
        inA.bytes[2] == inB.bytes[2]
        &&
        inA.bytes[3] == inB.bytes[3]
        &&
        inA.bytes[4] == inB.bytes[4]
        &&
        inA.bytes[5] == inB.bytes[5];
    
    /*
    for( int b=0; b<U; b++ ) {
        if( inA.bytes[b] != inB.bytes[b] ) {
            return false;
            }
        }
    return true;
    */
    }


// for hex encoding
#include "minorGems/formats/encodingUtils.h"

char *getHumanReadableString( uniqueID inID ) {

    return hexEncode( inID.bytes, U );
    }


char parseHumanReadableString( char *inString, uniqueID *outResult ) {
    
    if( strlen( inString ) != U * 2 ) {
        return false;
        }
    
    
    unsigned char *raw = hexDecode( inString );
    
    if( raw == NULL ) {
        return false;
        }
    

    memcpy( outResult->bytes, raw, U );
    
    delete [] raw;
    
    return true;
    }

    



