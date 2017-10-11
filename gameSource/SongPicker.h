#ifndef SONG_PICKER_INCLUDED
#define SONG_PICKER_INCLUDED 

#include "Song.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class SongPicker : public ResourcePicker<Song> {


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
        SongPicker( double inAnchorX, double inAnchorY );



        virtual ~SongPicker();

    };



#endif



