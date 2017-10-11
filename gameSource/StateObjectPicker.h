#ifndef STATE_OBJECT_PICKER_INCLUDED
#define STATE_OBJECT_PICKER_INCLUDED 

#include "StateObject.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


#include "buttons.h"



class StateObjectPicker : public ResourcePicker<StateObject> {


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
        StateObjectPicker( double inAnchorX, double inAnchorY );



        virtual ~StateObjectPicker();
        

        // override this so we can listen to tile-picker too
        virtual void actionPerformed( GUIComponent *inTarget );


    protected:
        
        // override, returning current tile as background to show
        // behind transparent sprites
        Sprite *getButtonGridBackground();

        // to toggle sprite trans
        SelectableButtonGL *mTransButton;

    };



#endif



