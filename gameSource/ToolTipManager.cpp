#include "ToolTipManager.h"

#include "minorGems/util/stringUtils.h"


char *ToolTipManager::mTip = NULL;
char ToolTipManager::mFrozen = false;


void ToolTipManager::setTip( const char *inTip ) {
    if( mFrozen ) {
        return;
        }
    
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }

    if( inTip != NULL ) {
        mTip = stringDuplicate( inTip );
        }
    }


char *ToolTipManager::getTip() {
    return mTip;
    }



void ToolTipManager::freeze( char inFrozen ) {
    mFrozen = inFrozen;
    }

    
        

void ToolTipManager::staticInit() {
    }


void ToolTipManager::staticFree() {
    if( mTip != NULL ) {
        delete [] mTip;
        mTip = NULL;
        }
    }

