#ifndef TRANSFORM_TOOL_SET_INCLUDED
#define TRANSFORM_TOOL_SET_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/ui/event/ActionListenerList.h"

#include "buttons.h"


enum transformTool { flipV, flipH, rotateCCW, rotateCW, clear, colorize };


class TransformToolSet : public GUIPanelGL, public ActionListener, 
                         public ActionListenerList {
        

    public:
        
        // sets its width/height automatically
        TransformToolSet( double inAnchorX, double inAnchorY,
                          char inHasColorize = false );


        transformTool getLastPressed();
        
        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );

    protected:
        
        transformTool mLastPressed;

        FlipHButtonGL *mFlipHButton;
        FlipVButtonGL *mFlipVButton;
        
        RotateCCWButtonGL *mCCWButton;
        RotateCWButtonGL *mCWButton;
        
        ClearButtonGL *mClearButton;

        ColorizeButtonGL *mColorizeButton;
    };


#endif
