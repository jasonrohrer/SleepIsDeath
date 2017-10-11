#include "BorderPanel.h"




BorderPanel::BorderPanel(
    double inAnchorX, double inAnchorY, double inWidth,
    double inHeight, Color *inColor, Color *inBorderColor,
    double inBorderWidth  )
    // make panel contents smaller
    : GUIPanelGL( inAnchorX + inBorderWidth, inAnchorY + inBorderWidth, 
                  inWidth - 2 * inBorderWidth, inHeight - 2 * inBorderWidth, 
                  inColor ),
      // draw the exteral panel to our full dimensions
      mBorder( new GUIPanelGL( inAnchorX, inAnchorY, inWidth, inHeight, 
                               inBorderColor ) ) {

    }



BorderPanel::~BorderPanel() {
    delete mBorder;
    }

        
        
void BorderPanel::fireRedraw() {

    mBorder->fireRedraw();
    
    // superclass redraw to fill contents
    GUIPanelGL::fireRedraw();
    }
