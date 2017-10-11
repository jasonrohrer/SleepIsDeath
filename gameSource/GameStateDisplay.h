#ifndef GAME_STATE_DISPLAY_INCLUDED
#define GAME_STATE_DISPLAY_INCLUDED

#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/ui/event/ActionListenerList.h"


#include "GameState.h"
#include "common.h"



class GameStateDisplay : public GUIComponentGL, public ActionListenerList {

    public:
        
        // sets its own width and height
        GameStateDisplay( int inAnchorX, int inAnchorY,
                          char inShowObjectToolTips = false );
        
        virtual ~GameStateDisplay();
        

        // only for Controller game
        // used to send redraw events back to editor
        void setEditor( void *inEditor );
        

        // destroyed by this class
        void setState( GameState *inState );
        
        // updates sprite positions from data in inState
        // inState assumed to be indentical to last setState otherwise
        // note that inState is still destroyed by this class!
        void updateSpritePositions( GameState *inState );
        
        // turns grid/anchor drawing on and off
        void showGrid( char inShow );

        // shows dark, transparent hints for object positions only
        // doesn't draw rooms
        void setHintMode( char inHintsOnly );

        // this class auto-adjusts speech flips and offsets 
        // for display purposes
        // this function allows the new ones to be copied into another
        // game state
        // inDestinationState must have the same number of objects, etc
        void copySpeechCoordinates( GameState *inDestinationState );

        // if true, then above function should be called
        char mHasAnySpeechBeenAutoFlipped;
        
        // set to true if currently in the middle of manual bubble positioning
        // with mouse
        // this is auto-set to false whenever mouse released
        char mManualBubblePositioningLive;
        
        char getSpeechOffTop( int inObject );
        



        // override
        virtual void fireRedraw();
        virtual void mouseDragged( double inX, double inY );
		virtual void mousePressed( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
        virtual void mouseMoved( double inX, double inY );
        
        virtual void setFocus( char inFocus );
		virtual char isFocused();
        virtual void keyPressed( unsigned char inKey, 
                                 double inX, double inY );

        // after this display fires an action, find out
        // where the mouse event happened
        int mLastPixelClickX, mLastPixelClickY;
        int mLastGridClickX, mLastGridClickY;
        
        char mLastActionRelease;
        char mLastActionPress;
        char mLastActionKeyPress;
        
        unsigned char mLastKeyPressed;
        

        int mLastMouseoverObjectIndex;
        
        char mMouseHover;
        int mLastHoverGridX, mLastHoverGridY;

        char mEditingSpeech;
        char mEditingAction;
        

        char mHighlightEditedBubble;
        

    protected:
        
        char mShowObjectToolTips;
        

        void setLastMouse( double inX, double inY );
        void redoToolTip( int inGridX, int inGridY );
        

        void *mEditor;
        
        
        char mFocused;
        
        char mShowGrid;
        
        char mHintMode;
        

        GameState *mState;
        
        Sprite *mSprites[G][G];
        
        
        SimpleVector<char> mSpeechOffTop;
        
        
        SimpleVector<Sprite*> mObjectSprites;
        SimpleVector<intPair> mObjectSpritePositions;
        SimpleVector<int> mObjectSpriteOwners;
        SimpleVector<double> mObjectSpriteTrans;
        SimpleVector<char> mObjectSpriteGlows;

        // depth from 0 to G (where G is above everything else)
        // allows quick sorting into horizontal depth bands
        SimpleVector<int> mObjectSpriteDepth;
        // we draw locked ones behind others at same depth
        SimpleVector<char> mObjectSpriteLocked;
        

        // maps i=[0,numSprites) to indices in depth-draw order 
        SimpleVector<int> mDrawOrderMap;
        
        void recomputeDrawOrderMap();
        



        Sprite *mBubbleTopSprite;
        Sprite *mBubbleMiddleSprite;
        Sprite *mBubbleMiddleExtraSprite;
        Sprite *mBubbleMiddleExtraThinSprite;
        Sprite *mBubbleBottomSprite;
        Sprite *mBubbleBottomSpriteFlip;
        Sprite *mBubbleBottomNoTailSprite;
        Sprite *mBubbleMiddleTailSprite;
        Sprite *mBubbleMiddleTailFlipSprite;
        
        Sprite *mSpeechBoxTopSprite;
        Sprite *mSpeechBoxBottomSprite;
        

        Sprite *mActionBoxStartSprite;
        Sprite *mActionBoxMiddleSprite;
        Sprite *mActionBoxMiddleExtraSprite;
        Sprite *mActionBoxEndSprite;
        Sprite *mActionBoxEndFlipSprite;

        Sprite *mActionBoxStartTallSprite;
        Sprite *mActionBoxMiddleTallSprite;
        Sprite *mActionBoxMiddleExtraTallSprite;
        Sprite *mActionBoxEndTallSprite;
        Sprite *mActionBoxEndFlipTallSprite;
        
        
        // for blinking selected anchor
        int mLastSelected;
        int mBlinkCycle;
        

    private:
        
        void addSpritesFromStateInstance( int inIndex );
        
        // returns new sprite index
        int updatePositionFromStateInstance( int inIndex, int inSpriteIndex );
        
        

        void drawSpeech( int inIndex );
        
    };


#endif
