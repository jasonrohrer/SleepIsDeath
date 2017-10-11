#ifndef STATE_TOOL_SET_INCLUDED
#define STATE_TOOL_SET_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/ui/event/ActionListenerList.h"

#include "buttons.h"


enum stateTool { move, speak };


class StateToolSet : public GUIPanelGL, public ActionListener, 
                     public ActionListenerList {
        

    public:
        
        // sets its width/height automatically
        StateToolSet( double inAnchorX, double inAnchorY );


        stateTool getSelected();
        
        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );

    protected:
        SelectableButtonGL *mMoveButton;
        SelectableButtonGL *mSpeakButton;
        
    };


#endif
