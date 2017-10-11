#ifndef MUSIC_PICKER_INCLUDED
#define MUSIC_PICKER_INCLUDED 

#include "Music.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class MusicPicker : public ResourcePicker<Music> {


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
        MusicPicker( double inAnchorX, double inAnchorY );



        virtual ~MusicPicker();

    };



#endif



