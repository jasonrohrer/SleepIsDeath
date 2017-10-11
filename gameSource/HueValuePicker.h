#ifndef HUE_VALUE_PICKER_INCLUDED
#define HUE_VALUE_PICKER_INCLUDED

#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/ui/event/ActionListenerList.h"

#include "minorGems/graphics/Image.h"

#include "color.h"
#include "BorderPanel.h"
#include "Sprite.h"


class HueValuePicker : public BorderPanel, public ActionListenerList {


    public:



        // w and h MUST be powers of two!
        HueValuePicker( double inAnchorX, double inAnchorY,
                        double inWidth, double inHeight );



        virtual ~HueValuePicker();


        // 0-1
        float getSelectedHue();
        float getSelectedValue();
        

        void setValues( float inHue, float inValue );
        
                

        
        // override fireRedraw in GUIComponentGL
		virtual void mousePressed( double inX, double inY );
		virtual void mouseDragged( double inX, double inY );
		virtual void mouseReleased( double inX, double inY );
        virtual void fireRedraw();
        
        
        // true if currently active from an on-picker press
        char mPressed;
        
        
    protected:
        
        void mouseActivity( double inX, double inY );

        Image *mFieldImage;
        Sprite *mFieldSprite;
        
        Sprite *mColorSpot;

        float mH;
        float mV;
        

        

    };



#endif



