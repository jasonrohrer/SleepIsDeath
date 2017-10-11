#ifndef STATE_OBJECT_DISPLAY_INCLUDED
#define STATE_OBJECT_DISPLAY_INCLUDED

#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/ui/event/ActionListenerList.h"


#include "StateObject.h"
#include "common.h"



// used to track trans of each displayed sprite
typedef struct clickMask{
        char opaque[P][P];
    } clickMask;



class StateObjectDisplay : public GUIComponentGL, public ActionListenerList {

    public:
        
        // sets its own width and height
        StateObjectDisplay( int inAnchorX, int inAnchorY );
        
        virtual ~StateObjectDisplay();
        
        
        void setStateObject( StateObject inStateObject, char inUseTrans );
        
        // updates sprite positions from data in inState
        // inState assumed to be indentical to last setState otherwise
        void updateSpritePositions( StateObject inStateObject );
        
        // turns grid/anchor drawing on and off
        void showGrid( char inShow );
        

        double getZoom();
        void setZoom( double inZoom );
        

        // passes out offset of click insided clicked layer
        // offset is from center of sprite layer
        int getClickedLayer( int inPixelClickX, int inPixelClickY,
                             int *outClickOffsetX, int *outClickOffsetY );
        
        

        // override
        virtual void fireRedraw();
        virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
        virtual void mouseMoved( double inX, double inY );
        

        // after this display fires an action, find out
        // where the mouse event happened
        int mLastPixelClickX, mLastPixelClickY;
        int mLastGridClickX, mLastGridClickY;

        char mLastActionRelease;
        char mLastActionPress;

        char mMouseHover;
        int mLastHoverX, mLastHoverY;
        
        
        // used to disable selected highlight during drags
        char mShowSelected;

    protected:
        
        void setLastMouse( double inX, double inY );
        
        double mZoomFactor;
        
        
        char mFocused;
        
        char mShowGrid;
        
        

        char mUseTrans;
        
        int mLastSelected;
        int mBlinkCycle;
        

        StateObject mStateObject;        

        
        SimpleVector<Sprite*> mObjectSprites;
        SimpleVector<Sprite*> mObjectSpriteHighlights;
        SimpleVector<intPair> mObjectSpritePositions;
        SimpleVector<float> mObjectSpriteFades;
        SimpleVector<char> mObjectSpriteGlows;
        
        SimpleVector<clickMask> mObjectMasks;
        
    };


#endif
