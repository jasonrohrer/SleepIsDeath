#include "WarningDisplay.h"



WarningDisplay::WarningDisplay( double inAnchorX, double inAnchorY )
        : ToolTipComponentGL( inAnchorX, inAnchorY, 16, 16 ),
          mBlinkCycle( 0 ), 
          mFramesLeft( 0 ),
          mSprite( "warning.tga", true ) {
    
    setEnabled( false );
    }



WarningDisplay::~WarningDisplay() {
    }


extern char isControllerGame;

#include "minorGems/network/HostAddress.h"
#include "minorGems/util/TranslationManager.h"


void WarningDisplay::show( const char *inToolTipTransKey, int inFrames ) {
    setToolTip( inToolTipTransKey );
    
    setEnabled( true );
    
    mBlinkCycle = 0;
    mFramesLeft = inFrames;
    }



void WarningDisplay::fireRedraw() {
    if( mEnabled ) {
        mBlinkCycle ++;
        
        mFramesLeft --;
        


        double fade = 0.5 * sin( mBlinkCycle / 2.0 ) + 0.5;
        
        if( mFramesLeft < 30 ) {
            // fade out gently for last second
            fade *= (double)mFramesLeft / 30;
            }
        

        Vector3D pos( mAnchorX + mWidth / 2,
                      mAnchorY + mHeight / 2,
                      0 );
    
                  

        mSprite.draw( 0, 0, &pos,
                      1,
                      fade );

        if( mFramesLeft == 0 ) {
            setEnabled( false );
            }
        }
    
    }

    
