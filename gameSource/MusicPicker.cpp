#include "MusicPicker.h"

// all share one stack
SimpleVector<uniqueID> globalMusicStack;


MusicPicker::MusicPicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Music>( inAnchorX, inAnchorY, &globalMusicStack ) {
    }




MusicPicker::~MusicPicker() {

    }
