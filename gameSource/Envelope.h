

class Envelope {
    public:
        

        /**
         * Constructs an ADSR envelope.
         *
         * All parameters are in range 0..1, and sum of the three time
         * parameters must be <= 1.
         *
         * @param inMinNoteLengthInGridSteps the minimum note length to
         *   cover.  Exact envelopes will be generated for notes starting at
         *   this length.
         * @param inMaxNoteLengthInGridSteps the maximum note length to
         *   cover.  Exact envelopes will be generated for notes up
         *   to this length.
         * @param inGridStepDurationInSamples the number of samples
         *   per grid step. 
         */ 
        Envelope( double inAttackTime, double inDecayTime, 
                  double inSustainLevel, double inReleaseTime,
                  int inMinNoteLengthInGridSteps,
                  int inMaxNoteLengthInGridSteps,
                  int inGridStepDurationInSamples );


        /**
         * Constructs an AHR envelope (sustain at 1 during hold)
         *
         * All parameters are in range 0..1, and sum of the three time
         * parameters must be <= 1.
         *
         * @param inMinNoteLengthInGridSteps the minimum note length to
         *   cover.  Exact envelopes will be generated for notes starting at
         *   this length.
         * @param inMaxNoteLengthInGridSteps the maximum note length to
         *   cover.  Exact envelopes will be generated for notes up
         *   to this length.
         * @param inGridStepDurationInSamples the number of samples
         *   per grid step. 
         */ 
        Envelope( double inAttackTime, double inHoldTime, 
                  double inReleaseTime,
                  int inMinNoteLengthInGridSteps,
                  int inMaxNoteLengthInGridSteps,
                  int inGridStepDurationInSamples );

        
        ~Envelope();

        

        /**
         * Gets an evenlope for a given note length.
         *
         * @return an evelope of values in [0,1] that can be indexed by
         *   sample number.  Will be destroyed when this class is destroyed.
         */
        double *getEnvelope( int inNoteLengthInGridSteps );
        


        // how many playing notes are still using this envelope?
        int mActiveNoteCount;


        int mGridStepDurationInSamples;
        

    private:
        int mMinEnvNum;
        
        int mNumComputedEnvelopes;
        
        int *mEvelopeLengths;
        
        double **mComputedEnvelopes;
    };
