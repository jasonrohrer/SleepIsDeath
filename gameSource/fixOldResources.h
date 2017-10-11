#include "uniqueID.h"

#include <stdio.h>


// fixes old v13 resources, replacing them with v14 resources 
// (to avoid cache duplicates)

// if none specified, all in cache are processed
void fixOldResources( uniqueID *inScenes = NULL, int inNumScenes = -1,
                      uniqueID *inObjects = NULL, int inNumObjects = -1 );
