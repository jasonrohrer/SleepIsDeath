#ifndef WARNING_DISPLAY_INCLUDED
#define WARNING_DISPLAY_INCLUDED


#include "ToolTipComponentGL.h"
#include "Sprite.h"



class WarningDisplay : public ToolTipComponentGL {
        

    public:
        WarningDisplay( double inAnchorX, double inAnchorY );

        virtual ~WarningDisplay();

        
        // shows the flashing warning for a certain number of frames
        void show( const char *inToolTipTransKey, int inFrames );
        
        // override
		virtual void fireRedraw();
        
    protected:
        int mBlinkCycle;
        int mFramesLeft;
        
        Sprite mSprite;
    };


#endif
