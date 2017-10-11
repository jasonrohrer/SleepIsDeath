#include "ScalePicker.h"

// all share one stack
SimpleVector<uniqueID> globalScaleStack;


ScalePicker::ScalePicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Scale>( inAnchorX, inAnchorY, 
                                      &globalScaleStack ) {
    }




ScalePicker::~ScalePicker() {

    }
