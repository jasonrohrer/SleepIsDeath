#ifndef TOOL_TIP_COMPONENT_GL_INCLUDED
#define TOOL_TIP_COMPONENT_GL_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"

class ToolTipComponentGL : public GUIComponentGL {
    public:
        ToolTipComponentGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight );
        
        virtual ~ToolTipComponentGL();
        
        // defaults to no tip
        // automatically invokes TranslationManager on passed-in key
        virtual void setToolTip( const char *inTipTranslationKey );
        

        // override functions in GUIComponentGL
        virtual void mouseMoved( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		
    protected:
        char mHover;
        char *mTip;
    };


#endif
