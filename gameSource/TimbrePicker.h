#ifndef TIMBRE_PICKER_INCLUDED
#define TIMBRE_PICKER_INCLUDED 

#include "TimbreResource.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class TimbrePicker : public ResourcePicker<TimbreResource> {


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
        TimbrePicker( double inAnchorX, double inAnchorY );



        virtual ~TimbrePicker();

    };



#endif



