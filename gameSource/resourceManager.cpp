#include "resourceManager.h"
#include "resourceDatabase.h"
#include "Connection.h"
#include "common.h"

#include <stdio.h>



#include "minorGems/io/file/File.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/log/AppLog.h"

#include "minorGems/system/Thread.h"
#include "minorGems/system/Time.h"




extern Connection *connection;


char testResourceCacheWritePermissions() {
    File cacheDir( NULL, "resourceCache" );
    
    if( !cacheDir.exists() ) {
        return true;
        }
    else {
        File *writeFile = cacheDir.getChildFile( "testWrite.txt " );
        
        char success = writeFile->writeToFile( "test" );
        
        writeFile->remove();
        
        delete writeFile;
        
        return success;
        }
    }




void initResourceManager() {
    
    File cacheDir( NULL, "resourceCache" );
    
    if( !cacheDir.exists() ) {
        AppLog::warning( "No resourceCache directory found, creating it" );
        cacheDir.makeDirectory();
        }
    
    if( cacheDir.exists() && cacheDir.isDirectory() ) {
        // good
        }
    else {
        AppLog::criticalError( 
            "ERROR: resourceCache not found, and could not create it" );
        }
    }



void freeResourceManager() {
    }






static char useNetwork = true;


void setUseNetwork( char inUseNetwork ) {
    useNetwork = inUseNetwork;
    }


static File *getResourceDir( const char *inResourceType ) {
    char *pathSteps[1];

    pathSteps[0] = (char *)"resourceCache";
    

    File *resourceDir = new File( new Path( pathSteps, 1, false ),
                                  (char*)inResourceType );
    
    return resourceDir;
    }




File *getResourceFile( const char *inResourceType, uniqueID inID ) {
    
    char *idString = getHumanReadableString( inID );

    char *pathSteps[2];

    pathSteps[0] = (char *)"resourceCache";
    pathSteps[1] = (char *)inResourceType;


    File *resourceFile = new File( new Path( pathSteps, 2, false ),
                                   idString );
    
    delete [] idString;

    return resourceFile;
    }



static void addRequestMessage( SimpleVector<unsigned char> *inMessageAccum,
                               const char *inResourceType,
                               uniqueID inID ) {

    int typeLength = strlen( inResourceType );
    inMessageAccum->push_back( (unsigned char)typeLength );
    inMessageAccum->push_back( (unsigned char *)inResourceType, typeLength );
    
    inMessageAccum->push_back( inID.bytes, U );
    }




unsigned char *loadResourceData( const char *inResourceType,
                                 uniqueID inID,
                                 int *outLength,
                                 char *outCameFromNetwork ) {
    
    File *resourceFile = getResourceFile( inResourceType, inID );
    
    if( resourceFile->exists() ) {
        
        *outLength = resourceFile->getLength();

        char *fullFileName = resourceFile->getFullFileName();
        
        delete resourceFile;
        

        FILE *f = fopen( fullFileName, "rb" );
        
        delete [] fullFileName;
        
        unsigned char *data = new unsigned char[ *outLength ];
        
        int numRead = fread( data, 1, *outLength, f );
        
        fclose( f );

        *outCameFromNetwork = false;

        if( numRead != *outLength ) {
            delete [] data;
            AppLog::getLog()->logPrintf( 
                Log::ERROR_LEVEL,
                "Failed to read from %s file\n", inResourceType );
            return NULL;
            }
                
        
        return data;
        }
    else if( useNetwork && 
             connection != NULL && connection->isConnected() ) {

        delete resourceFile;

        // try to fetch from network
        
        SimpleVector<unsigned char> messageAccum;
        
        // single request in this bundle
        messageAccum.push_back( 1 );

        addRequestMessage( &messageAccum, inResourceType, inID );
                
        unsigned char *message = messageAccum.getElementArray();
        
        // resource requests on channel 1
        connection->sendMessage( message, messageAccum.size(), 1 );

        delete [] message;

        
        // now wait for response

        int length;
        message = NULL;
        
        unsigned long sec, ms;
        
        Time::getCurrentTime( &sec, &ms );

        char *idString = getHumanReadableString( inID );
        AppLog::getLog()->logPrintf( 
            Log::DETAIL_LEVEL,
            "Waiting for %s resource %s from network (%lu:%lu)...\n", 
            inResourceType, idString, sec, ms );
        delete [] idString;

        while( message == NULL ) {
            // keep stepping until network operations are finished
            char workLeft = true;
            while( workLeft ) {
                workLeft = connection->step();
                }

            message = connection->receiveMessage( &length, 1 );

            if( message == NULL ) {
                // wait
                AppLog::trace( "sleeping\n" );
                Thread::staticSleep( 50 );
                }
            }

        Time::getCurrentTime( &sec, &ms );
        AppLog::getLog()->logPrintf( 
            Log::DETAIL_LEVEL,
            "...got response (%lu:%lu)\n", sec, ms );

        // Got message
        
        *outCameFromNetwork = true;
        
        // first 4 bytes represent length of chunk, skip them (only one chunk)
        *outLength = length - 4;

        unsigned char *returnVal = new unsigned char[ length - 4 ];
        memcpy( returnVal, &( message[4] ), length - 4 );
        
        delete [] message;
        
        return returnVal;
        }
    else {
        delete resourceFile;
        
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Failed to open %s file (does not exist)\n", inResourceType );

        // fix:
        // why were we trying to open it anyway?  Remove it from 
        // database for future
        removeData( (char*)inResourceType, inID );

        // failure
        return NULL;
        }
    
    }



unsigned char **loadResourceData( const char *inResourceType,
                                  int inNumResources,
                                  uniqueID *inIDs,
                                  int *outLengths,
                                  char *outCameFromNetwork ) {
    
    unsigned char **results = new unsigned char *[ inNumResources ];
    
    int numToFetch = 0;

    // mark duplicates so that we can fill them in later w/out sending 
    // duplicate requests in our batch
    int *duplicate = new int[ inNumResources ];
    

    for( int i=0; i<inNumResources; i++ ) {
        
        File *resourceFile = getResourceFile( inResourceType, inIDs[i] );
        
        if( resourceFile->exists() ) {
            
            // simply fetch from disk using single-function
            results[i] = loadResourceData( inResourceType,
                                           inIDs[i],
                                           &( outLengths[i] ),
                                           &( outCameFromNetwork[i] ) );
            duplicate[i] = -1;
            }
        else {
            // flag and deal with in batch below

            // but only flag it if it is the *first* occurrence of this
            // ID in the batch request (don't fetch same resource
            // multiple times in same batch)
            duplicate[i] = -1;
            
            int alreadyListed = false;
            for( int j=0; j<i && !alreadyListed; j++ ) {
                if( equal( inIDs[j], inIDs[i] ) ) {
                    alreadyListed = true;
                    duplicate[i] = j;
                    }
                }
            results[i] = NULL;
            outLengths[i] = -1;

            if( !alreadyListed ) {
                duplicate[i] = false;
                outCameFromNetwork[i] = true;
                numToFetch ++;
                }
            else {
                // fill this in later
                outCameFromNetwork[i] = false;
                }
            }
        delete resourceFile;
        }

    if( numToFetch == 0 ) {
        // all on disk
        delete [] duplicate;
        return results;
        }
    
    
    SimpleVector<unsigned char> messageAccum;
    
    if( numToFetch > 255 ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL,
            "Error:  too many resources fetched from network in one"
            " batch: %d\n", numToFetch );
        // return null results
        delete [] duplicate;
        return results;
        }

    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Sending a batch request for %d [%s] resources\n", numToFetch,
        inResourceType );
    
    
    // first, send number in batch
    messageAccum.push_back( (unsigned char)numToFetch );
    
    for( int i=0; i<inNumResources; i++ ) {
        if( outCameFromNetwork[i] ) {        
            addRequestMessage( &messageAccum, inResourceType, inIDs[i] );
            }
        }
    
    unsigned char *message = messageAccum.getElementArray();
    
    // resource requests on channel 1
    connection->sendMessage( message, messageAccum.size(), 1 );
    
    delete [] message;


    // now wait for response
    int length;
    message = NULL;
        
    unsigned long sec, ms;
        
    Time::getCurrentTime( &sec, &ms );

    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Waiting for batch of %s resources from network (%lu:%lu)...\n", 
        inResourceType, sec, ms );
        
    while( message == NULL ) {
        // keep stepping until network operations are finished
        char workLeft = true;
        while( workLeft ) {
            workLeft = connection->step();
            }

        message = connection->receiveMessage( &length, 1 );

        if( message == NULL ) {
            // wait
            AppLog::trace( "sleeping\n" );
            Thread::staticSleep( 50 );
            }
        }

    Time::getCurrentTime( &sec, &ms );
    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "...got response (%lu:%lu)\n", sec, ms );

    // Got message

    unsigned char *messageLeft = message;
    int lengthLeft = length;

    // split into parts
    for( int i=0; i<inNumResources; i++ ) {
        if( outCameFromNetwork[i] ) {

            int numUsed;
            
            int chunkLength = readInt( messageLeft, lengthLeft, &numUsed );
            
            if( numUsed == -1 ) {
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "Error:Failed to read resource chunk from message" );
                delete [] message;
                delete [] duplicate;
                return results;
                }
            
            messageLeft = &( messageLeft[ numUsed ] );
            lengthLeft -= numUsed;
            

            outLengths[i] = chunkLength;
            
            if( lengthLeft < chunkLength ) {
                AppLog::getLog()->logPrintf( 
                    Log::ERROR_LEVEL,
                    "Error:Failed to read resource chunk "
                    "from messag\n" );
                delete [] message;
                delete [] duplicate;
                return results;
                }
            
            results[i] = new unsigned char[ chunkLength ];
            memcpy( results[i], messageLeft, chunkLength );
            
            messageLeft = &( messageLeft[ chunkLength ] );
            lengthLeft -= chunkLength;
            }
        }

    
    // fill in duplicates by simply copying fetched data
    for( int i=0; i<inNumResources; i++ ) {
        if( !outCameFromNetwork[i] && duplicate[i] != -1 ) {
            
            int j = duplicate[i];

            outLengths[i] = outLengths[j];
            results[i] = new unsigned char[ outLengths[i] ];
            memcpy( results[i], results[j], outLengths[i] );
            }    
        }
    
    
    delete [] message;
    

    delete [] duplicate;
    

    return results;
    }



void saveResourceData( const char *inResourceType,
                       uniqueID inID,
                       char *inWordString,
                       unsigned char *inData, int inLength ) {

    File *resourceDir = getResourceDir( inResourceType );
    
    if( !resourceDir->exists() ) {
        AppLog::warning( "No appropriate resourceCache subdirectory "
                         "directory found, creating it" );
        resourceDir->makeDirectory();
        }
    
    if( resourceDir->exists() && resourceDir->isDirectory() ) {
        // good
        }
    else {
        AppLog::criticalError( 
            "ERROR: necessary resourceCache subdirectory not found, and "
            "could not create it" );
        delete resourceDir;
        return;
        }
    delete resourceDir;


    File *resourceFile = getResourceFile( inResourceType, inID );
        
    
    if( resourceFile->exists() ) {
        AppLog::info( "Resource collides with same ID already on disk\n" );
        
        // but just because data file exists doesn't mean it has the same
        // contents (rare chance of a hash collision... we want to overwrite
        // the old data, not ignore the new data)

        AppLog::detail( "  Removing old search data from database\n" );
        removeData( (char *)inResourceType, inID );

        AppLog::detail( 
            "  Adding replacement resource data into that ID slot\n" );
        }
    

    char *fullFileName = resourceFile->getFullFileName();
        
    FILE *f = fopen( fullFileName, "wb" );
        
    delete [] fullFileName;
    
    
    int numWritten = fwrite( inData, 1, inLength, f );
        
    fclose( f );

    if( numWritten != inLength ) {
        AppLog::getLog()->logPrintf( 
            Log::ERROR_LEVEL, "Failed to write to %s file\n", inResourceType );
        }
    
    delete resourceFile;


    // add to the search database
    addData( (char*)inResourceType, inID, inWordString );
    }



void deleteResource( const char *inResourceType,
                     uniqueID inID ) {
    File *resourceFile = getResourceFile( inResourceType, inID );

    resourceFile->remove();
    
    delete resourceFile;

    removeData( (char*)inResourceType, inID );
    }




char resourceExists( const char *inResourceType,
                     uniqueID inID ) {

    File *resourceFile = getResourceFile( inResourceType, inID );
    
    char exists = resourceFile->exists();
    
    delete resourceFile;
    
    return exists;
    }
