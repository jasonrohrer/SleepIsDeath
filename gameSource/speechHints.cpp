#include "speechHints.h"

#include "minorGems/io/file/File.h"
#include "minorGems/util/log/AppLog.h"


static File *hintDir = NULL;


void initSpeechHints() {
    File cacheDir( NULL, "resourceCache" );
    
    if( !cacheDir.exists() ) {
        AppLog::criticalError( 
            "No resourceCache directory found for speech hints" );
        }
    else {
         hintDir = cacheDir.getChildFile( "speechHints" );
        

        if( !hintDir->exists() ) {
            AppLog::warning( "No speechHints directory found, creating it" );
            hintDir->makeDirectory();
            }
        
        if( hintDir->exists() && hintDir->isDirectory() ) {
            // good
            }
        else {
            AppLog::criticalError( 
                "ERROR: speechHints not found, and could not create it" );

            delete hintDir;
            hintDir = NULL;
            }
        }
    }



void freeSpeechHints() {
    if( hintDir != NULL ) {
        delete hintDir;
        }
    }




void setSpeechHint( uniqueID inObjectID,
                    intPair inSpeechOffset, char inSpeechflip ) {
    
    if( hintDir != NULL ) {
        char *fileName = getHumanReadableString( inObjectID );
    
        File *hintFile = hintDir->getChildFile( fileName );
        
        SimpleVector<unsigned char> fileAccum;
        fileAccum.push_back( getChars( inSpeechOffset ), 8 );
        if( inSpeechflip ) {
            fileAccum.push_back( 1 );
            }
        else {
            fileAccum.push_back( 0 );
            }
        
        unsigned char *fileData = fileAccum.getElementArray();
        hintFile->writeToFile( fileData, fileAccum.size() );
        delete [] fileData;


        delete [] fileName;
        delete hintFile;
        }
    
    }


static intPair defaultHint = { P/2, P/2 };




intPair getSpeechHint( uniqueID inObjectID, char *outSpeechFlip ) {

    intPair returnVal = defaultHint;
    

    if( hintDir != NULL ) {
        char *fileName = getHumanReadableString( inObjectID );
        
        File *hintFile = hintDir->getChildFile( fileName );

        if( hintFile->exists() ) {
            
        
            int dataLength;
            
            unsigned char *data = 
                hintFile->readFileContents( &dataLength );
            
            if( data != NULL ) {
                if( dataLength == 9 ) {
                    int numUsed;
                    returnVal = readIntPair( data, dataLength, &numUsed );
                    
                    *outSpeechFlip = data[8];
                    }
                delete [] data;
                }
            }

        delete [] fileName;
        delete hintFile;
        }
     
    return returnVal;
    }



void clearSpeechHint( uniqueID inObjectID ) {
    
    if( hintDir != NULL ) {
        char *fileName = getHumanReadableString( inObjectID );
    
        File *hintFile = hintDir->getChildFile( fileName );
        
        hintFile->remove();

        delete [] fileName;
        delete hintFile;
        }
    
    }

