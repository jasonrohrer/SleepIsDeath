#include "DrawToolSet.h"



DrawToolSet::DrawToolSet( double inAnchorX, double inAnchorY,
                          const char *inPickButtonTipKey,
                          char inShowStamp )
        : GUIPanelGL( inAnchorX, inAnchorY, 120, 20,
                      // transparent bg
                      new Color( 0, 0, 0, 0 ) ) {
    
    mPenButton = new SelectableButtonGL( 
        new Sprite( "pen.tga", true ),
        1,
        inAnchorX, inAnchorY, 20, 20 );

    mHorLineButton = new SelectableButtonGL( 
        new Sprite( "horLine.tga", true ),
        1,
        inAnchorX + 20, inAnchorY, 20, 20 );

    mVerLineButton = new SelectableButtonGL( 
        new Sprite( "verLine.tga", true ),
        1,
        inAnchorX + 40, inAnchorY, 20, 20 );
    
    mFillButton = new SelectableButtonGL( 
        new Sprite( "fill.tga", true ),
        1,
        inAnchorX + 60, inAnchorY, 20, 20 );
    
    mPickColorButton = new SelectableButtonGL( 
        new Sprite( "pickColor.tga", true ),
        1,
        inAnchorX + 80, inAnchorY, 20, 20 );

    mStampButton = new SelectableButtonGL( 
        new Sprite( "stamp.tga", true ),
        1,
        inAnchorX + 100, inAnchorY, 20, 20 );


    mPenButton->setToolTip( "tip_pen" );
    mHorLineButton->setToolTip( "tip_horLine" );
    mVerLineButton->setToolTip( "tip_verLine" );
    mFillButton->setToolTip( "tip_fill" );
    mPickColorButton->setToolTip( inPickButtonTipKey );
    mStampButton->setToolTip( "tip_stamp" );


    
    add( mPenButton );
    add( mHorLineButton );
    add( mVerLineButton );
    add( mFillButton );
    add( mPickColorButton );
    add( mStampButton );
    
    mPenButton->addActionListener( this );
    mHorLineButton->addActionListener( this );
    mVerLineButton->addActionListener( this );
    mFillButton->addActionListener( this );
    mPickColorButton->addActionListener( this );
    mStampButton->addActionListener( this );
    
    mStampButton->setEnabled( inShowStamp );

    mPenButton->setSelected( true );

    mLastSelectedBeforeShift = mPenButton;
    }



drawTool DrawToolSet::getSelected() {
    if( mPenButton->getSelected() ) {
        return pen;
        }
    if( mHorLineButton->getSelected() ) {
        return horLine;
        }
    if( mVerLineButton->getSelected() ) {
        return verLine;
        }
    if( mFillButton->getSelected() ) {
        return fill;
        }
    if( mPickColorButton->getSelected() ) {
        return pickColor;
        }
    if( mStampButton->getSelected() ) {
        return stamp;
        }
    
    return pen;
    }


        
void DrawToolSet::actionPerformed( GUIComponent *inTarget ) {    
    // select hit, turn others off
    mPenButton->setSelected( inTarget == mPenButton );
    mHorLineButton->setSelected( inTarget == mHorLineButton );
    mVerLineButton->setSelected( inTarget == mVerLineButton );
    mFillButton->setSelected( inTarget == mFillButton );
    mPickColorButton->setSelected( inTarget == mPickColorButton );
    mStampButton->setSelected( inTarget == mStampButton );

    fireActionPerformed( this );
    }



char DrawToolSet::isFocused() {
    return true;
    }

void DrawToolSet::specialKeyPressed( int inKey, double inX, double inY ) {
    if( inKey == MG_KEY_RSHIFT || inKey == MG_KEY_LSHIFT ) {
        // save last selected
        if( mPenButton->getSelected() ) {
            mLastSelectedBeforeShift = mPenButton;
            }
        else if( mHorLineButton->getSelected() ) {
            mLastSelectedBeforeShift = mHorLineButton;
            }
        else if( mVerLineButton->getSelected() ) {
            mLastSelectedBeforeShift = mVerLineButton;
            }
        else if( mFillButton->getSelected() ) {
            mLastSelectedBeforeShift = mFillButton;
            }
        else if( mPickColorButton->getSelected() ) {
            mLastSelectedBeforeShift = mPickColorButton;
            }
        else if( mStampButton->getSelected() ) {
            mLastSelectedBeforeShift = mStampButton;
            }

        actionPerformed( mPickColorButton );
        }
    }

void DrawToolSet::specialKeyReleased( int inKey, double inX, double inY ) {
    if( inKey == MG_KEY_RSHIFT || inKey == MG_KEY_LSHIFT ) {
        actionPerformed( mLastSelectedBeforeShift );
        }
    }


