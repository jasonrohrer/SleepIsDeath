
#include "uniqueID.h"


// tests if we can write to cache folder
// only returns false if it exists but is not writeable
// can be called before init
// (returns true if it does not exist)
char testResourceCacheWritePermissions();


void initResourceManager();
void freeResourceManager();




// should failed resource loads be fetched from the network?
// if not, then NULL is returned on load failure.
//
// Defaults to true
void setUseNetwork( char inUseNetwork );




// loads a resource from file or network, if not found in file
//    caches resources fetched from network in file
// inResourceType is a string such as "tile", "sprite", etc.
// returns NULL on failure
// caller destroys result
//
// outCameFromNetwork flag passed out indicating that data was not found
// on disk and should be saved by caller (via saveResourceData)
unsigned char *loadResourceData( const char *inResourceType,
                                 uniqueID inID,
                                 int *outLength,
                                 char *outCameFromNetwork );


// loads data as a batch (more efficient if data coming across the network
unsigned char **loadResourceData( const char *inResourceType,
                                  int inNumResources,
                                  uniqueID *inIDs,
                                  int *outLengths,
                                  char *outCameFromNetwork );




// saves to file
void saveResourceData( const char *inResourceType,
                       uniqueID inID,
                       char *inWordString,
                       unsigned char *inData, int inLength );


void deleteResource( const char *inResourceType,
                     uniqueID inID );


char resourceExists( const char *inResourceType,
                     uniqueID inID );
