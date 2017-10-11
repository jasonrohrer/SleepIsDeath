#ifndef UNIQUE_ID_INCLUDED
#define UNIQUE_ID_INCLUDED

// length of a unique ID  (only first 6 bytes of SHA1 needed)
#define U    6


typedef struct uniqueID {
        unsigned char bytes[U];
    } uniqueID;



extern uniqueID defaultID;



uniqueID makeUniqueID( unsigned char *inData, int inLength );



#include "minorGems/crypto/hashes/sha1.h"

typedef SHA_CTX partialUniqueID;


// for building a unique ID from several chunks of data
partialUniqueID startUniqueID();
partialUniqueID addToUniqueID( partialUniqueID inPartial, 
                               unsigned char *inData, int inLength );
uniqueID makeUniqueID( partialUniqueID inPartial );


// reads a unique ID from a data string
uniqueID readUniqueID( unsigned char *inBytes, int inLength,
                       int *outNumUsed );


// reads an array of unique IDs from a data string
// stored in space pointed to by outDest
// returns true on success
char readUniqueIDs( uniqueID *outDest, int inNumToRead,
                    unsigned char *inBytes, int inLength,
                    int *outNumUsed);


char equal( uniqueID inA, uniqueID inB );


// gets ID in hex as a \0-terminated string
// destroyed by caller
char *getHumanReadableString( uniqueID inID );


// returns true on success
char parseHumanReadableString( char *inString, uniqueID *outResult );



#endif


