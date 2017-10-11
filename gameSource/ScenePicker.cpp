#include "ScenePicker.h"

// all share one stack
SimpleVector<uniqueID> globalSceneStack;


ScenePicker::ScenePicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Scene>( inAnchorX, inAnchorY, &globalSceneStack ) {
    }




ScenePicker::~ScenePicker() {

    }
