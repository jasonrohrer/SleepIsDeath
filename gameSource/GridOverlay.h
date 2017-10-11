#ifndef GRID_OVERLAY_INCLUDED
#define GRID_OVERLAY_INCLUDED


#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/graphics/Color.h"


static Color defaultGridColor( 1, 1, 1, 0.25f );


// transparent with white grid lines over component area.
// should be added last in root panel so that it is on top
class GridOverlay : public GUIComponentGL {
        
    public:

        GridOverlay( double inAnchorX, double inAnchorY, 
                     double inWidth, double inHeight,
                     int inNumCells, Color inColor = defaultGridColor );
        
        virtual void fireRedraw();

    protected:
        int mNumCells;
        Color mColor;
        
                
        
    };



#endif
