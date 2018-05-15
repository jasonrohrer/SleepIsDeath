#include "resourceDatabase.h"
#include "StringTree.h"


// naive implementation for now with no intelligent index
// as performance issues grow, see this article:

// http://en.wikipedia.org/wiki/Substring_index 

#include "minorGems/util/SimpleVector.h"
#include "minorGems/system/Time.h"

#include "stdio.h"
#include "minorGems/io/file/File.h"

#include "minorGems/util/log/AppLog.h"


static File *getFullDBFile() {
    char *pathSteps[1];

    pathSteps[0] = (char*)"resourceCache";
    

    File *dbFile = new File( new Path( pathSteps, 1, false ),
                             "stringDatabase.txt" );
    
    return dbFile;
    }




typedef struct resourceRecord {
        const char *type;
        char *wordString;
        uniqueID id;
    } resourceRecord;


void printResourceRecord( void *inR ) {
    resourceRecord *r = (resourceRecord*)inR;
    char *idString = getHumanReadableString( r->id );
    
    printf( "Type %s, keyword %s, ID %s\n",
            r->type, r->wordString, idString );
    delete [] idString;
    }


static SimpleVector<char*> dataLines;


static SimpleVector<resourceRecord *> records;


// resource types stored in each tree
// one tree 
static SimpleVector<char *> recordTreeTypes;
static SimpleVector<StringTree *> recordTrees;




// hash table for quick check for duplicates on addData call

// number of bins
#define B 2000



SimpleVector<resourceRecord *> hashBins[B];

int getHashKey( uniqueID inID ) {
    unsigned int sum = 0;
    for( int i=0; i<U; i++ ) {
        
        sum += inID.bytes[i];
        }
    return sum % B;
    }

    

void hashInsert( resourceRecord *inRecord ) {
    hashBins[ getHashKey( inRecord->id ) ].push_back( inRecord );
    }

void hashRemove( resourceRecord *inRecord ) {
    uniqueID id = inRecord->id;
    SimpleVector<resourceRecord *> *bin = &( hashBins[ getHashKey( id ) ] );
    
    int numEntries = bin->size();
    for( int i=0; i<numEntries; i++ ) {
        resourceRecord *r = *( bin->getElement( i ) );
        if( equal( r->id, id ) ) {
            bin->deleteElement( i );
            return;
            }
        }
    }

char hashExists( uniqueID inID ) {
    SimpleVector<resourceRecord *> *bin = &( hashBins[ getHashKey( inID ) ] );
    
    int numEntries = bin->size();
    for( int i=0; i<numEntries; i++ ) {
        resourceRecord *r = *( bin->getElement( i ) );
        if( equal( r->id, inID ) ) {
            return true;
            }
        }
    return false;
    }


resourceRecord *hashLookup( uniqueID inID ) {
    SimpleVector<resourceRecord *> *bin = &( hashBins[ getHashKey( inID ) ] );
    
    int numEntries = bin->size();
    for( int i=0; i<numEntries; i++ ) {
        resourceRecord *r = *( bin->getElement( i ) );
        if( equal( r->id, inID ) ) {
            return r;
            }
        }
    return NULL;
    }





StringTree *getTreeForType( const char *inType ) {
    int numTrees = recordTrees.size();
    for( int i=0; i<numTrees; i++ ) {
        if( strcmp( inType, *( recordTreeTypes.getElement( i ) ) ) == 0 ) {
            // match
            return *( recordTrees.getElement( i ) );
            }
        }
    // no match
    
    // create a new one
    recordTreeTypes.push_back( stringDuplicate( inType ) );
    
    StringTree *returnVal = new StringTree();
    recordTrees.push_back( returnVal );
    
    return returnVal;
    }





static char writeNewEntriesToFile = true;




static char **getDataFileLines( int *outNumLines ) {
    File *fullDBFile = getFullDBFile();

    if( fullDBFile->exists() ) {
        
        char *fileContents = fullDBFile->readFileContents();
        
        delete fullDBFile;
        

        if( fileContents == NULL ) {
            AppLog::error( 
                "Error:  failed to read from stringDatabase.txt\n" );
            return NULL;
            }
        
        int numLines;
        char **lines = split( fileContents, "\n", &numLines );
        
        delete [] fileContents;

        *outNumLines = numLines;
        return lines;
        }
    else {
        AppLog::error( "Error:  stringDatabase.txt does not exist\n" );
        }
    
    delete fullDBFile;
    return NULL;
    }

    


void initDatabase() {

    
        
    double start = Time::getCurrentTime();

    
    writeNewEntriesToFile = false;
    
    int numLines;
    char **lines = getDataFileLines( &numLines );
    

    if( lines != NULL ) {
        
        AppLog::getLog()->logPrintf( 
            Log::INFO_LEVEL, "Splitting %d lines took %d ms\n",
            numLines, (int)( 1000 * (Time::getCurrentTime() - start ) ) );

        
        for( int i=0; i<numLines; i++ ) {
            
            char *line = lines[i];

            dataLines.push_back( line );

            int charsLeft = strlen( line );

            if( charsLeft < U * 2 ) {
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "Failed to read unique ID from line %d of string DB\n",
                    i );
                }
            else {
                
                // first 12 chars is unique ID
            
                char *idString = new char[ U * 2 + 1 ];
                
                memcpy( idString, line, U * 2 );
                
                idString[ U * 2 ] = '\0';
            
                uniqueID id;
                char result = parseHumanReadableString( idString, &id );
            
                if( !result ) {
                    AppLog::getLog()->logPrintf( 
                        Log::ERROR_LEVEL,
                        "Failed to read unique ID from "
                        "line %d of string DB\n", i );
                    }
                else {
                    
                    // skip ID and space
                    char *substring = &( line[ U * 2 + 1 ] );
            
            
                    char typeString[100];
                
                    int numRead = sscanf( substring, "%99s", typeString );
                
                    if( numRead == 1 ) {
                        char *skipPointer = &substring[ strlen( typeString ) 
                                                        + 1 ];
                    
                        // rest of contents is word string
                        addData( typeString, id, skipPointer );
                        }
                    }
                
            
                delete [] idString;    
                }

            }
        delete [] lines;
        }

    writeNewEntriesToFile = true;


    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Loading database cache from disk took %d ms\n",
        (int)( 1000 * (Time::getCurrentTime() - start ) ) );
    }



void freeDatabase() {
    for( int i=0; i<records.size(); i++ ) {
        resourceRecord *r = *( records.getElement(i) );
        
        delete [] r->type;
        delete [] r->wordString;
        
        delete r;
        }
    records.deleteAll();
    
    for( int i=0; i<dataLines.size(); i++ ) {
        delete [] *( dataLines.getElement( i ) );
        }
    dataLines.deleteAll();

    for( int i=0; i<recordTrees.size(); i++ ) {
        delete [] *( recordTreeTypes.getElement( i ) );
        
        delete *( recordTrees.getElement( i ) );
        }

    recordTreeTypes.deleteAll();
    recordTrees.deleteAll();
    }



void addData( const char *inResourceType,
              uniqueID inID,
              const char *inWordString ) {

    //double start = Time::getCurrentTime();

    //printf( "Adding data to database for %s,%s\n", 
    //        inResourceType, inWordString );


    // make sure there's no collision with existing data
    if( hashExists( inID ) ) {
        return;
        }
    
    
    resourceRecord *r = new resourceRecord;
    
    r->type = stringDuplicate( inResourceType );
    
    r->wordString = stringDuplicate( inWordString );
    r->id = inID;
    

    records.push_back( r );
    hashInsert( r );
    


    // New code:  string trees
    StringTree *t = getTreeForType( inResourceType );
    
    // lower case to make searches case-insensitive
    char *lowerCase = stringToLowerCase( r->wordString );
    t->insert( lowerCase, (void *)r );
    delete [] lowerCase;


    if( writeNewEntriesToFile ) {
        
        File *fullDBFile = getFullDBFile();
        
        char *fullFileName = fullDBFile->getFullFileName();
            
        delete fullDBFile;
        

        FILE *f = fopen( fullFileName, "a" );
        
        delete [] fullFileName;
        
        char *idString = getHumanReadableString( inID );
        
        char *line = autoSprintf( "%s %s %s", 
                                  idString, inResourceType, inWordString );

        dataLines.push_back( line );


        fprintf( f, "\n%s", line );
        fclose( f );
        
        delete [] idString;
        
        /*
        printf( "Adding data took %d ms\n",
            (int)( 1000 * (Time::getCurrentTime() - start ) ) );
        */
        }
    }



void removeData( const char *inResourceType,
                 uniqueID inID ) {

    //double start = Time::getCurrentTime();

    char found = false;
    
    for( int i=0; i<records.size() && !found; i++ ) {
        resourceRecord *r = *( records.getElement( i ) );

        if( equal( inID, r->id ) &&
            strcmp( inResourceType, r->type ) == 0 ) {

            // New code:  string trees
            StringTree *t = getTreeForType( inResourceType );
    
            
            // lower case for case-insensitive
            char *lowerCase = stringToLowerCase( r->wordString );
            t->remove( lowerCase, (void *)r );
            
            delete [] lowerCase;

            records.deleteElement( i );
            hashRemove( r );

            delete [] r->type;
            delete [] r->wordString;
            delete r;
            
            found = true;
            }
        }

    

    File *fullDBFile = getFullDBFile();

    char *fullFileName = fullDBFile->getFullFileName();
    delete fullDBFile;
    

    FILE *f = fopen( fullFileName, "w" );
        
    delete [] fullFileName;

        

    char *idString = getHumanReadableString( inID );
    char doneSkipping = false;
    
    char someLinesWritten = false;
        
    for( int i=0; i<dataLines.size(); i++ ) {    
        char *line = *( dataLines.getElement( i ) );
        
        if( doneSkipping || strncmp( idString, line, 12 ) != 0 ) {
            if( someLinesWritten ) {
                fprintf( f, "\n%s", line );
                }
            else {
                // first line, no newline
                fprintf( f, "%s", line );
                someLinesWritten = true;
                }
            }
        else {
            // skipped one!
            doneSkipping = true;
            delete [] line;
            dataLines.deleteElement( i );
            i--;
            }
        }
    delete [] idString;

    
    fclose( f );
    
    
    /*
    printf( "Removing data took %d ms\n",
            (int)( 1000 * (Time::getCurrentTime() - start ) ) );
    */
    }



static SimpleVector< resourceRecord *> *getUnionOfWordMatches( 
    SimpleVector<char *> *inWords, const char *inResourceType ) {
    
    StringTree *t = getTreeForType( (char*)inResourceType );


    SimpleVector< resourceRecord *> *unionMatches = 
        new SimpleVector< resourceRecord *>();
    
            
    for( int i=0; i<inWords->size(); i++ ) {
        int wordCount = 
            t->countMatches( *( inWords->getElement(i) ) );

        resourceRecord **values = new resourceRecord *[wordCount];
                
        // -1 means get all
        t->getMatches( *( inWords->getElement(i) ), 0, wordCount,
                       (void **)values );

        if( i == 0 ) {
            // populate union with first set            
            for( int w=0; w<wordCount; w++ ) {
                unionMatches->push_back( values[w] );
                }
            }
        else {
            for( int u=0; u<unionMatches->size(); u++ ) {
                // is this member of union in next set?
                resourceRecord *unionMember = 
                    *( unionMatches->getElement( u ) );
                        
                char found = false;
                        
                for( int w=0; w<wordCount && !found; w++ ) {
                    if( unionMember == values[w] ) {
                        found = true;
                        }
                    }
                        
                if( !found ) {
                    unionMatches->deleteElement( u );
                    u--;
                    }
                }
            }
        delete [] values;
        }

    return unionMatches;
    }




int countSearchResults( const char *inResourceType,
                        const char *inSearchString ) {

    //double start = Time::getCurrentTime();
    
    int count = 0;

    // New code:  string trees
    StringTree *t = getTreeForType( (char*)inResourceType );
    
    // lower-case in tree
    char *lowerSearchString = stringToLowerCase( inSearchString );
        


    if( lowerSearchString[0] == '\0' ) {
        // empty search, count all
        count = t->countMatches( lowerSearchString );
        }
    else {
        // how many words?
        SimpleVector<char *> *words = tokenizeString( lowerSearchString );

        if( words->size() == 0 ) {
            // ignore whitespace, count everything
            count = t->countMatches( "" );
            }
        else if( words->size() == 1 ) {
            count = t->countMatches( *( words->getElement(0) ) );
            }
        else {
            // multi-word, take union of results
            
            SimpleVector< resourceRecord *> *unionMatches = 
                getUnionOfWordMatches( words, inResourceType );

            // union now contains only element that match every word
            count = unionMatches->size();

            delete unionMatches;
            }

        for( int i=0; i<words->size(); i++ ) {
            delete [] *( words->getElement( i ) );
            }
        delete words;
        }
    

    delete [] lowerSearchString;

    /*
    printf( "Counting %d results took %d ms\n",
            count, (int)( 1000 * (Time::getCurrentTime() - start ) ) );
    */

    return count;
    }



// caller allocates spaces for inNumToGet and passes pointer as outIDs
int getSearchResults(  const char *inResourceType,
                       const char *inSearchString,
                       int inNumToSkip,
                       int inNumToGet,
                       uniqueID *outIDs ) {
    
    //double start = Time::getCurrentTime();
    
    int numGotten = 0;

    // New code:  string trees
    StringTree *t = getTreeForType( (char*)inResourceType );
    
    resourceRecord **values = new resourceRecord *[inNumToGet];
    
    // lower-case in tree
    char *lowerSearchString = stringToLowerCase( inSearchString );


    if( *lowerSearchString == '\0' ) {
        // empty search, consider everything
        numGotten = t->getMatches( lowerSearchString, inNumToSkip, inNumToGet,
                                   (void **)values );
        }
    else {
        // how many words?
        SimpleVector<char *> *words = tokenizeString( lowerSearchString );

        if( words->size() == 0 ) {
            // ignore spaces, show everything
            numGotten = t->getMatches( "", inNumToSkip, inNumToGet,
                                       (void **)values );
            }
        else if( words->size() == 1 ) {
            // just this word (no whitespace
            numGotten = t->getMatches( *( words->getElement(0) ), 
                                       inNumToSkip, inNumToGet,
                                       (void **)values );
            }
        else {
            // take union of results from multiple words, THEN
            // apply skip and limit
            SimpleVector< resourceRecord *> *unionMatches = 
                getUnionOfWordMatches( words, inResourceType );

            int count = unionMatches->size();
            
            numGotten = 0;
            
            for( int i=0; i<inNumToGet; i++ ) {
                if( i + inNumToSkip < count ) {
                    values[i] = 
                        *( unionMatches->getElement( i + inNumToSkip ) );
            
                    numGotten ++;
                    }
                }
            
            delete unionMatches;
            }

        for( int i=0; i<words->size(); i++ ) {
            delete [] *( words->getElement( i ) );
            }
        delete words;
        }
    
    delete [] lowerSearchString;

    for( int i=0; i<numGotten; i++ ) {
        outIDs[i] = values[i]->id;
        }
    delete [] values;
    
    /*
    printf( "Gettingresults took %d ms\n",
            (int)( 1000 * (Time::getCurrentTime() - start ) ) );
    */

    return numGotten;
    }




char *getResourceName( uniqueID inID ) {
    resourceRecord *r = hashLookup( inID );
    
    if( r != NULL ) {
        return r->wordString;
        }
    else {
        return NULL;
        }
    }


