#include "PalettePicker.h"

// all share one stack
SimpleVector<uniqueID> globalPaletteStack;


PalettePicker::PalettePicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Palette>( inAnchorX, inAnchorY, &globalPaletteStack ) {
    }




PalettePicker::~PalettePicker() {

    }
