
#include "resourceImporter.h"


#include "resourceManager.h"
#include "resourceDatabase.h"
#include "Connection.h"
#include "common.h"
#include "fixOldResources.h"

#include <stdio.h>



#include "minorGems/io/file/File.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/log/AppLog.h"



// save IDs of stuff we import so we can fix it later
SimpleVector<uniqueID> importedScenes;
SimpleVector<uniqueID> importedObjects;


static void processImportDBLines( char **inLines, int inNumLines,
                                  File *inImportCacheDir ) {

    for( int i=0; i<inNumLines; i++ ) {
            
        char *line = inLines[i];

        int charsLeft = strlen( line );
        
        if( charsLeft < U * 2 ) {
            AppLog::getLog()->logPrintf( 
                Log::ERROR_LEVEL,
                "Failed to read unique ID from line %d of "
                "imported string DB\n", i );
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
                    "line %d of imported string DB\n", i );
                }
            else {
                    
                // skip ID and space
                char *substring = &( line[ U * 2 + 1 ] );
            
            
                char typeString[100];
                
                int numRead = sscanf( substring, "%99s", typeString );
                
                if( numRead == 1 ) {

                    // first, make sure resource does not already exist
                    if( !resourceExists( typeString, id ) ) {
                        
                        char *namePointer = &substring[ strlen( typeString ) 
                                                        + 1 ];
                    
                        File *resourceTypeDir = 
                            inImportCacheDir->getChildFile( typeString );
                    
                        if( resourceTypeDir->exists() && 
                            resourceTypeDir->isDirectory() ) {
                        
                        
                            File *resourceFile =
                                resourceTypeDir->getChildFile( idString );
                    
                            if( resourceFile->exists() ) {
                            
                                int dataLength;
                                unsigned char *fileData =
                                    resourceFile->readFileContents( 
                                        &dataLength );
                            
                                if( fileData != NULL ) {
                                
                                    saveResourceData( 
                                        typeString,
                                        id,
                                        namePointer,
                                        fileData, dataLength );
                                
                                    if( strcmp( typeString, "object" ) == 0 ) {
                                        importedObjects.push_back( id );
                                        }
                                    else if( 
                                        strcmp( typeString, "scene" ) == 0 ) {
                                        
                                        importedScenes.push_back( id );
                                        }

                                    delete [] fileData;
                                    }
                                else {
                                    AppLog::getLog()->logPrintf( 
                                        Log::ERROR_LEVEL,
                                        "Failed to read file data for "
                                        "line %d of imported string DB\n", i );
                                    }
                                }
                            else {
                                AppLog::getLog()->logPrintf( 
                                    Log::ERROR_LEVEL,
                                    "File does not exist for "
                                    "line %d of imported string DB\n", i );
                                }


                            delete resourceFile;
                            }
                        else {
                            AppLog::getLog()->logPrintf( 
                                Log::ERROR_LEVEL,
                                "Resource type directory not exist for "
                                "line %d of imported string DB\n", i );
                            }

                        delete resourceTypeDir;                    
                        }
                    }
                }
                
            
            delete [] idString;    
            }

        }
    }




void importResources() {
    
    File importDir( NULL, "importOldCache" );

    if( !importDir.exists() ) {
        AppLog::warning( "No importOldCache directory found, creating it" );
        importDir.makeDirectory();
        }
    else if( !importDir.isDirectory() ) {
        AppLog::error( "importOldCache is not a directory" );
        }
    else {
        File *cacheDir = importDir.getChildFile( "resourceCache" );
        
        if( cacheDir->exists() && cacheDir->isDirectory() ) {
            
            AppLog::info( "Found a resource cache directory to import" );


            File *flagFile =
                cacheDir->getChildFile( "alreadyImported.txt" );

            if( flagFile->exists() ) {
                AppLog::info( "Directory is flagged as already imported" );
                }
            else {
                AppLog::info( "Directory is NOT flagged as already imported" );
                
                File *dbFile = cacheDir->getChildFile( "stringDatabase.txt" );
            
            
                if( dbFile->exists() ) {
        
                    char *fileContents = dbFile->readFileContents();
        

                    if( fileContents == NULL ) {
                        AppLog::error( 
                            "Error:  "
                            "failed to import from stringDatabase.txt" );
                        }
                    else {
        
                        int numLines;
                        char **lines = split( fileContents, "\n", &numLines );
        
                        delete [] fileContents;

                        AppLog::getLog()->logPrintf( 
                            Log::INFO_LEVEL,
                            "Importing %d resources from importOldCache",
                            numLines );
                    
                        processImportDBLines( lines, numLines, cacheDir );
                    
                        for( int i=0; i<numLines; i++ ) {
                            delete [] lines[i];
                            }
                        delete [] lines;

                        
                        
                        AppLog::info( "Running fix script on imported"
                                      " resources from importOldCache." );
                        
                        uniqueID *sceneArray = 
                            importedScenes.getElementArray();
                        uniqueID *objectArray = 
                            importedObjects.getElementArray();

                        fixOldResources( sceneArray, importedScenes.size(),
                                         objectArray, importedObjects.size() );
                        
                        delete [] sceneArray;
                        delete [] objectArray;

                        importedScenes.deleteAll();
                        importedObjects.deleteAll();
                        
                        AppLog::info( "Done running fix script" );
                        
                        flagFile->writeToFile( "import complete" );
                        }
                    }
                delete dbFile;
                }
            delete flagFile;
            
            AppLog::info( "Done importing resource cache directory" );
            }
        delete cacheDir;
        }    
    }

