#ifndef TIMER_DISPLAY_INCLUDED
#define TIMER_DISPLAY_INCLUDED


#include "ToolTipComponentGL.h"



class TimerDisplay : public ToolTipComponentGL {
        

    public:
        TimerDisplay( double inAnchorX, double inAnchorY );

        virtual ~TimerDisplay();


        void setTime( int inSecondsLeft );
        
        void freeze( char inFrozen );
        

        
        // override
		virtual void fireRedraw();
        
    protected:
        int mTime;
        
        int mBlinkCycle;
        
        char mNegativeTipSet;

        char mFrozen;
    };


#endif
