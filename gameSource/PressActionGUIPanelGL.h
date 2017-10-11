
#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"

#include "minorGems/ui/event/ActionListenerList.h"


// panel that fires an action when mouse pressed on it
// can be used to hold focus when mouse pressed outside a component of interest
//  (but hits containing panel)
class PressActionGUIPanelGL : public GUIPanelGL, public ActionListenerList {
    public:
        
        PressActionGUIPanelGL(
			double inAnchorX, double inAnchorY, double inWidth,
			double inHeight, Color *inColor );


        virtual ~PressActionGUIPanelGL() {
            }
        

        // override to fire action
        virtual void mouseReleased( double inX, double inY );
        
    };
