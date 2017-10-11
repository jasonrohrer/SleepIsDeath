#include "RoomPicker.h"

// all share one stack
SimpleVector<uniqueID> globalRoomStack;


RoomPicker::RoomPicker(
    double inAnchorX, double inAnchorY )
    : ResourcePicker<Room>( inAnchorX, inAnchorY, &globalRoomStack ) {
    }




RoomPicker::~RoomPicker() {

    }
