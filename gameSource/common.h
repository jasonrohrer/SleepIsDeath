#ifndef COMMON_INCLUDED
#define COMMON_INCLUDED


// width of a grid square
#define P   16

// number of grid squares on screen is GxG (square)
#define G   13

// number of wells in a color palette
#define C   40


// undo stack sizes
#define MAX_UNDOS  256


// code used at the start of some resource files as an identifier
#define SID_MAGIC_CODE "SiD1977"



#include "minorGems/graphics/Image.h"

//#include <SDL/SDL.h>

// reads a TGA file from the default ("graphics") folder
Image *readTGA( const char *inFileName );


Image *readTGA( const char *inFolderName, const char *inFileName );


// write to the current directory
void writeTGA( Image *inImage, char *inFileName );



// maps linear values in [0,1] to smoothed [0,1] using sine
double smoothBlend( double inValue );



typedef struct intPair {
        int x;
        int y;
    } intPair;


char equals( intPair inA, intPair inB );

intPair add( intPair inA, intPair inB );

intPair subtract( intPair inA, intPair inB );


void print( intPair inA );


// generates a bytes string representing a pair
// result is statically-allocated and overwritten with each subsequent call 
// (NOT thread safe)
unsigned char *getChars( intPair inA );

// reads pair from data string
intPair readIntPair( unsigned char *inBytes, int inLength,
                     int *outNumUsed );


unsigned char *getChars( int inA );

int readInt( unsigned char *inBytes, int inLength, int *outNumUsed );


#endif
