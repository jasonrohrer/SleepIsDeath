#ifndef SCALE_PICKER_INCLUDED
#define SCALE_PICKER_INCLUDED 

#include "Scale.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class ScalePicker : public ResourcePicker<Scale> {


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
        ScalePicker( double inAnchorX, double inAnchorY );



        virtual ~ScalePicker();

    };



#endif



