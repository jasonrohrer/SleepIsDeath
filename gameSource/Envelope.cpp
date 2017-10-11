#include "Envelope.h"

#include <stdio.h>

#include "minorGems/util/log/AppLog.h"


// #include <assert.h>


Envelope::Envelope( double inAttackTime, double inDecayTime, 
                    double inSustainLevel, double inReleaseTime,
                    int inMinNoteLengthInGridSteps,
                    int inMaxNoteLengthInGridSteps,
                    int inGridStepDurationInSamples )
        : mActiveNoteCount( 0 ),
          mGridStepDurationInSamples( inGridStepDurationInSamples ),
          mMinEnvNum( inMinNoteLengthInGridSteps ),
          mNumComputedEnvelopes( inMaxNoteLengthInGridSteps ),
          mEvelopeLengths( new int[ inMaxNoteLengthInGridSteps ] ),
          mComputedEnvelopes( new double*[ inMaxNoteLengthInGridSteps ] ) {
    
    for( int i=mMinEnvNum-1; i<mNumComputedEnvelopes; i++ ) {
        int length = (i+1) * inGridStepDurationInSamples;
        mEvelopeLengths[i] = length;
        
        mComputedEnvelopes[i] = new double[ length ];

        for( int s=0; s<length; s++ ) {
            
            double t = s / (double)( length - 1 );
            
            if( t < inAttackTime ) {
                // assert( inAttackTime > 0 );

                mComputedEnvelopes[i][s] = t / inAttackTime;
                }
            else if( t < inAttackTime + inDecayTime ) {
                // assert( inDecayTime > 0 );
                
                // decay down to sustain level
                mComputedEnvelopes[i][s] = 
                    ( 1.0 - inSustainLevel ) *
                    ( inAttackTime + inDecayTime - t ) / 
                    ( inDecayTime ) 
                    + inSustainLevel;
                }
            else if( 1.0 - t > inReleaseTime ) {
                mComputedEnvelopes[i][s] = inSustainLevel;
                }
            else {
                if( inReleaseTime > 0 ) {
                    
                    mComputedEnvelopes[i][s] = 
                        inSustainLevel -
                        inSustainLevel *
                        ( inReleaseTime - ( 1.0 - t ) ) / inReleaseTime;
                    }
                else {
                    // release time 0
                    // hold sustain until end
                    mComputedEnvelopes[i][s] = inSustainLevel;
                    }
                }
            
            }

        // test code to output evelopes for plotting in gnuplot
        if( false && i == 0 ) {
            FILE *file = fopen( "env0.txt", "w" );
            for( int s=0; s<length; s++ ) {
                fprintf( file, "%f %f\n", 
                         s / (double)( length - 1 ),
                         mComputedEnvelopes[i][s] );
                }
            fclose( file );
            }
        
        }
    
    }





Envelope::Envelope( double inAttackTime, double inHoldTime, 
                    double inReleaseTime,
                    int inMinNoteLengthInGridSteps,
                    int inMaxNoteLengthInGridSteps,
                    int inGridStepDurationInSamples )
        : mActiveNoteCount( 0 ),
          mGridStepDurationInSamples( inGridStepDurationInSamples ),
          mMinEnvNum( inMinNoteLengthInGridSteps ),
          mNumComputedEnvelopes( inMaxNoteLengthInGridSteps ),
          mEvelopeLengths( new int[ inMaxNoteLengthInGridSteps ] ),
          mComputedEnvelopes( new double*[ inMaxNoteLengthInGridSteps ] ) {
    
    for( int i=mMinEnvNum-1; i<mNumComputedEnvelopes; i++ ) {
        int length = (i+1) * inGridStepDurationInSamples;
        mEvelopeLengths[i] = length;
        
        mComputedEnvelopes[i] = new double[ length ];

        double *thisEnv = mComputedEnvelopes[i];
        
        // do boundary checking in sample space... faster
        int attackSamples = (int)( length * inAttackTime );
        int attackHoldSamples = (int)( length * (inAttackTime + inHoldTime) );
        int attackHoldReleaseSamples =
            (int)( length * (inAttackTime + inHoldTime + inReleaseTime ) );
        
        int releaseSamples =
            (int)( length * inReleaseTime );
        

        for( int s=0; s<attackSamples; s++ ) {
            thisEnv[s] = s / (double)attackSamples;
            }
        for( int s=attackSamples; s<attackHoldSamples; s++ ) {
            thisEnv[s] = 1.0;
            }
        for( int s=attackHoldSamples; s<attackHoldReleaseSamples; s++ ) {
            
            thisEnv[s] = 1.0 - 
                (s - attackHoldSamples) / (double)releaseSamples;
            }
        for( int s=attackHoldReleaseSamples; s<length; s++ ) {
            thisEnv[s] = 0;
            }
        
        /*
        for( int s=0; s<length; s++ ) {
            
            double t = s / (double)( length - 1 );
            
            //if( t < inAttackTime ) {
            if( s < attackSamples ) {
                // assert( inAttackTime > 0 );

                thisEnv[s] = t / inAttackTime;
                }
            //else if( t < inAttackTime + inHoldTime ) {
            else if( s < attackHoldSamples ) {
                // assert( inDecayTime > 0 );
                
                // hold at 1
                thisEnv[s] = 1.0;
                }
            else {
                if( inReleaseTime > 0 && 
                    //t < inAttackTime + inHoldTime + inReleaseTime ) {
                    s < attackHoldReleaseSamples ) {
                    
                    thisEnv[s] = 
                        1.0 - 
                        ( t - ( inAttackTime + inHoldTime ) ) 
                        / inReleaseTime;
                    }
                else {
                    // release time 0
                    // end immediately after hold
                    thisEnv[s] = 0;
                    }
                }
            
            }
        */
        // test code to output evelopes for plotting in gnuplot
        if( false && i == 0 ) {
            FILE *file = fopen( "env0.txt", "w" );
            for( int s=0; s<length; s++ ) {
                fprintf( file, "%f %f\n", 
                         s / (double)( length - 1 ),
                         mComputedEnvelopes[i][s] );
                }
            fclose( file );
            }
        
        }
    
    }


              
Envelope::~Envelope() {
    for( int i=mMinEnvNum-1; i<mNumComputedEnvelopes; i++ ) {
        delete [] mComputedEnvelopes[i];
        }
    delete [] mEvelopeLengths;
    delete [] mComputedEnvelopes;
    }



double *Envelope::getEnvelope( int inNoteLengthInGridSteps ) {
    if( inNoteLengthInGridSteps > mNumComputedEnvelopes ||
        inNoteLengthInGridSteps < mMinEnvNum ) {
    
        AppLog::error( "Error:  evelope for unsupported number of" 
                       " grid steps requested" );
        
        inNoteLengthInGridSteps = mMinEnvNum;
        }
    
    return mComputedEnvelopes[ inNoteLengthInGridSteps - 1 ];
    }


