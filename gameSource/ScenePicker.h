#ifndef SCENE_PICKER_INCLUDED
#define SCENE_PICKER_INCLUDED 

#include "Scene.h"
#include "ResourcePicker.h"
// must include this to avoid linker errors
#include "ResourcePicker.cpp"


class ScenePicker : public ResourcePicker<Scene> {


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
        ScenePicker( double inAnchorX, double inAnchorY );



        virtual ~ScenePicker();

    };



#endif



