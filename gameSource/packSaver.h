
#include "uniqueID.h"


void initPackSaver();
void freePackSaver();


// add to the running pack
// word string and data destroyed by caller (copied internally)
void addToPack( const char *inResourceType,
                uniqueID inID,
                char *inWordString,
                unsigned char *inData, int inLength );

char alreadyInPack( uniqueID inID );


// saves the pack and clears the running pack
void savePack();


// loads all packs in the pack dir
void loadPacks();

