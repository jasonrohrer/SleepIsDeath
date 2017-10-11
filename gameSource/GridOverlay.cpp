#include "GridOverlay.h"

#include <GL/gl.h>



GridOverlay::GridOverlay( double inAnchorX, double inAnchorY, 
                          double inWidth, double inHeight,
                          int inNumCells, Color inColor )
        : GUIComponentGL( inAnchorX, inAnchorY, inWidth, inHeight ),
          mNumCells( inNumCells ),
          mColor( inColor ) {

    }


void GridOverlay::fireRedraw() {


    double cellWidth = mWidth / mNumCells;
    double cellHeight = mHeight / mNumCells;
    

    glColor4f( mColor.r, mColor.g, mColor.b, mColor.a ); 
    glBegin( GL_LINES ); {
        
        for( int x=0; x<mNumCells+1; x++ ) {
            glVertex2d( mAnchorX + x * cellWidth, mAnchorY );
            glVertex2d( mAnchorX + x * cellWidth, mAnchorY + mHeight );
            }
        for( int y=0; y<mNumCells+1; y++ ) {
            glVertex2d( mAnchorX, mAnchorY + y * cellHeight );
            glVertex2d( mAnchorX + mWidth, mAnchorY + y * cellHeight );
            }
        }
    glEnd();
    }

