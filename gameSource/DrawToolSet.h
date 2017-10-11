#ifndef DRAW_TOOL_SET_INCLUDED
#define DRAW_TOOL_SET_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/ui/event/ActionListenerList.h"

#include "buttons.h"


enum drawTool { pen, horLine, verLine, fill, pickColor, stamp };


class DrawToolSet : public GUIPanelGL, public ActionListener, 
                    public ActionListenerList {
        

    public:
        
        // sets its width/height automatically
        // also can specify translation key for pick button tool tip
        DrawToolSet( double inAnchorX, double inAnchorY,
                     const char *inPickButtonTipKey = "tip_pickColor",
                     char inShowStamp = true );


        drawTool getSelected();
        
        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );

        // always focused, listens for shift to switch to picker tool
        virtual char isFocused();
		virtual void specialKeyPressed( int inKey, double inX, double inY );
		virtual void specialKeyReleased( int inKey, double inX, double inY );

    protected:
        
        SelectableButtonGL *mPenButton;
        SelectableButtonGL *mHorLineButton;
        SelectableButtonGL *mVerLineButton;
        SelectableButtonGL *mFillButton;
        SelectableButtonGL *mPickColorButton;        
        SelectableButtonGL *mStampButton;        


        // for when shift is released
        SelectableButtonGL *mLastSelectedBeforeShift;
        
    };


#endif
