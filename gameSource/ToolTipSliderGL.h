#ifndef TOOL_TIP_SLIDER_INCLUDED
#define TOOL_TIP_SLIDER_INCLUDED


#include "minorGems/graphics/openGL/gui/SliderGL.h"


class ToolTipSliderGL : public SliderGL {
    public:

        ToolTipSliderGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, Image *inIconImage,
			double inIconWidthFraction,
			Color *inBarStartColor, Color *inBarEndColor,
			Color *inThumbColor, Color *inBorderColor,
            double inBorderWidth,
            double inThumbWidth,
            double inMinThumbIncrement );
        
        virtual ~ToolTipSliderGL();
        
        // defaults to no tip
        // automatically invokes TranslationManager on passed-in key
        // if inUseTranslationManager is false, uses Key as tool tip directly
        virtual void setToolTip( const char *inTipTranslationKey,
                                 char inUseTranslationManager = true );
        

        // override functions in SliderGL
        virtual void mouseMoved( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
    
    protected:
        char mHover;
        char *mTip;
    };


#endif
