#include "ToolTipDisplay.h"
#include "ToolTipManager.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/util/stringUtils.h"


extern TextGL *largeText;



ToolTipDisplay::ToolTipDisplay( double inAnchorX, double inAnchorY )
        : GUIComponentGL( inAnchorX, inAnchorY, 224, 8 ),
          mLastTip( NULL ), mFadeFactor( 0 ) {
    }



ToolTipDisplay::~ToolTipDisplay() {
    if( mLastTip != NULL ) {
        delete [] mLastTip;
        }
    }



char *ToolTipDisplay::getTip() {
    return ToolTipManager::getTip();
    }




void ToolTipDisplay::fireRedraw() {
    
    
    char *tip = getTip();
    

    if( tip != NULL ) {
        
        if( mLastTip == NULL ) {
            mLastTip = stringDuplicate( tip );
            }
        else {
            if( strcmp( mLastTip, tip ) != 0 ) {
                // tip changed
                delete [] mLastTip;
                mLastTip = stringDuplicate( tip );
                }
            }
        
        mFadeFactor = 1;
        }
    else {
        mFadeFactor -= 0.02;
    
        if( mFadeFactor < 0 ) {
            mFadeFactor = 0;
            
            if( mLastTip != NULL ) {
                delete [] mLastTip;
                mLastTip = NULL;
                }
            }
        }
    
    
    if( mLastTip != NULL ) {
        // left aligned
        
        int drawWidth = strlen( mLastTip ) * 8;
        
    
        Color *oldColor = largeText->getFontColor()->copy();
        
        Color *tempColor = oldColor->copy();
        tempColor->a = mFadeFactor;
        largeText->setFontColor( tempColor );
        
        largeText->drawText( mLastTip, mAnchorX,
                             mAnchorY, drawWidth, 8 );
        
        largeText->setFontColor( oldColor );
        }
    }

    
