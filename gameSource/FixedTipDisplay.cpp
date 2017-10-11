
#include "FixedTipDisplay.h"
#include "minorGems/util/stringUtils.h"
#include <stdio.h>


FixedTipDisplay::FixedTipDisplay( double inAnchorX, double inAnchorY )
        : ToolTipDisplay( inAnchorX, inAnchorY ),
          mTip( NULL ) {

    }


FixedTipDisplay::~FixedTipDisplay() {
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }
    }



char *FixedTipDisplay::getTip() {
    return mTip;
    }


void FixedTipDisplay::setTip( char *inTip ) {
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }

    if( inTip != NULL ) {
        mTip = stringDuplicate( inTip );
        }
    }
