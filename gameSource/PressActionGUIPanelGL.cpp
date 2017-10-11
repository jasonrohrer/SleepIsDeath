#include "PressActionGUIPanelGL.h"


PressActionGUIPanelGL::PressActionGUIPanelGL(
    double inAnchorX, double inAnchorY, double inWidth,
    double inHeight, Color *inColor )
        : GUIPanelGL( inAnchorX, inAnchorY, inWidth,
                      inHeight, inColor ) {

    }



void PressActionGUIPanelGL::mouseReleased( double inX, double inY ) {
    GUIPanelGL::mouseReleased( inX, inY );
    
    if( isInside( inX, inY ) ) {

        fireActionPerformed( this );
        }
    }

