// searchable database associating keywords with resources 

#include "uniqueID.h"



void initDatabase();
void freeDatabase();



void addData( const char *inResourceType,
              uniqueID inID,
              const char *inWordString );


void removeData( const char *inResourceType,
                 uniqueID inID );



int countSearchResults( const char *inResourceType,
                        const char *inSearchString );


// caller allocates spaces for inNumToGet and passes pointer as outIDs
// returns the number returned
int getSearchResults(  const char *inResourceType,
                       const char *inSearchString,
                       int inNumToSkip,
                       int inNumToGet,
                       uniqueID *outIDs );


// fast access to resource name without loading resource
// result NOT destroyed by caller
char *getResourceName( uniqueID inID );
