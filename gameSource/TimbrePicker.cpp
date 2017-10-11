#include "TimbrePicker.h"

// all share one stack
SimpleVector<uniqueID> globalTimbreStack;


TimbrePicker::TimbrePicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<TimbreResource>( inAnchorX, inAnchorY, 
                                      &globalTimbreStack ) {
    }




TimbrePicker::~TimbrePicker() {

    }
