#include "imageCache.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileInputStream.h"
#include "minorGems/io/file/FileOutputStream.h"



typedef struct imageCacheRecord {
        uniqueID id;
        char useTrans;
        Image *image;
    } imageCacheRecord;


static SimpleVector<imageCacheRecord> cacheRecords;

static const char *cacheDirName = "imageCache";



void initImageCache() {
    File cacheDir( NULL, cacheDirName );
    
    if( !cacheDir.exists() ) {
        cacheDir.makeDirectory();
        }
    
    if( ! cacheDir.exists() ||  ! cacheDir.isDirectory() ) {
        AppLog::error( "Could not create an image cache directory" );
        }
    }



void freeImageCache() {
    int numRecords = cacheRecords.size();
    for( int i=0; i<numRecords; i++ ) {
        delete cacheRecords.getElement(i)->image;
        }
    cacheRecords.deleteAll();
    }



static File *getCacheFile( uniqueID inID, char inUseTrans ) {
    char *idString = getHumanReadableString( inID );
    
    int transFlag = 0;
    if( inUseTrans ) {
        transFlag = 1;
        }
    
    char *name = autoSprintf( "%s_%d", idString, inUseTrans );
    
    delete [] idString;
    
    char *pathSteps[1];

    pathSteps[0] = (char *)"imageCache";

    File *cacheFile = new File( new Path( pathSteps, 1, false ),
                                   name );
    
    delete [] name;

    return cacheFile;
    }



void addCachedImage( uniqueID inID, char inUseTrans, Image *inImage ) {
    imageCacheRecord r = { inID, inUseTrans, inImage };
    cacheRecords.push_back( r );

    // limit size
    if( cacheRecords.size() > 63 ) {
        // drop oldest
        delete cacheRecords.getElement(0)->image;
        cacheRecords.deleteElement( 0 );
        //printf( "Deleting excess record from image cache.\n" );
        }

    File *cacheFile = getCacheFile( inID, inUseTrans );
    
    if( ! cacheFile->exists() ) {
        
        FileOutputStream out( cacheFile );
    
        inImage->serialize( &out );

        char *error = out.getLastError();
        if( error != NULL ) {
            AppLog::error( "Failed to write image cache file" );
            AppLog::error( error );
            delete [] error;
            }            
        }
    delete cacheFile;
    }




// returns NULL if image not found in cache
Image *getCachedImage( uniqueID inID, char inUseTrans ) {
    int numRecords = cacheRecords.size();
    for( int i=numRecords-1; i>=0; i-- ) {
        imageCacheRecord r = *( cacheRecords.getElement(i) );

        if( equal( r.id, inID ) && inUseTrans == r.useTrans ) {
            // found

            // move to head
            cacheRecords.deleteElement( i );
            cacheRecords.push_back( r );
            
            return r.image;
            }
        }


    // not found in memory?

    // try disk
    File *cacheFile = getCacheFile( inID, inUseTrans );

    if( cacheFile->exists() ) {
        
        FileInputStream in( cacheFile );
    
        Image *image = new Image( 1, 1, 4, false );
        
        image->deserialize( &in );
        
        char *error = in.getLastError();
        if( error != NULL ) {
            AppLog::error( "Failed to read image cache file" );
            AppLog::error( error );
            delete [] error;
            }            
        else {
            // add it
            addCachedImage( inID, inUseTrans, image );
            
            delete cacheFile;

            return image;
            }
        }
    
    delete cacheFile;


    return NULL;
    }



void clearCachedImages( uniqueID inID ) {
    int numRecords = cacheRecords.size();
    for( int i=numRecords-1; i>=0; i-- ) {
        imageCacheRecord r = *( cacheRecords.getElement(i) );

        if( equal( r.id, inID ) ) {
            // found


            // delete and keep looking
            delete cacheRecords.getElement(i)->image;
            cacheRecords.deleteElement( i );
            }
        }

    // delete both
    File *cacheFile = getCacheFile( inID, 0 );

    if( cacheFile->exists() ) {
        cacheFile->remove();
        }
    delete cacheFile;
    
    cacheFile = getCacheFile( inID, 1 );

    if( cacheFile->exists() ) {
        cacheFile->remove();
        }
    delete cacheFile;
    }


