#include "SongPicker.h"

// all share one stack
SimpleVector<uniqueID> globalSongStack;


SongPicker::SongPicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Song>( inAnchorX, inAnchorY, &globalSongStack ) {
    }




SongPicker::~SongPicker() {

    }
