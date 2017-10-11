// database tracking which resources use which other resources 

#include "uniqueID.h"



void initUsageDatabase();
void freeUsageDatabase();



// inID uses inUsesID
void addUsage( uniqueID inID,
               uniqueID inUsesID );

// remove all usages associated with inID
// note that because inID is a hash of the using object, removing one
// usage from that object means that the ID of the object will change
// Thus, we would never be removing a usage from inID unless we were deleting
// inID entirely, which means we'd want to remove ALL usages from inID
void removeUsages( uniqueID inID );



char isUsed( uniqueID inID );


// if isUsed returns true, returns ID of one of the resources using inID
uniqueID getUser( uniqueID inID );
