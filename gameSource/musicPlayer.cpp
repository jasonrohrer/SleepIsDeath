
#include "musicPlayer.h"

#include "common.h"
#include "Timbre.h"
#include "Envelope.h"
#include "Song.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/system/Time.h"



#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <math.h>
#include <stdlib.h>


// smoothly fade in particular tracks based on track fade level
// low value plays only first track... high value plays all tracks
extern double musicTrackFadeLevel;


// whether note is currently on and playing or not
// toggle these to control music as it plays

char noteToggles[PARTS][S][N][N];

int partLengths[PARTS];
// phrase position within each part 
int partPositions[PARTS];


double partLoudness[PARTS];
double partStereo[PARTS];


// last column of notes that sounded within each current tone matrix
int lastNoteColumnPlayed;





int sampleRate = 22050;
//int sampleRate = 11025;



// 16x16 tone matrix used in each phrase of each part
int w = N;
int h = N;

// total number of samples played so far
int streamSamples = 0;

// offset into grid at start
// for testing
int gridStartOffset = 0;



// overal loudness of music
double musicLoudness = 1.0;






// one grid step in seconds
double gridStepDuration = 0.5;
int gridStepDurationInSamples = (int)( gridStepDuration * sampleRate );


void setSpeed( int inSpeed ) {
    SDL_LockAudio();

    //double oldDuration = gridStepDuration;
    
    switch( inSpeed ) {
        case 0:
            gridStepDuration = 1;
            break;
        case 1:
            gridStepDuration = 0.5;
            break;
        case 2:
            gridStepDuration = 0.25;
            break;
        }

    //double speedMultiple = gridStepDuration / oldDuration;
    
    gridStepDurationInSamples = (int)( gridStepDuration * sampleRate );
    
    // jump in stream to maintain our current grid location
    // otherwise, we're in danger of playing the same note twice simultaneously

    SDL_UnlockAudio();
    }


//double entireGridDuraton;


// c
double v13KeyFrequency = 261.63;

// actually, can't support high notes in the key of c w/out round-off errors
// because this is a wavetable implementation, and when the tables get short,
// the errors get huge
// THIS, however, evenly divides our sample rate (22050)
// which means that we get perfect, whole-number wave tables at octaves
double v14KeyFrequency = 172.265625;

double keyFrequency = v13KeyFrequency;

// sets v13 frequency by default, UNTIL a timbre is changed (because only 
// a v14 game would do that).
// Thus, v13 music, when we're player, still sounds right.




int numTimbres = PARTS;

Timbre *musicTimbres[ PARTS ];

int numEnvelopes = PARTS;

Envelope *musicEnvelopes[ PARTS ];


// waiting for destruction
SimpleVector<Timbre *> oldTimbres;
SimpleVector<Envelope *> oldEnvelopes;




class Note {
    public:
        // index into musicTimbres array
        int mTimbreNumber;

        // pointer to actual Timbre used when note was last sounded
        // (so it doesn't change out from under note, in middle of note)
        Timbre *mTimbrePointer;
        

        // index into musicEnvelopes array
        int mEnvelopeNumber;
                
        // pointer to actual Envelope, for same reason as above
        Envelope *mEnvelopePointer;
        

        int mScaleNoteNumber;
        
        // additional loudness adjustment
        // places note in stereo space
        double mLoudnessLeft;
        double mLoudnessRight;
        

        // start time, in seconds from start of note grid
        //double mStartTime;

        // duration in seconds
        //double mDuration;

        // used when note is currently playing to track progress in note
        // negative if we should wait before starting to play the note
        int mCurrentSampleNumber;

        // duration in samples, set each time the note is playe
        // incase speed changes
        int mNumSamples;
        
        // set once
        int mNumGridSteps;
        

        // carry the grid step duration with us as we play, in case
        // it is changed before we're done playing
        int mOurGridStepDurationInSamples;
        

        Note *copy() {
            Note *note = new Note();
            
            note->mTimbreNumber = mTimbreNumber;
            note->mTimbrePointer = mTimbrePointer;
        
            note->mEnvelopeNumber = mEnvelopeNumber;
                
            note->mEnvelopePointer = mEnvelopePointer;
        
            note->mScaleNoteNumber = mScaleNoteNumber;
            note->mLoudnessLeft = mLoudnessLeft;
            note->mLoudnessRight = mLoudnessRight;
            note->mCurrentSampleNumber = mCurrentSampleNumber;
            
            note->mNumSamples = mNumSamples;
            note->mNumGridSteps = mNumGridSteps;
            note->mOurGridStepDurationInSamples = 
                mOurGridStepDurationInSamples;
            
            return note;
            }
        
    };


// all possible notes in a 16x16 phrase grid

// indexed as noteGrid[part][y][x]
Note *noteGrid[PARTS][N][N];





SimpleVector<Note*> currentlyPlayingNotes;



// need to synch these with audio thread

void setMusicLoudness( double inLoudness ) {
    SDL_LockAudio();
    
    musicLoudness = inLoudness;
    
    SDL_UnlockAudio();
    }



double getMusicLoudness() {
    return musicLoudness;
    }



void restartMusic() {
    SDL_LockAudio();

    // return to beginning (and forget samples we've played so far)
    streamSamples = 0;
    
    // drop all currently-playing notes
    currentlyPlayingNotes.deleteAll();
        
    SDL_UnlockAudio();
    }




// called by SDL to get more samples
void audioCallback( void *inUserData, Uint8 *inStream, int inLengthToFill ) {
    
    // 2 bytes for each channel of stereo sample
    int numSamples = inLengthToFill / 4;
    

    Sint16 *samplesL = new Sint16[ numSamples ];
    Sint16 *samplesR = new Sint16[ numSamples ];
    
    // first, zero-out the buffer to prepare it for our sum of note samples
    // each sample is 2 bytes
    memset( samplesL, 0, 2 * numSamples );
    memset( samplesR, 0, 2 * numSamples );
    

    int i;


    // hop through all grid steps that *start* in this stream buffer
    // add notes that start during this stream buffer

    // how far into stream buffer before we hit our first grid step? 
    int startOfFirstGridStep = streamSamples % gridStepDurationInSamples;
    
    if( startOfFirstGridStep != 0 ) {
        startOfFirstGridStep = 
            gridStepDurationInSamples - startOfFirstGridStep;
        }
    

    // hop from start of grid step to start of next grid step
    // ignore samples in between, since notes don't start there,
    // and all we're doing right now is finding notes that start
    for( i=startOfFirstGridStep; 
         i<numSamples; 
         i += gridStepDurationInSamples ) {
        
        // start of new grid position

        // check for new notes that are starting
        
        // map into our music image:
        int x = ( streamSamples + i ) / gridStepDurationInSamples;
          
        // for each part
        for( int si=0; si<PARTS; si++ ) {
            
            if( partLengths[si] > 0 ) {
                
                // step in part
                partPositions[si] = (x / w) % ( partLengths[si] );
                                
                
                // step in tone matrix for that part-step
                int matrixX = x % w;
                
                lastNoteColumnPlayed = matrixX;
                
                for( int y=0; y<h; y++ ) {
                    
                    Note *note = noteGrid[si][y][matrixX];
                    
                    if( note != NULL && 
                        noteToggles[si][ partPositions[si] ][y][matrixX] ) {
                        // new note
                        note = note->copy();
                        
                        currentlyPlayingNotes.push_back( note );

                        // save pointer to active envelope and timbre
                        // when this note began
                        note->mTimbrePointer = musicTimbres[
                            note->mTimbreNumber ];
                        note->mEnvelopePointer = musicEnvelopes[
                            note->mEnvelopeNumber ];
                        
                        note->mTimbrePointer->mActiveNoteCount ++;
                        note->mEnvelopePointer->mActiveNoteCount ++;

                        // tweak loudness based on part loudness and stereo
                        note->mLoudnessRight *= partLoudness[ si ];
                        note->mLoudnessLeft *= partLoudness[ si ];
                        
                        // constant power rule
                        double p = M_PI * partStereo[ si ] / 2;
                        
                        note->mLoudnessRight *= sin( p );
                        note->mLoudnessLeft *= cos( p );
                        


                        // start it


                        // base these on our
                        // envelope (instead of currently set duration in
                        // player, which may have been updated before
                        // the envelopes were properly updated due to thread
                        // interleaving issues)
                        int envGridStepDuration = 
                            musicEnvelopes[ note->mEnvelopeNumber ]->
                            mGridStepDurationInSamples;

                        // compute length
                        note->mNumSamples = 
                            note->mNumGridSteps * envGridStepDuration;

                        note->mOurGridStepDurationInSamples = 
                            envGridStepDuration;
                        

                        // set a delay for its start based on our position
                        // in this callback buffer
                        note->mCurrentSampleNumber = -i;
                        }            
                    }
                }
            else {
                partPositions[si] = 0;
                }
            
            }
        }
    
    
    
    streamSamples += numSamples;
    

    // loop over all current notes and add their samples to buffer
        
    for( int n=0; n<currentlyPlayingNotes.size(); n++ ) {
            
        Note *note = *( currentlyPlayingNotes.getElement( n ) );
             
        int waveTableNumber = note->mScaleNoteNumber;
        //Timbre *timbre = musicTimbres[ note->mTimbreNumber ];
        Timbre *timbre = note->mTimbrePointer;
        int tableLength = timbre->mWaveTableLengths[ waveTableNumber ];
            
        Sint16 *waveTable = timbre->mWaveTable[ waveTableNumber ];
        
        //Envelope *env = musicEnvelopes[ note->mEnvelopeNumber ];
        Envelope *env = note->mEnvelopePointer;
        double *envLevels = 
            env->getEnvelope( 
                // index envelope by number of grid steps in note
                note->mNumSamples / 
                note->mOurGridStepDurationInSamples );
        
        
        double noteLoudnessL = note->mLoudnessLeft;
        double noteLoudnessR = note->mLoudnessRight;
        
        // do this outside inner loop
        noteLoudnessL *= musicLoudness;
        noteLoudnessR *= musicLoudness;
        
        // factor in externally-set track fade level

        // level from 0..(numTimbres)
        double trackFadeInLevel = musicTrackFadeLevel * (numTimbres);

        // level for this track based on trackFadeInLevel
        double thisTrackLevel;
        
        if( trackFadeInLevel >= note->mTimbreNumber + 1 ) {
            // full volume
            thisTrackLevel = 1.0;
            }
        else if( trackFadeInLevel > note->mTimbreNumber ) {
            // linear fade in for this track
            thisTrackLevel = trackFadeInLevel - (int)trackFadeInLevel;
            }
        else {
            // track silent
            thisTrackLevel = 0;
            }
        
        noteLoudnessL *= thisTrackLevel;
        noteLoudnessR *= thisTrackLevel;
        


        int noteStartInBuffer = 0;
        int noteEndInBuffer = numSamples;
        
        if( note->mCurrentSampleNumber < 0 ) {
            // delay before note starts in this sample buffer
            noteStartInBuffer = - note->mCurrentSampleNumber;
            
            // we've taken account of the delay
            note->mCurrentSampleNumber = 0;
            }

        char endNote = false;
        
        int numSamplesLeftInNote = 
            note->mNumSamples - note->mCurrentSampleNumber;
        
        if( noteStartInBuffer + numSamplesLeftInNote < noteEndInBuffer ) {
            // note ends before end of buffer
            noteEndInBuffer = noteStartInBuffer + numSamplesLeftInNote;
            endNote = true;
            }
        

        int waveTablePos = note->mCurrentSampleNumber % tableLength;
        
        int currentSampleNumber = note->mCurrentSampleNumber;
        
        for( i=noteStartInBuffer; i != noteEndInBuffer; i++ ) {
            double envelope = envLevels[ currentSampleNumber ];
            
            double monoSample = envelope * 
                waveTable[ waveTablePos ];
            

            samplesL[i] += (Sint16)( noteLoudnessL * monoSample );
            samplesR[i] += (Sint16)( noteLoudnessR * monoSample );
            
            currentSampleNumber ++;
            
            waveTablePos ++;
            
            // avoid using mod operator (%) in inner loop
            // found with profiler
            if( waveTablePos == tableLength ) {
                // back to start of table
                waveTablePos = 0;
                }
            
            }
        
        note->mCurrentSampleNumber += ( noteEndInBuffer - noteStartInBuffer );
        
        if( endNote ) {
            // note ended in this buffer
            currentlyPlayingNotes.deleteElement( n );
            n--;

            // note not using these anymore
            note->mTimbrePointer->mActiveNoteCount --;
            note->mEnvelopePointer->mActiveNoteCount --;
            
            if( note->mTimbrePointer->mActiveNoteCount == 0 &&
                musicTimbres[ note->mTimbreNumber ] != note->mTimbrePointer ) {
                // this timbre is no longer used

                oldTimbres.deleteElementEqualTo( note->mTimbrePointer );
                delete note->mTimbrePointer;
                
                }

            if( note->mEnvelopePointer->mActiveNoteCount == 0 &&
                musicEnvelopes[ note->mEnvelopeNumber ] != 
                note->mEnvelopePointer ) {
                // this envelope is no longer used

                oldEnvelopes.deleteElementEqualTo( note->mEnvelopePointer );
                delete note->mEnvelopePointer;
                }

            // this was a copy
            delete note;
            }
        
        }


    // now copy samples into Uint8 buffer
    int streamPosition = 0;
    for( i=0; i != numSamples; i++ ) {
        Sint16 intSampleL = samplesL[i];
        Sint16 intSampleR = samplesR[i];
        
        inStream[ streamPosition ] = (Uint8)( intSampleL & 0xFF );
        inStream[ streamPosition + 1 ] = (Uint8)( ( intSampleL >> 8 ) & 0xFF );
        
        inStream[ streamPosition + 2 ] = (Uint8)( intSampleR & 0xFF );
        inStream[ streamPosition + 3 ] = (Uint8)( ( intSampleR >> 8 ) & 0xFF );
        
        streamPosition += 4;
        }

    delete [] samplesL;
    delete [] samplesR;
    
    }



// limit on n, based on Nyquist, when summing sine components
//int nLimit = (int)( sampleRate * M_PI );
// actually, this is way too many:  it takes forever to compute
// use a lower limit instead
// This produces fine results (almost perfect square wave)
int nLimit = 40;



// square wave with period of 2pi
double squareWave( double inT ) {
    double sum = 0;
    
    for( int n=1; n<nLimit; n+=2 ) {
        sum += 1.0/n * sin( n * inT );
        }
    return sum;
    }



// sawtoot wave with period of 2pi
double sawWave( double inT ) {
    double sum = 0;
    
    for( int n=1; n<nLimit; n++ ) {
        sum += 1.0/n * sin( n * inT );
        }
    return sum;
    }


// white noise, ignores inT
double whiteNoise( double inT ) {
    return 2.0 * ( rand() / (double)RAND_MAX ) - 1.0;
    }


// white noise where each sample is averaged with last sample
// effectively a low-pass filter
double lastNoiseSample = 0;

double smoothedWhiteNoise( double inT ) {
    // give double-weight to last sample to make it even smoother
    lastNoiseSample = ( 2 * lastNoiseSample + whiteNoise( inT ) ) / 3;
    
    return lastNoiseSample;
    }



// square where each sample is averaged with last sample
// effectively a low-pass filter
double lastSquareSample = 0;

double smoothedSquareWave( double inT ) {
    // give double-weight to last sample to make it even smoother
    lastSquareSample = ( 4 * lastSquareSample + squareWave( inT ) ) / 5;
    
    return lastSquareSample;
    }


double harmonicSine( double inT ) {
    return
        1.0 * sin( inT ) 
        +
        0.5 * sin( 2 * inT )
        +
        0.25 * sin( 4 * inT );
    }


double harmonicSaw( double inT ) {
    return
        1.0 * sawWave( inT ) 
        +
        0.5 * sawWave( 2 * inT )
        +
        0.25 * sawWave( 4 * inT );
    }


static double coefficients[256];
static int numCoefficientsToUse;

double coefficientMix( double inT ) {
    double sum = 0;
    
    for( int n=1; n<=numCoefficientsToUse; n++ ) {
        sum += coefficients[n-1] * sin( n * inT );
        }
    return sum;
    }



void setTimbre( int inTimbreNumber,
                double *inPartialCoefficients, int numCoefficients,
                int inOctavesDown ) {
    
    // this must be a v14 game
    keyFrequency = v14KeyFrequency;
    

    // set up coefficients used by mix function above
    numCoefficientsToUse = numCoefficients;
    
    for( int i=0; i<numCoefficients; i++ ) {
        coefficients[i] = inPartialCoefficients[i];
        }
    


    int heightPerTimbre = h;

    // possible for all notes in a column to be on at user's request
    // and notes are 3 long at max (decays), so consider overlap
    double  maxNoteLoudnessInAColumn = h * 3;

    double loudnessPerTimbre = 1.0 / maxNoteLoudnessInAColumn;


    //double t = Time::getCurrentTime();
    

    double freq = keyFrequency / pow( 2, inOctavesDown );

    Timbre *newTimbre = new Timbre( sampleRate, 1.0 * loudnessPerTimbre,
                                    freq,
                                    heightPerTimbre, coefficientMix, 1 );
    
    // now replace it
    SDL_LockAudio();

    if( musicTimbres[inTimbreNumber]->mActiveNoteCount > 0 ) {    
        // save old one, because some currently-playing notes are using it!
        oldTimbres.push_back( musicTimbres[inTimbreNumber] );
        }
    else {
        delete musicTimbres[inTimbreNumber];
        }
    
    
    
    musicTimbres[inTimbreNumber] = newTimbre;
    
    SDL_UnlockAudio();
    }





void setScale( char inToneOn[12] ) {
    setTimbreScale( inToneOn );
    }




void setEnvelope( int inTimbreNumber,
                  double inAttack, double inHold,
                  double inRelease ) {

    if( inAttack + inHold + inRelease > 1.0 ) {
        AppLog::error( 
            "Attack + Hold + Release in specified envelope too long" );
        if( inAttack > 1 ) {
            inAttack = 1;
            }
        inHold = 1 - inAttack;
        inRelease = 0;
        }
    
    int maxNoteLength = 3;

    Envelope *newEnvelope =  new Envelope( inAttack, inHold, 
                                           inRelease,
                                           maxNoteLength,
                                           maxNoteLength,
                                           gridStepDurationInSamples );
    // replace it
    SDL_LockAudio();
    
    if( musicEnvelopes[inTimbreNumber]->mActiveNoteCount > 0 ) {    
        // save old one, because some currently-playing notes are using it!
        oldEnvelopes.push_back( musicEnvelopes[inTimbreNumber] );
        }
    else {
        delete musicEnvelopes[inTimbreNumber];
        }
    
    
    
    musicEnvelopes[inTimbreNumber] = newEnvelope;
    
    SDL_UnlockAudio();
    }



void setDefaultMusicSounds() {
    // back to v13 default
    keyFrequency = v13KeyFrequency;
    

    gridStepDuration = 0.5;
    gridStepDurationInSamples = (int)( gridStepDuration * sampleRate );


    setDefaultScale();
    
    
    SDL_LockAudio();

    for( int i=0; i<PARTS; i++ ) {
        
        if( musicTimbres[i] != NULL ) {
            if( musicTimbres[i]->mActiveNoteCount > 0 ) {    
            
                // save old one, because some currently-playing notes 
                // are using it!
                oldTimbres.push_back( musicTimbres[i] );
                }
            else {
                delete musicTimbres[i];
                }
            }
        if( musicEnvelopes[i] != NULL ) {
            if( musicEnvelopes[i]->mActiveNoteCount > 0 ) {    
            
                // save old one, because some currently-playing notes 
                // are using it!
                oldEnvelopes.push_back( musicEnvelopes[i] );
                }
            else {
                delete musicEnvelopes[i];
                }
            }
        }

    
    int heightPerTimbre = h;


    // possible for all notes in a column to be on at user's request
    // and notes are 3 long at max (decays), so consider overlap
    double  maxNoteLoudnessInAColumn = h * 3;
    

   
    // divide loudness amoung timbres to avoid clipping
    double loudnessPerTimbre = 1.0 / maxNoteLoudnessInAColumn;
    
    // further adjust loudness per channel here as we construct
    // each timbre.

    //double t = Time::getCurrentTime();
    
    
    // load defaults into first 3 banks.
    // thus, music sent from v13 and earlier (if we'r Player) will
    // play correctly
    musicTimbres[0] = new Timbre( sampleRate, 1.0 * loudnessPerTimbre,
                                  keyFrequency / 4,
                                  heightPerTimbre, sawWave );   
 
    musicTimbres[1] = new Timbre( sampleRate, 0.75 * loudnessPerTimbre,
                                  keyFrequency / 2,
                                  heightPerTimbre, harmonicSaw );

    // last timbre has one extra note at top (top row of grid)
    musicTimbres[2] = new Timbre( sampleRate, 0.65 * loudnessPerTimbre,
                                  keyFrequency,
                                  heightPerTimbre + 1, harmonicSine );
    

    for( int i=3; i<PARTS; i++ ) {
        musicTimbres[i] = new Timbre( sampleRate, 0.65 * loudnessPerTimbre,
                                      keyFrequency/2,
                                      heightPerTimbre, harmonicSine );
        }
    
    //exit( 0 );
    
    

    
    // next, compute the longest note in the song
    // fixed at 2
    int maxNoteLength = 3;
        
    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Max note length in song = %d\n", maxNoteLength );


    // load defaults into first 3 banks.
    // thus, music sent from v13 and earlier (if we'r Player) will
    // play correctly

    musicEnvelopes[0] = new Envelope( 0.02, 0.98, 0, 0,
                                      maxNoteLength,
                                      maxNoteLength,
                                      gridStepDurationInSamples );
    musicEnvelopes[1] = new Envelope( 0.15, 0.85, 0.0, 0.0,
                                      maxNoteLength,
                                      maxNoteLength,
                                      gridStepDurationInSamples );

    musicEnvelopes[2] = new Envelope( 0.01, 0.99, 0.0, 0.0,
                                      maxNoteLength,
                                      maxNoteLength,
                                      gridStepDurationInSamples );
    
      
    for( int i=3; i<PARTS; i++ ) {
        musicEnvelopes[i] = new Envelope( //0.5, 0.5, 0.0, 0.0,
            0.01, 0.99, 0.0, 0.0,
            maxNoteLength,
            maxNoteLength,
            gridStepDurationInSamples );
        }


    for( int i=0; i<PARTS; i++ ) {
        partLoudness[i] = 1;
        partStereo[i] = 0.5;
        }
    

    
    SDL_UnlockAudio();

    }






void initMusicGrid() {


    /*
    // test for profiler
    for( int i=0; i<20; i++ ) {
        
        Envelope *newEnvelope =  new Envelope( 0.25, 0.5, 
                                               0.25,
                                               10,
                                               gridStepDurationInSamples );
        delete newEnvelope;
        }
    exit( 0 );
    */    



    
    // setDefaultScale();
    

    //entireGridDuraton = gridStepDuration * w;
    

    // jump ahead in stream, if needed
    streamSamples += gridStartOffset * gridStepDurationInSamples;
    
    
    int heightPerTimbre = h;

    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Height in grid per timbre = %d\n", heightPerTimbre );
    

    // nullify so they are not added to old list by setDefaultMusicSounds
    for( int i=0; i<PARTS; i++ ) {
        musicTimbres[i] = NULL;
        musicEnvelopes[i] = NULL;
        }

    setDefaultMusicSounds();
    

    

    // fix L/R loudness for all notes, based on their timbres
    
    double leftLoudness[PARTS];//  = { 0.75, 1.0, 1.0 };
    double rightLoudness[PARTS];// = { 1.0, 0.75, 1.0 };
    
            

    
    for( int si=0; si<PARTS; si++ ) {
        
        //leftLoudness[si] = 0.75 + (si+1) / (double)PARTS;
        //rightLoudness[si] = 1.0 - 0.75 * (si+1) / (double)PARTS;
        // live per-part panning now
        leftLoudness[si] = 1;
        rightLoudness[si] = 1;
        

        for( int y=0; y<h; y++ ) {
            
            for( int x=0; x<w; x++ ) {
                
                // default to NULL
                noteGrid[si][y][x] = NULL;
            
                /*
                // the note number in our scale
                // scale starts over for each timbre 
                int noteNumber = y % heightPerTimbre;
                
                if( y == h-1 ) {
                // highest note, hangs off top
                noteNumber = heightPerTimbre;
                }
                */
                int noteNumber = y;
            

                // start a new note
                noteGrid[si][y][x] = new Note();
                
                noteGrid[si][y][x]->mScaleNoteNumber = noteNumber;
                
                noteGrid[si][y][x]->mTimbreNumber = si;

                noteGrid[si][y][x]->mTimbrePointer = musicTimbres[ si ];
            

                // same as timbre number
                noteGrid[si][y][x]->mEnvelopeNumber = 
                    noteGrid[si][y][x]->mTimbreNumber;
                
                noteGrid[si][y][x]->mEnvelopePointer = musicEnvelopes[ si ];


                // left loudness fixed
                noteGrid[si][y][x]->mLoudnessLeft = 
                    leftLoudness[ noteGrid[si][y][x]->mTimbreNumber ];
                    
                // right loudness fixed
                noteGrid[si][y][x]->mLoudnessRight = 
                    rightLoudness[ noteGrid[si][y][x]->mTimbreNumber ];
                

                //noteGrid[si][y][x]->mStartTime = gridStepDuration * x;
                
                // three grid steps long, for overlap
                //noteGrid[si][y][x]->mDuration = 
                //    gridStepDuration * 3;
                noteGrid[si][y][x]->mNumGridSteps = 3;
                
                // set this when the note is played
                //noteGrid[si][y][x]->mNumSamples = 
                //    gridStepDurationInSamples * 3;
                }
            }
        }
    
    
    for( int i=0; i<PARTS; i++ ) {
        for( int s=0; s<S; s++ ) {
        
            for( int y=0; y<h; y++ ) {
                
                for( int x=0; x<w; x++ ) {
                    
                    // all notes start off off
                    noteToggles[i][s][y][x] = false;
                    }
                }
            }
        partLengths[i] = 0;
        partPositions[i] = 0;
        }
    lastNoteColumnPlayed = 0;
    
        
    // test
    for( int i=0; i<w; i++ ) {
        //noteToggles[i][i] = true;
        }
    
        
    }



void startMusic() {
    
    initMusicGrid();
    
    
    SDL_AudioSpec audioFormat;

    // Set 16-bit stereo audio at 22Khz 
    audioFormat.freq = sampleRate;
    audioFormat.format = AUDIO_S16;
    audioFormat.channels = 2;
    audioFormat.samples = 512;        // A good value for games
    //audioFormat.samples = 1024;        // A good value for games
    audioFormat.callback = audioCallback;
    audioFormat.userdata = NULL;

    // Open the audio device and start playing sound!
    if( SDL_OpenAudio( &audioFormat, NULL ) < 0 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Unable to open audio: %s\n", SDL_GetError() );
        }

    // set pause to 0 to start audio
    SDL_PauseAudio(0);
    
    
    }



void stopMusic() {
    SDL_CloseAudio();


    for( int i=0; i<PARTS; i++ ) {
        for( int y=0; y<h; y++ ) {
            
            for( int x=0; x<w; x++ ) {
                
                if( noteGrid[i][y][x] != NULL ) {
                    delete noteGrid[i][y][x];
                    }
                }
            }
        }
    
    int i;
    
    for( i=0; i<numTimbres; i++ ) {
        delete musicTimbres[i];
        }
    for( i=0; i<numEnvelopes; i++ ) {
        delete musicEnvelopes[i];
        }


    for( i=0; i<oldTimbres.size(); i++ ) {
        delete *( oldTimbres.getElement( i ) );
        }
    for( i=0; i<oldEnvelopes.size(); i++ ) {
        delete *( oldEnvelopes.getElement( i ) );
        }

    // delete these copies
    for( i=0; i<currentlyPlayingNotes.size(); i++ ) {
        delete *( currentlyPlayingNotes.getElement( i ) );
        }
    
    
    }

    
