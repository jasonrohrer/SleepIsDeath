
#include "packSaver.h"
#include "common.h"
#include "resourceManager.h"
#include "fixOldResources.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/io/file/File.h"


#include <zlib.h>



typedef struct packRecord {
        const char *type;
        uniqueID id;
        char *wordString;
        unsigned char *data;
        int length;
    } packRecord;
    
SimpleVector<packRecord> runningPack;


void initPackSaver() {
    }


void freePackSaver() {
    for( int i=0; i<runningPack.size(); i++ ) {
        packRecord *r = runningPack.getElement( i );
        
        delete [] r->wordString;
        delete [] r->data;
        }
    runningPack.deleteAll();
    }



char alreadyInPack( uniqueID inID ) {
    // already present?
    for( int i=0; i<runningPack.size(); i++ ) {
        packRecord *r = runningPack.getElement( i );

        if( equal( r->id, inID ) ) {
            return true;
            }
        }
    return false;
    }



void addToPack( const char *inResourceType,
                uniqueID inID,
                char *inWordString,
                unsigned char *inData, int inLength ) {
    
    if( alreadyInPack( inID ) ) {
        return;
        }
        
    
    // not found
    // add

    unsigned char *data = new unsigned char[ inLength ];
    memcpy( data, inData, inLength );
    
    packRecord r = {
        inResourceType,
        inID,
        stringDuplicate( inWordString ),
        data, 
        inLength };
    
    runningPack.push_back( r );
    }



// saves the pack and clears the running pack
void savePack() {
    
    
    File packDir( NULL, "resourcePacks" );
    
    if( !packDir.exists() ) {
        packDir.makeDirectory();
        }
    
    if( packDir.exists() && packDir.isDirectory() ) {
        
        int numFiles;
        File **childFiles = packDir.getChildFiles( &numFiles );
        
        int largestNumber = 0;
        
        for( int i=0; i<numFiles; i++ ) {
            
            
            char *name = childFiles[i]->getFileName();
                
            int number;
            
            int numRead = sscanf( name, "%d", &number );
            
            if( numRead == 1 ) {
                
                if( number > largestNumber ) {
                    largestNumber = number;
                    }
                }
            delete [] name;
            delete childFiles[i];
            }
        
        delete [] childFiles;


        int newFileNumber = largestNumber + 1;

        
        char *fileName = autoSprintf( "%d.pak", newFileNumber );
        
        File *packFile = packDir.getChildFile( fileName );
        
        delete [] fileName;

        char *fullFileName = packFile->getFullFileName();
        



        SimpleVector<unsigned char> dataAccum;


        // first, output number of chunks
        unsigned char *countChars = getChars( runningPack.size() );

        dataAccum.push_back( countChars, 4 );
        
        for( int i=0; i<runningPack.size(); i++ ) {
            packRecord *r = runningPack.getElement( i );
            dataAccum.push_back( (unsigned char *)( r->type ), 
                                 strlen( r->type ) + 1 );
            dataAccum.push_back( r->id.bytes, U );
            dataAccum.push_back( (unsigned char *)( r->wordString ), 
                                 strlen( r->wordString ) + 1 );
            
            unsigned char *lengthChars = getChars( r->length );
            
            dataAccum.push_back( lengthChars, 4 );
            dataAccum.push_back( r->data, r->length );
            }  
        
        unsigned char *data = dataAccum.getElementArray();
        int numBytes = dataAccum.size();

        uLongf compressSize = compressBound( numBytes );

        unsigned char *compressData = new unsigned char[ compressSize + 4 ];
        
        // first 4 bytes are uncompressed size
        unsigned char *sizeChars = getChars( numBytes );
        memcpy( compressData, sizeChars, 4 );
        
        int result = compress2( &( compressData[4] ),   &compressSize,
                                data, numBytes,
                                Z_BEST_COMPRESSION );
        
        if( result == Z_OK ) {
            char result = packFile->writeToFile( compressData, 
                                                 compressSize + 4 );
            
            if( !result ) {
                AppLog::error( "Failed to write to pack file:" );
                AppLog::error( fullFileName );
                }
            }
        else {
            AppLog::error( "Zlib compression failed\n" );
            }
        delete [] data;
        delete [] compressData;
        
        delete packFile;
        delete [] fullFileName;

        }




    for( int i=0; i<runningPack.size(); i++ ) {
        packRecord *r = runningPack.getElement( i );
        
        delete [] r->wordString;
        delete [] r->data;
        }
    runningPack.deleteAll();
    }



void loadPacks() {
    
    File loadDir( NULL, "loadingBay" );
    
    if( !loadDir.exists() ) {
        loadDir.makeDirectory();
        }
    
    if( loadDir.exists() && loadDir.isDirectory() ) {
        
        int numFiles;
        File **childFiles = loadDir.getChildFiles( &numFiles );
        
        for( int i=0; i<numFiles; i++ ) {
            
            
            char *name = childFiles[i]->getFullFileName();
                
            if( strstr( name, ".pak" ) != NULL ) {
                // a pack file
                
                char *shortName = childFiles[i]->getFileName();
                char found;
                char *doneFlagName =
                    replaceOnce( shortName, ".pak",
                                 ".done", &found );
                delete [] shortName;
                
                File *doneFlagFile = loadDir.getChildFile( doneFlagName );
                delete [] doneFlagName;
                
                if( !doneFlagFile->exists() ) {
                    


                int bytesLeft;
                unsigned char *bytes = NULL;
                

                int compressSize;
                
                unsigned char *compressData = 
                    childFiles[i]->readFileContents( &compressSize );
                
                if( compressData != NULL ) {
                    
                    // first 4 are uncompressed size
                    int numUsed;
                    
                    bytesLeft = 
                        readInt( compressData, compressSize, &numUsed );
                
                    if( numUsed > 0 ) {
                        bytes = new unsigned char[ bytesLeft ];
                        

                        uLongf destLen = bytesLeft;
                        
                        int result = uncompress( 
                            bytes, &destLen,
                            &( compressData[numUsed] ), 
                            compressSize - numUsed );
                        
                        if( result != Z_OK || 
                            destLen != (unsigned int)bytesLeft ) {
                            
                            AppLog::error( "Zlib decompression failed" );
                            
                            delete [] bytes;
                            bytes = NULL;
                            }
                        }
                    delete [] compressData;
                    }
                


                unsigned char *allBytes = bytes;
                

                if( bytes != NULL ) {
                    
                    SimpleVector<uniqueID> importedScenes;
                    SimpleVector<uniqueID> importedObjects;
                    


                    int numUsed;
                    
                    int numChunks = readInt( bytes, bytesLeft, &numUsed );
                    
                    if( numUsed > 0 ) {
                        bytesLeft -= numUsed;
                        bytes = &( bytes[numUsed] );
                        
                        char chunkSuccess = true;
                        
                        for( int c=0; c<numChunks && chunkSuccess; c++ ) {
                            chunkSuccess = false;
                            
                            char *type = stringDuplicate( (char*)bytes );
                            
                            numUsed = strlen( type ) + 1;
                            
                            bytesLeft -= numUsed;
                            bytes = &( bytes[numUsed] );
                        
                            uniqueID id =
                                readUniqueID( bytes, bytesLeft, &numUsed );

                            if( numUsed == U ) {
                                bytesLeft -= numUsed;
                                bytes = &( bytes[numUsed] );
                                
                                char *wordString = 
                                    stringDuplicate( (char*)bytes );
                            
                                numUsed = strlen( wordString ) + 1;
                            
                                bytesLeft -= numUsed;
                                bytes = &( bytes[numUsed] );
                                
                                int dataLength = readInt( bytes, bytesLeft, 
                                                          &numUsed );
                                
                                if( numUsed > 0 ) {
                                    bytesLeft -= numUsed;
                                    bytes = &( bytes[numUsed] );
                                    

                                    if( bytesLeft >= dataLength ) {
                                        
                                        saveResourceData( type,
                                                          id,
                                                          wordString,
                                                          bytes, 
                                                          dataLength );

                                        if( strcmp( type, "object" ) == 0 ) {
                                            importedObjects.push_back( id );
                                            }
                                        else if( 
                                            strcmp( type, "scene" ) == 0 ) {
                                            
                                            importedScenes.push_back( id );
                                            }


                                        AppLog::trace( "Saved pack chunk" );
                                        
                                        chunkSuccess = true;
                                        
                                        bytesLeft -= dataLength;
                                        bytes = &( bytes[dataLength] );
                                        }
                                    }
                                delete [] wordString;
                                }
                            delete [] type;
                            }
                        if( chunkSuccess ) {
                            AppLog::info( "Successfully loaded all chunks"
                                          " from pack file." );
                            }
                        else {
                            AppLog::error( "Failed to load a chunk"
                                           " from pack file." );
                            } 

                        // create a flag file
                        doneFlagFile->writeToFile( "done loading" );


                        AppLog::info( "Running fix script on imported"
                                      " resources from pack." );
                        
                        uniqueID *sceneArray = 
                            importedScenes.getElementArray();
                        uniqueID *objectArray = 
                            importedObjects.getElementArray();

                        fixOldResources( sceneArray, importedScenes.size(),
                                         objectArray, importedObjects.size() );
                        
                        delete [] sceneArray;
                        delete [] objectArray;

                        AppLog::info( "Done running fix script." );
                        }
                    delete [] allBytes;
                    }
                else {
                    AppLog::error( "Failed to read pack file:" );
                    AppLog::error( name );
                    }
                    }  // end check for done file existing
                else {
                    AppLog::info( "Done flag file present for pak file:" );
                    AppLog::info( name );
                    }
                
                delete doneFlagFile;
                }
            
            delete [] name;
            delete childFiles[i];
            }
        
        delete [] childFiles;
        }
    
    }

