#ifndef PALETTE_PICKER_INCLUDED
#define PALETTE_PICKER_INCLUDED 

#include "Palette.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class PalettePicker : public ResourcePicker<Palette> {


    public:



        /**
         * Constructs a picker.
         *
         * @param inAnchorX the x position of the upper left corner
         *   of this component.
         * @param inAnchorY the y position of the upper left corner
         *   of this component.
         * 
         * Sets its own width and height automatically.
         */
        PalettePicker( double inAnchorX, double inAnchorY );



        virtual ~PalettePicker();

    };



#endif



