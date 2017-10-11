#ifndef COLOR_WELLS_INCLUDED
#define COLOR_WELLS_INCLUDED 

#include "minorGems/graphics/openGL/gui/GUIComponentGL.h"
#include "minorGems/ui/event/ActionListenerList.h"
#include "minorGems/ui/event/ActionListener.h"

#include "color.h"
#include "BorderPanel.h"
#include "buttons.h"
#include "common.h"
#include "Palette.h"


class ColorWellButtonGL : public ToolTipButtonGL {
        
    public:
        ColorWellButtonGL( double inAnchorX, double inAnchorY, 
                           double inWidth, double inHeight );
        
        void setColor( rgbaColor inColor );
        
        void setSelected( char inSelected );

        virtual void drawPressed();

        virtual void drawUnpressed();

        // override to fire on drag
        virtual void mouseDragged( double inX, double inY );
        

    protected:
        char mSelected;
        Color mColor;
        
    };



class ColorWells : public BorderPanel, public ActionListenerList,
                   public ActionListener {


    public:



        /**
         * Constructs a well set.
         *
         * @param inAnchorX the x position of the upper left corner
         *   of this component.
         * @param inAnchorY the y position of the upper left corner
         *   of this component.
         * 
         * Sets its own width and height automatically.
         */
        ColorWells( double inAnchorX, double inAnchorY );



        virtual ~ColorWells();


        
        rgbaColor getSelectedColor();
        

        // jumps to color, if it exists, or replaces current well if it
        // does not
        // adds color to the currently selected well, replacing existing
        void pushColor( rgbaColor inColor, char inForceReplace = false );
        

        Palette getPalette();
        
        void setPalette( Palette inPalette );
        
        
        virtual void actionPerformed( GUIComponent *inTarget );
        
        
        char mLastActionWellChange;
        
        
    protected:


        
        
        static const int mColorGridW = 5;
        static const int mColorGridH = 8;
        
        ColorWellButtonGL *mColorButtons[mColorGridH][mColorGridW];
        rgbaColor mColorWells[mColorGridH][mColorGridW];

        ColorWellButtonGL *mMainColorButton;
        AddButtonGL *mAddButton;

        rgbaColor mMainColor;
        

        intPair mSelected;
        

    };



#endif



