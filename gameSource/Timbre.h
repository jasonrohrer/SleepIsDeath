
#include <SDL/SDL.h>


// one of these must be called before constructing first timbre
void setDefaultScale();

void setTimbreScale( char inToneOn[12] );



class Timbre {
    public:
        
        /**
         * Constructs a timbre and fills its wavetables.
         *
         * @param inSampleRate number of samples per second.
         * @param inLoudness a scale factor in [0,1].
         * @param inBaseFrequency the lowest note in the wave table, in Hz.
         *   This is also the key for the major scale held in the wave table.
         * @param inNumWaveTableEntries the number of wavetable entries.
         * @param inPeriodsPerTable number of periods stored in each table.
         *   For functions that don't have a period of 2PI (like noise), this
         *   will make a more accurrate signal representation when the 
         *   wave table is looped.  Defaults to 1 (fine for 2PI functions).
         * @param inWaveFunction a function mapping a double parameter t
         *   to a wave height in [-1,1].  Must have a period of 2pi.
         */
        Timbre( int inSampleRate,
                double inLoudness,
                double inBaseFrequency,
                int inNumWaveTableEntries, 
                double( *inWaveFunction )( double ),
                int inPeriodsPerTable = 1 );
        
        ~Timbre();
        

        
        int mNumWaveTableEntries;
        // mWaveTable[x] corresponds to a wave with frequency of 
        // getFrequency(x)
        Sint16 **mWaveTable;
        int *mWaveTableLengths;


        // how many playing notes are still using this timbre?
        int mActiveNoteCount;
        
    };
