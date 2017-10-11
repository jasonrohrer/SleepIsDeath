#ifndef BORDER_PANEL_INCLUDED
#define BORDER_PANEL_INCLUDED 

#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"

#include <GL/gl.h>


/**
 * A panel with a colored border of a given width.
 *
 * @author Jason Rohrer
 */
class BorderPanel : public GUIPanelGL {


    public:



        /**
         * Constructs a panel.
         *
         * Parameters are same as for GUIPanelGL.
         */
        BorderPanel(
            double inAnchorX, double inAnchorY, double inWidth,
            double inHeight, Color *inColor, Color *inBorderColor,
            double inBorderWidth );


        
        virtual ~BorderPanel();


        
        // override fireRedraw() in GUIPanelGL
        virtual void fireRedraw();

    protected:

        GUIPanelGL *mBorder;
        

    };



#endif



