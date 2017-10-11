#ifndef BUTTONS_INCLUDED
#define BUTTONS_INCLUDED


#include "minorGems/graphics/openGL/gui/ButtonGL.h"

#include "color.h"




class ToolTipButtonGL : public ButtonGL {
    public:
        ToolTipButtonGL( double inAnchorX, double inAnchorY, 
                         double inWidth, double inHeight );
        
        virtual ~ToolTipButtonGL();
        
        // defaults to no tip
        // automatically invokes TranslationManager on passed-in key
        // if inUseTranslationManager is false, uses Key as tool tip directly
        virtual void setToolTip( const char *inTipTranslationKey,
                                 char inUseTranslationManager = true );
        

        // override functions in ButtonGL
        virtual void mouseMoved( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
    protected:
        char *mTip;
    };





class BorderButtonGL : public ToolTipButtonGL {
    public:
        void setFillColor( rgbaColor inColor );
        
        void setBorderColor( rgbaColor inColor );
    
    protected:
        BorderButtonGL( double inAnchorX, double inAnchorY, 
                        double inWidth, double inHeight );
        
        
        virtual void drawBorder();
    
        rgbaColor mBorderColor;
        rgbaColor mFillColor;
        
    };



// base class for buttons with icons
class IconButtonGL : public BorderButtonGL {
        
    public:
        IconButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );
        
        virtual void drawPressed();

        virtual void drawUnpressed();
    
    protected:
        // subclasses must implement this
        virtual void drawIcon() = 0;
    };
        



class AddButtonGL : public IconButtonGL {
        
    public:
        AddButtonGL( double inAnchorX, double inAnchorY, 
                     double inWidth, double inHeight );
        
    protected:
        
        void drawIcon();
    };









#include "Sprite.h"


// buttons that display sprites inside a border
// sprite can be NULL
// invisible when disabled
class SpriteButtonGL : public BorderButtonGL {
        
    public:
        // sprite is destroyed by this class
        SpriteButtonGL( Sprite *inSprite,
                        double inSpriteDrawScale,
                        double inAnchorX, double inAnchorY, 
                        double inWidth, double inHeight );
        
        virtual ~SpriteButtonGL();
        
        // defaults to white, destroyed internally
        // set to NULL to restore to white
        void setColor( Color *inColor );
        

        // replaces sprite
        virtual void setSprite( Sprite *inSprite );
        
        virtual void drawPressed();

        virtual void drawUnpressed();
    
    protected:
        Sprite *mSprite;
        double mSpriteDrawScale;
        
        Color *mColor;
        
    };




class LeftButtonGL : public SpriteButtonGL {
        
    public:
        LeftButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );
    };



class RightButtonGL : public SpriteButtonGL {
        
    public:
        RightButtonGL( double inAnchorX, double inAnchorY, 
                       double inWidth, double inHeight );
    };



class EditButtonGL : public SpriteButtonGL {
        
    public:
        EditButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );
    };




class SmallAddButtonGL : public SpriteButtonGL {
        
    public:
        SmallAddButtonGL( double inAnchorX, double inAnchorY, 
                          double inWidth, double inHeight );
    };


// delete button with no confirmation behavior
class QuickDeleteButtonGL : public SpriteButtonGL {
        
    public:
        QuickDeleteButtonGL( double inAnchorX, double inAnchorY, 
                             double inWidth, double inHeight );
    };



// a button with a keyboard equivalent (when CTRL held)
class KeyEquivButtonGL : public SpriteButtonGL {

    public:
        
        KeyEquivButtonGL( Sprite *inSprite,
                          double inSpriteDrawScale,
                          double inAnchorX, double inAnchorY, 
                          double inWidth, double inHeight,
                          char inKeyA, char inKeyB );
        
        // always focused, listens for ctrl-z
        virtual char isFocused();
        virtual void keyPressed( unsigned char inKey, double inX, double inY );
    protected:
        char mKeyA, mKeyB;
    };




class CloseButtonGL : public KeyEquivButtonGL {
        
    public:
        CloseButtonGL( double inAnchorX, double inAnchorY, 
                       double inWidth, double inHeight );

        // override this to make click area slightly bigger
        // (so it's infinite if this button in corner of screen)
        virtual char isInside( double inX, double inY );
        
    };



class NoKeyCloseButtonGL : public SpriteButtonGL {
        
    public:
        NoKeyCloseButtonGL( double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight );

        // override this to make click area slightly bigger
        // (so it's infinite if this button in corner of screen)
        virtual char isInside( double inX, double inY );
        
    };




class UndoButtonGL : public KeyEquivButtonGL {
        
    public:
        UndoButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );

    };



class RedoButtonGL : public SpriteButtonGL {
        
    public:
        RedoButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );
    };



class FlipHButtonGL : public SpriteButtonGL {
        
    public:
        FlipHButtonGL( double inAnchorX, double inAnchorY, 
                       double inWidth, double inHeight );
    };


class FlipVButtonGL : public SpriteButtonGL {
        
    public:
        FlipVButtonGL( double inAnchorX, double inAnchorY, 
                       double inWidth, double inHeight );
    };


class RotateCCWButtonGL : public SpriteButtonGL {
        
    public:
        RotateCCWButtonGL( double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );
    };


class RotateCWButtonGL : public SpriteButtonGL {
        
    public:
        RotateCWButtonGL( double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );
    };



class ClearButtonGL : public SpriteButtonGL {
        
    public:
        ClearButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );
    };


class ColorizeButtonGL : public SpriteButtonGL {
        
    public:
        ColorizeButtonGL( double inAnchorX, double inAnchorY, 
                          double inWidth, double inHeight );
    };




// buttons that shows a color
// responds to dragging across it and pressing (as well as releasing) 
//   by firing events
class ColorButtonGL : public BorderButtonGL {
        
    public:
        ColorButtonGL( rgbaColor inColor,
                       double inAnchorX, double inAnchorY, 
                       double inWidth, double inHeight );
        
        // replaces color
        void setColor( rgbaColor inColor, char inForceDarkBorder = false );
        
        // was last action triggered by hover w/out press or release
        char wasLastActionHover();
        
        

        virtual void drawPressed();

        virtual void drawUnpressed();
    
        
        // overrides these
        virtual void mousePressed( double inX, double inY );
        virtual void mouseDragged( double inX, double inY );
        virtual void mouseMoved( double inX, double inY );
        virtual void mouseReleased( double inX, double inY );


    protected:
        rgbaColor mColor;
        
        char mLastActionHover;
    };




// button that has a highlight that can be toggled
class HighlightColorButtonGL : public ColorButtonGL {
    public:
        HighlightColorButtonGL( rgbaColor inColor,
                                double inAnchorX, double inAnchorY, 
                                double inWidth, double inHeight );

        virtual void setHighlight( char inHighlightOn );

        virtual void setSelection( char inSelectionOn );

        virtual void setOverlay( char inOverlayOn );

        virtual void setOverlayColor( rgbaColor inColor );
        
        // does overlay represent a transparent area
        virtual void setOverlayTrans( char inTrans );

        // override
        virtual void drawPressed();

        virtual void drawUnpressed();

    protected:
        char mHighlightOn;

        char mSelectionOn;

        char mOverlayOn;
        char mOverlayTrans;
        
        rgbaColor mOverlayColor;


        void drawCheckerboard( unsigned char inAlpha );
        

    };




// buttons that display sprites without a border
// sprite can be NULL
// invisible when disabled
// responds to dragging across it and pressing (as well as releasing) 
//   by firing events
class SpriteCellButtonGL : public ToolTipButtonGL {
        
    public:
        // sprite is destroyed by this class
        SpriteCellButtonGL( Sprite *inSprite,
                        double inSpriteDrawScale,
                        double inAnchorX, double inAnchorY, 
                        double inWidth, double inHeight );
        
        ~SpriteCellButtonGL();
        
        virtual void setHighlight( char inHighlightOn );
        

        // replaces sprite
        // destroyed by this class
        virtual void setSprite( Sprite *inSprite );

        virtual Sprite *getSprite();
        

        // sets the sprite to draw when main sprite is NULL
        // defaults to NULL (draw nothing)
        // NOT destroyed by this class
        virtual void setBlankSprite( Sprite *inSprite );
        
        
        // defaults to white, destroyed internally
        // set to NULL to restore to white
        void setColor( Color *inColor );


        virtual void drawPressed();

        virtual void drawUnpressed();

        // overrides these
        virtual void mousePressed( double inX, double inY );
        virtual void mouseDragged( double inX, double inY );
        virtual void mouseReleased( double inX, double inY );
        
    protected:
        Sprite *mSprite;
        Sprite *mBlankSprite;
        double mSpriteDrawScale;
        
        char mHighlightOn;

        Color *mColor;

        // on by default... subclasses can turn off
        char mFireOnDrag;
        char mFireOnRelease;
    };



// a SpriteCellButtonGL that also has a time position overlay,
// only displayed when sprite not NULL
class MusicCellButtonGL : public SpriteCellButtonGL {
        
    public:
        // sprite is destroyed by this class
        MusicCellButtonGL( Sprite *inSprite,
                           double inSpriteDrawScale,
                           double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );

        void setMusicInfo( int inPartNumber, int inPhraseNumber );
        
        // override to draw timeline
        void drawUnpressed();
        

    protected:
        Sprite mTimeMark;
        int mPartNumber, mPhraseNumber;
    };







// a button that has a double-border when selected
// make sure sizing makes room for two borders
class SelectableButtonGL : public SpriteButtonGL {
    public:
        SelectableButtonGL( Sprite *inSprite,
                            double inSpriteDrawScale,
                            double inAnchorX, double inAnchorY, 
                            double inWidth, double inHeight );
        
        void setSelected( char inSelected );
        char getSelected();
        


    protected:
        
        // overrides from BorderButtonGL
        virtual void drawBorder();

    private:
        
        char mSelected;

        double mOuterAnchorX, mOuterAnchorY, mOuterWidth, mOuterHeight;
    };




// button that must be pressed twice before firing
// confirmation
class DoublePressSpriteButtonGL : public SpriteButtonGL {
        
    public:
        // sprite is destroyed by this class
        DoublePressSpriteButtonGL( Sprite *inSprite, Sprite *inConfirmSprite,
                                   double inSpriteDrawScale,
                                   double inAnchorX, double inAnchorY, 
                                   double inWidth, double inHeight );
        
        ~DoublePressSpriteButtonGL();

        // resets button back to first, unpressed state
        //void resetPressState();
        
        // override to unpress when disabled
        void setEnabled( char inEnabled );
        
        
        // overrides to PREVENT sprite replacement
        virtual void setSprite( Sprite *inSprite );
        
        // overrides
        virtual void setToolTip( const char *inTipTranslationKey );
        virtual void mouseReleased( double inX, double inY );
    
    protected:
        Sprite *mOtherSprite;
        
        char mPressedOnce;

        char *mFirstTip;
        char *mConfirmTip;

    };




class DeleteButtonGL : public DoublePressSpriteButtonGL {
        
    public:
        DeleteButtonGL( double inAnchorX, double inAnchorY, 
                        double inWidth, double inHeight );
        
    };



class SendButtonGL : public DoublePressSpriteButtonGL {
        
    public:
        SendButtonGL( double inAnchorX, double inAnchorY, 
                      double inWidth, double inHeight );
        
    };




// button that that has two states and two sprites (toggles when pressed)
class ToggleSpriteButtonGL : public SpriteButtonGL {
        
    public:
        // sprite is destroyed by this class
        ToggleSpriteButtonGL( Sprite *inSprite, Sprite *inSecondSprite,
                              double inSpriteDrawScale,
                              double inAnchorX, double inAnchorY, 
                              double inWidth, double inHeight );
        
        ~ToggleSpriteButtonGL();
        
        char getState() {
            return mState;
            }
        
        void setState( char inState );
        
        
        virtual void setSecondToolTip( const char *inTipTranslationKey );
        
        // overrides to PREVENT sprite replacement
        virtual void setSprite( Sprite *inSprite );
        
        // overrides
        virtual void setToolTip( const char *inTipTranslationKey );
        virtual void mouseReleased( double inX, double inY );
    
    protected:
        Sprite *mOtherSprite;
        
        char mState;

        char *mFirstTip;
        char *mSecondTip;
    };




// a ToggleSpriteButtonGL that becomes brighter when it's scale note is
// being played
class ScaleToggleButton : public ToggleSpriteButtonGL {
        
    public:
        ScaleToggleButton( Sprite *inSprite, Sprite *inSecondSprite,
                           double inSpriteDrawScale,
                           double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );

        void setMusicInfo( int inNoteNumber, int inNotesPerOctave,
                           int inPartToWatch );
        
        // override to change draw color
        void drawUnpressed();
        void drawPressed();
        

    protected:
        Color *getDrawColor();
            
        int mNoteNumber, mNotesPerOctave, mPartToWatch;
        double mOnFade;
        int mLastColumnOn;
    };





class StackSearchButtonGL : public ToggleSpriteButtonGL {
        
    public:
        StackSearchButtonGL( double inAnchorX, double inAnchorY, 
                             double inWidth, double inHeight );
    };





// buttons that display TWO layered sprites inside a border
// sprites can be NULL
// invisible when disabled
class TwoSpriteButtonGL : public SpriteButtonGL {
        
    public:
        // sprites are destroyed by this class
        TwoSpriteButtonGL( Sprite *inSprite,
                           Sprite *inFrontSprite,
                           double inSpriteDrawScale,
                           double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );
        
        virtual ~TwoSpriteButtonGL();
        
        // replaces sprite
        virtual void setFrontSprite( Sprite *inSprite );
        
        virtual void drawPressed();

        virtual void drawUnpressed();
    
    protected:
        Sprite *mFrontSprite;
                
    };




// version of two-sprite button that fires action upon press too, to
// potentially initiate drag and drop functionality
class DraggableTwoSpriteButtonGL : public TwoSpriteButtonGL {
        
    public:
        // sprites are destroyed by this class
        DraggableTwoSpriteButtonGL( Sprite *inSprite,
                           Sprite *inFrontSprite,
                           double inSpriteDrawScale,
                           double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );
        
        // override functions in ButtonGL
        virtual void mousePressed( double inX, double inY );
        virtual void mouseReleased( double inX, double inY );
        
        char mLastActionFromPress;
    };





#endif
