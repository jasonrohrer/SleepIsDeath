

class ToolTipManager {
        

    public:
        
        // can be NULL, copied internally, so destroyed by caller
        static void setTip( const char *inTip );
        
        
        // destroyed by this class
        static char *getTip();
        
        
        // setTip calls have no effect until unfrozen
        static void freeze( char inFrozen );
        


        static void staticInit();
        static void staticFree();
        

    private:
        static char *mTip;
        
        static char mFrozen;
    };
