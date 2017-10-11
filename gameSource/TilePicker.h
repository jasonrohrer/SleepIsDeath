#ifndef TILE_PICKER_INCLUDED
#define TILE_PICKER_INCLUDED 

#include "Tile.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class TilePicker : public ResourcePicker<Tile> {


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
        TilePicker( double inAnchorX, double inAnchorY );



        virtual ~TilePicker();



        // used for background in other pickers
        // does not change when selected tile in picker changes


        // fires an event to listeners
        void setBackgroundTile( Tile inTile );
        
        Tile getBackgroundTile();
        


    protected:
        

        Tile mChosenBackgroundTile;
        
    };



#endif



