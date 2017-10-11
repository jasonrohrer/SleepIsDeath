#include "TimerDisplay.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/util/stringUtils.h"


extern TextGL *largeTextFixed;



TimerDisplay::TimerDisplay( double inAnchorX, double inAnchorY )
        : ToolTipComponentGL( inAnchorX, inAnchorY, 16, 8 ),
          mTime( 0 ),
          mBlinkCycle( 0 ),
          mNegativeTipSet( false ),
          mFrozen( false ) {

    setToolTip( "tip_timer" );
    }



TimerDisplay::~TimerDisplay() {
    }


extern char isControllerGame;

extern char *addressString;


#include "minorGems/util/TranslationManager.h"


void TimerDisplay::freeze( char inFrozen ) {
    char oldFrozen = mFrozen;
    mFrozen = inFrozen;
    
    if( mFrozen && !oldFrozen ) {
        if( ! isControllerGame || addressString == NULL ) {    
            setToolTip( "tip_noConnection" );
            }
        else {
            // put waiting address in tip
            char *tip = autoSprintf( "%s %s",
                                     TranslationManager::translate(
                                         "tip_noConnection_address" ),
                                     addressString );
            setToolTip( tip );
            
            delete [] tip;
            }
        }
    else if( !mFrozen && oldFrozen ) {
        if( mNegativeTipSet ) {
            setToolTip( "tip_waiting" );
            }
        else {
            setToolTip( "tip_timer" );
            }
        }            
    }



void TimerDisplay::setTime( int inSecondsLeft ) {
    mTime = inSecondsLeft;

    if( mTime <= 0 && !mNegativeTipSet ) {
        if( !mFrozen ) {  // frozen tip overrides
            setToolTip( "tip_waiting" );
            mNegativeTipSet = true;
            }
        }
    else if( mTime > 0 && mNegativeTipSet ) {
        if( !mFrozen ) {  // frozen tip overrides
            setToolTip( "tip_timer" );
            mNegativeTipSet = false;
            }
        }
    
    }



void TimerDisplay::fireRedraw() {

    char *timeString = autoSprintf( "%d", mTime );
    
    
    // right aligned
        
    int drawWidth = strlen( timeString ) * 8;
        
    
    Color *oldColor = largeTextFixed->getFontColor()->copy();
    
    Color *tempColor = oldColor->copy();
    
    // timer becomes redder during last 10 seconds
    if( mTime <= 10 ) {    
        tempColor->g = (mTime - 5) / 5.0;
        tempColor->b = (mTime - 5) / 5.0;
        }
    
    if( mTime <= 5 && mTime > 0 ) {
        mBlinkCycle ++;
        
        tempColor->a = 0.5 * sin( mBlinkCycle / 2.0 ) + 0.5;
        }
    if( mTime == 0 ) {
        // reset
        mBlinkCycle = 0;
        }

    largeTextFixed->setFontColor( tempColor );
        
    
    
    largeTextFixed->drawText( timeString, mAnchorX + mWidth - drawWidth,
                              mAnchorY, drawWidth, 8 );
    
    largeTextFixed->setFontColor( oldColor );


    delete [] timeString;
    }

    
