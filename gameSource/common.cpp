#include "common.h"

#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/File.h"

#include "minorGems/io/file/FileInputStream.h"

#include "minorGems/util/log/AppLog.h"


#include <math.h>
#include <stdio.h>



Image *readTGA( const char *inFileName ) {
    return readTGA( "graphics", inFileName );
    }



Image *readTGA( const char *inFolderName, const char *inFileName ) {
    File tgaFile( new Path( inFolderName ), inFileName );
    FileInputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    Image *result = converter.deformatImage( &tgaStream );
    
    if( result == NULL ) {
        char *logString = autoSprintf( 
            "CRITICAL ERROR:  could not read TGA file %s, wrong format?",
            inFileName );
        AppLog::criticalError( logString );
        delete [] logString;
        }
    return result;
    }


void writeTGA( Image *inImage, char *inFileName ) {
    File tgaFile( NULL, inFileName );
    FileOutputStream tgaStream( &tgaFile );
    
    TGAImageConverter converter;
    
    return converter.formatImage( inImage, &tgaStream );
    }




double smoothBlend( double inValue ) {
    

    return 1 - ( sin( inValue * M_PI
                      +
                      M_PI / 2 ) 
                 + 1 ) / 2;
    }


char equals( intPair inA, intPair inB ) {
    return inA.x == inB.x && inA.y == inB.y;
    }



intPair add( intPair inA, intPair inB ) {
    intPair returnVal;
    returnVal.x = inA.x + inB.x;
    returnVal.y = inA.y + inB.y;
    
    return returnVal;
    }



intPair subtract( intPair inA, intPair inB ) {
    intPair returnVal;
    returnVal.x = inA.x - inB.x;
    returnVal.y = inA.y - inB.y;
    
    return returnVal;
    }


void print( intPair inA ) {
    printf( "{%d,%d}", inA.x, inA.y );
    }



static unsigned char pairCharBuffer[8];

unsigned char *getChars( intPair inA ) {
    
    pairCharBuffer[0] = ( inA.x >> 24 ) & 0xFF;
    pairCharBuffer[1] = ( inA.x >> 16 ) & 0xFF;
    pairCharBuffer[2] = ( inA.x >> 8 ) & 0xFF;
    pairCharBuffer[3] = ( inA.x ) & 0xFF;

    pairCharBuffer[4] = ( inA.y >> 24 ) & 0xFF;
    pairCharBuffer[5] = ( inA.y >> 16 ) & 0xFF;
    pairCharBuffer[6] = ( inA.y >> 8 ) & 0xFF;
    pairCharBuffer[7] = ( inA.y ) & 0xFF;

    return pairCharBuffer;
    }


intPair readIntPair( unsigned char *inBytes, int inLength,
                     int *outNumUsed ) {
    
    intPair p;

    if( inLength >= 8 ) {
        p.x = 
            inBytes[0] << 24 |
            inBytes[1] << 16 |
            inBytes[2] << 8 |
            inBytes[3];
        p.y = 
            inBytes[4] << 24 |
            inBytes[5] << 16 |
            inBytes[6] << 8 |
            inBytes[7];
        
        *outNumUsed = 8;
        }
    else {
        AppLog::error( 
            "ERROR:  not enough bytes in data string for an intPair" );
        *outNumUsed = -1;
        }
    
    return p;
    }



unsigned char *getChars( int inA ) {
    
    pairCharBuffer[0] = ( inA >> 24 ) & 0xFF;
    pairCharBuffer[1] = ( inA >> 16 ) & 0xFF;
    pairCharBuffer[2] = ( inA >> 8 ) & 0xFF;
    pairCharBuffer[3] = ( inA ) & 0xFF;

    return pairCharBuffer;
    }


int readInt( unsigned char *inBytes, int inLength,
             int *outNumUsed ) {
    
    int a;

    if( inLength >= 4 ) {
        a = 
            inBytes[0] << 24 |
            inBytes[1] << 16 |
            inBytes[2] << 8 |
            inBytes[3];
        
        *outNumUsed = 4;
        }
    else {
        AppLog::error( 
            "ERROR:  not enough bytes in data string for an int" );
        *outNumUsed = -1;
        }
    
    return a;
    }

