#ifndef MOVE_TOOL_SET_INCLUDED
#define MOVE_TOOL_SET_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/ui/event/ActionListenerList.h"

#include "buttons.h"


enum moveTool { playerMove, playerSpeak, playerAct };


class MoveToolSet : public GUIPanelGL, public ActionListener, 
                     public ActionListenerList {
        

    public:
        
        // sets its width/height automatically
        MoveToolSet( double inAnchorX, double inAnchorY );


        moveTool getSelected();
        
        void setSelected( moveTool inTool );
        
        void setMoveAllowed( char inAllowed );
        
        
        // implements ActionListener
        virtual void actionPerformed( GUIComponent *inTarget );
        
        virtual void setEnabled( char inEnabled );


    protected:
        
        SelectableButtonGL *mActButton;
        SelectableButtonGL *mMoveButton;
        SelectableButtonGL *mSpeakButton;
        

    };


#endif
