#ifndef TOOL_TIP_DISPLAY_INCLUDED
#define TOOL_TIP_DISPLAY_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"



class ToolTipDisplay : public GUIComponentGL {
        

    public:
        ToolTipDisplay( double inAnchorX, double inAnchorY );

        virtual ~ToolTipDisplay();

        // override
		virtual void fireRedraw();
        
    protected:

        // result NOT to be detroyed by caller
        // subclasses can override this to control where tip comes from
        virtual char *getTip();
        

        char *mLastTip;
        
        double mFadeFactor;
        
    };


#endif
