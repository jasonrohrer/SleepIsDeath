#ifndef SPRITE_PICKER_INCLUDED
#define SPRITE_PICKER_INCLUDED 

#include "SpriteResource.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


#include "buttons.h"



class SpritePicker : public ResourcePicker<SpriteResource> {


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
        SpritePicker( double inAnchorX, double inAnchorY );



        virtual ~SpritePicker();
        

        // override this so we can listen to tile-picker too
        virtual void actionPerformed( GUIComponent *inTarget );

        // override to copy trans button state
        virtual void cloneState( 
            ResourcePicker<SpriteResource> *inOtherPicker );
        

        // gets state of trans button
        char isTransOn();

        // so that others can add action listeners to it
        SelectableButtonGL *getTransButton();
        


    protected:

        
        // override, returning current tile as background to show
        // behind transparent sprites
        Sprite *getButtonGridBackground();

        // to toggle sprite trans
        SelectableButtonGL *mTransButton;

    };



#endif



