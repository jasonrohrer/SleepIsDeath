#include "Timbre.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"

#include <math.h>
#include <stdio.h>


double twelthRootOfTwo = pow( 2, 1.0/12 );

// #define SCALE_SIZE 7

// for major scale
// W, W, H, W, W, W, H
//int halfstepMap[ SCALE_SIZE ] = { 0, 2, 4, 5, 7, 9, 11 };

// minor scale
// W,H,W,W,H,W,W
//int halfstepMap[ SCALE_SIZE ] = { 0, 2, 3, 5, 7, 8, 10 };

#define MAX_SCALE_SIZE 12

int halfstepMap[ MAX_SCALE_SIZE ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };


int usedScaleNotes = 5;


void setDefaultScale() {
    usedScaleNotes = 5;
    // major pentatonic
    //int halfstepMap[ SCALE_SIZE ] = { 0, 2, 4, 7, 9 };
    
    // minor pentatonic
    //int halfstepMap[ SCALE_SIZE ] = { 0, 3, 5, 7, 10 };
    halfstepMap[0] = 0;
    halfstepMap[1] = 3;
    halfstepMap[2] = 5;
    halfstepMap[3] = 7;
    halfstepMap[4] = 10;
    }


void setTimbreScale( char inToneOn[12] ) {
    usedScaleNotes = 0;
    
    int nextStep = 0;
    for( int i=0; i<12; i++ ) {
        if( inToneOn[i] ) {
            halfstepMap[nextStep] = i;
            usedScaleNotes ++;
            nextStep++;
            }
        }
    printf( "halfstep map= " );
    for( int i=0; i<usedScaleNotes; i++ ) {
        printf( "%d ", halfstepMap[i] );
        }
    printf( "\n" );
    
    }






// gets frequency of note in our scale
double getFrequency( double inBaseFrequency, int inScaleNoteNumber ) {
    if( usedScaleNotes == 0 ) {
        return inBaseFrequency;
        }
    
    int octavesUp = inScaleNoteNumber / usedScaleNotes;
    
    int numHalfsteps = 
        halfstepMap[ inScaleNoteNumber % usedScaleNotes ] + octavesUp * 12;
    
    return inBaseFrequency * pow( twelthRootOfTwo, numHalfsteps );
    }




/*
  Was used during testing

#include "minorGems/sound/formats/aiff.h"

int outputFileNumber = 0;


// outputs a wave table as an AIFF
void outputWaveTable( Sint16 *inTable, int inLength, int inSampleRate ) {
    // generate the header
    int headerSize;
    unsigned char *aiffHeader =
        getAIFFHeader( 1,
                       16,
                       inSampleRate,
                       inLength, 
                       &headerSize );

    char *fileName = autoSprintf( "waveTable%d.aiff", outputFileNumber );
    outputFileNumber++;
    

    FILE *aiffFile = fopen( fileName, "wb" );

    delete [] fileName;
    

    //printf( "Header size = %d\n", headerSize );
    
    fwrite( aiffHeader, 1, headerSize, aiffFile );

    delete [] aiffHeader;
    
    for( int i=0; i<inLength; i++ ) {
        Sint16 val = inTable[i];
        
        unsigned char msb = val >> 8 & 0xFF;
        unsigned char lsb = val && 0xFF;

        fwrite( &msb, 1, 1, aiffFile );
        fwrite( &lsb, 1, 1, aiffFile );
        }
    fclose( aiffFile );
    
    }
*/




Timbre::Timbre( int inSampleRate,
                double inLoudness,
                double inBaseFrequency,
                int inNumWaveTableEntries, 
                double( *inWaveFunction )( double ),
                int inPeriodsPerTable )
        : mNumWaveTableEntries( inNumWaveTableEntries ),
          mWaveTable( new Sint16*[ inNumWaveTableEntries ] ),
          mWaveTableLengths( new int[ inNumWaveTableEntries ] ),
          mActiveNoteCount( 0 ) {

    // build wave table for each possible pitch in image
    
    for( int i=0; i<mNumWaveTableEntries; i++ ) {
        double freq = getFrequency( inBaseFrequency, i );

        if( freq > inSampleRate / 2 ) {
            // cap at Nyquist limit
            freq = inSampleRate / 2;
            }
        


        double period = 1.0 / freq;
        
        
        int tableLength = (int)( 
            rint( inPeriodsPerTable * period * inSampleRate ) );
        
        if( tableLength == 0 ) {
            AppLog::getLog()->logPrintf( Log::CRITICAL_ERROR_LEVEL,
                                         "table length 0 for freq %f\n", 
                                         freq );
            }
        
        mWaveTableLengths[i] = tableLength;
        mWaveTable[i] = new Sint16[ tableLength ];

        // store double samples in temp table so we can compute
        // max value for normalization
        double *tempTable = new double[ tableLength ];
        double maxValue = 0;
        
        int s;
        
        for( s=0; s<tableLength; s++ ) {
            //double t = (double)s / (double)inSampleRate;
            //double waveValue = inWaveFunction( 2 * M_PI * t * freq );
        
            // base t on table length to ensure a perfect set of periods
            // in our table.  Otherwise, we hear clicks when table is looped
            double t = (double)s / (double)(tableLength);
            double waveValue = 
                inWaveFunction( 2 * M_PI * t * inPeriodsPerTable );
 
            tempTable[s] = waveValue;
            
            // track max value
            if( waveValue > maxValue ) {
                maxValue = waveValue;
                }
            else if( -waveValue > maxValue ) {
                maxValue = -waveValue;
                }
            }
        //printf( "max value = %f\n", maxValue );
        
        // now normalize and convert to int
        for( s=0; s<tableLength; s++ ) {
            double waveValue = tempTable[s] * inLoudness / maxValue;
            
            // convert to int
            mWaveTable[i][s] = (Sint16)( 32767 * waveValue );
            }
        
        delete [] tempTable;
        
                
        mWaveTableLengths[i] = tableLength;
        

        // to examine waveforms for testing
        // outputWaveTable( mWaveTable[i], tableLength, inSampleRate );
        
        }
    }


        
Timbre::~Timbre() {
    delete [] mWaveTableLengths;
    
    for( int i=0; i<mNumWaveTableEntries; i++ ) {
        delete [] mWaveTable[i];
        }
    delete [] mWaveTable;
    }

