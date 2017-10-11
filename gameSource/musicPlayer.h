

// dimensions of note grid
#define N  16


// instrument parts
// one extra part (SI) for when the phrase editor is open
#define PARTS SI+1


// starts playing music from the grid
void startMusic();



// set loudness in range [0.0,1.0]
void setMusicLoudness( double inLoudness );


double getMusicLoudness();


// returns all timbres/evelopes to those set by default
void setDefaultMusicSounds();


// inNumCoefficients must not exceed 256
void setTimbre( int inTimbreNumber, 
                double *inPartialCoefficients, int numCoefficients,
                int inOctavesDown );




// release + attack + hold must be <= 1.0
// sustains at full volume between attack and release slopes (no decay)
void setEnvelope( int inTimbreNumber,
                  double inAttack, double inHold,
                  double inRelease );



// sets scale 
// NOTE:  all timbres must be regenerated for this to have an effect
void setScale( char inToneOn[12] );


// 0, 1, 2, slow, med, fast    
// NOTE:  all envelopes must be regenerated for this to 
// have an effect
void setSpeed( int inSpeed );



// causes music to jump back to beginning
void restartMusic();



void stopMusic();

