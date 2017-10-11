// tracks speech hints for objects (last time speech offset was assigned 
// for an object by a scene selection or user action). 

#include "uniqueID.h"
#include "common.h"



// must be called after resourceManager init call
void initSpeechHints();
void freeSpeechHints();



// inID uses inUsesID
void setSpeechHint( uniqueID inObjectID,
                    intPair inSpeechOffset, char inSpeechflip );

intPair getSpeechHint( uniqueID inObjectID, char *outSpeechFlip );


void clearSpeechHint( uniqueID inObjectID );
