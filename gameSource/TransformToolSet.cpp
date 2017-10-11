#include "TransformToolSet.h"



TransformToolSet::TransformToolSet( double inAnchorX, double inAnchorY,
                                    char inHasColorize )
        : GUIPanelGL( inAnchorX, inAnchorY, 16, 100,
                      new Color( 0, 0, 0, 0 ) ),
          mLastPressed( flipH ) {
    
    

    mFlipHButton = new FlipHButtonGL( 
        inAnchorX, inAnchorY, 16, 16 );

    mFlipVButton = new FlipVButtonGL( 
        inAnchorX, inAnchorY + 18, 16, 16 );

    mCCWButton = new RotateCCWButtonGL( 
        inAnchorX, inAnchorY + 36, 16, 16 );
    
    mCWButton = new RotateCWButtonGL( 
        inAnchorX, inAnchorY + 54, 16, 16 );

    if( inHasColorize ) {
        mColorizeButton = new ColorizeButtonGL( 
            inAnchorX, inAnchorY + 72, 16, 16 );

        mClearButton = new ClearButtonGL( 
            inAnchorX, inAnchorY + 90, 16, 16 );
        }
    else {
        mColorizeButton = NULL;
        mClearButton = new ClearButtonGL( 
            inAnchorX, inAnchorY + 72, 16, 16 );
        }
    

    add( mFlipHButton );
    add( mFlipVButton );
    add( mCCWButton );
    add( mCWButton );
    add( mClearButton );
    
        

    mFlipHButton->addActionListener( this );
    mFlipVButton->addActionListener( this );
    mCCWButton->addActionListener( this );
    mCWButton->addActionListener( this );
    mClearButton->addActionListener( this );

    if( mColorizeButton != NULL ) {
        add( mColorizeButton );
        mColorizeButton->addActionListener( this );
        }
    }



transformTool TransformToolSet::getLastPressed() {
    return mLastPressed;
    }


        
void TransformToolSet::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == mFlipHButton ) {
        mLastPressed = flipH;
        }
    else if( inTarget == mFlipVButton ) {
        mLastPressed = flipV;
        }
    else if( inTarget == mCCWButton ) {
        mLastPressed = rotateCCW;
        }
    else if( inTarget == mCWButton ) {
        mLastPressed = rotateCW;
        }
    else if( inTarget == mClearButton ) {
        mLastPressed = clear;
        }
    else if( inTarget == mColorizeButton ) {
        mLastPressed = colorize;
        }
    
    fireActionPerformed( this );
    }
