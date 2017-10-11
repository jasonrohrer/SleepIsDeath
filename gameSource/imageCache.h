
// cache for generated images that are frequently re-generated, but that
// don't change as long as inID remains the same
// caching saves generation time (especially for slow-to-generate ones like
// Rooms and StateObjects).


#include "uniqueID.h"

#include "minorGems/graphics/Image.h"




void initImageCache();


// free before program termination
void freeImageCache();


// inImage destroyed by the cache eventually
void addCachedImage( uniqueID inID, char inUseTrans, Image *inImage );



// returns NULL if image not found in cache... caller does not destroy result
Image *getCachedImage( uniqueID inID, char inUseTrans );



// clears any images associated with this id (trans or non-trans)
void clearCachedImages( uniqueID inID );
