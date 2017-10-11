#ifndef COLOR_EDITOR_INCLUDED
#define COLOR_EDITOR_INCLUDED


#include "Editor.h"
#include "buttons.h"
#include "HueValuePicker.h"

#include "ToolTipSliderGL.h"


class ColorEditor : public Editor {
        
    public:
        
        ColorEditor( ScreenGL *inScreen );
        
        ~ColorEditor();

        virtual void actionPerformed( GUIComponent *inTarget );

        // triggered by add button or close
        // can be called externally to force edited color to top of stack
        void addColor();


        // true if controls are currently being dragged
        char getDragging();
        
        // true if closed due to press of edit palette
        char mEditPalettePressed;


        void setEditPaletteButtonVisible( char inIsVisible );
        

    protected:
        
        // implemented by all subclasses
        // called by parent class when editor is being closed
        virtual void editorClosing();


        void adjustBarColors();
        


        ToolTipSliderGL *mValueSlider;
        
        ToolTipSliderGL *mSaturationSlider;

        HueValuePicker *mHVPicker;
        

        Color *mWorkingColor;
        
        EditButtonGL *mEditPaletteButton;
        

        char mIgnoreEvent;
    };

#endif
