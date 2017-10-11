#ifndef ROOM_PICKER_INCLUDED
#define ROOM_PICKER_INCLUDED 

#include "Room.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class RoomPicker : public ResourcePicker<Room> {


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
        RoomPicker( double inAnchorX, double inAnchorY );



        virtual ~RoomPicker();

    };



#endif



