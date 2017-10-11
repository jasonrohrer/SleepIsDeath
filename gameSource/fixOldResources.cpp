

#include "fixOldResources.h"
#include "resourceDatabase.h"
#include "resourceManager.h"
#include "Scene.h"
#include "StateObject.h"

SimpleVector< uniqueID > objectsToDelete;
SimpleVector< uniqueID > scenesToDelete;



void fixOldResources( uniqueID *inScenes, int inNumScenes,
                      uniqueID *inObjects, int inNumObjects ) {
    
    int numScenes;
    uniqueID *scenes;
    

    if( inScenes != NULL ) {
        numScenes = inNumScenes;
        scenes = inScenes;
        }
    else {
        
        numScenes = countSearchResults( "scene", "" );
        scenes = new uniqueID[ numScenes ];    

        int numGotten = getSearchResults(  "scene", "", 0, numScenes, scenes );

        numScenes = numGotten;
        }
    
    printf( "processing %d scenes\n", numScenes );
    

    for( int i=0; i<numScenes; i++ ) {
    
        
        Scene s( scenes[i] );
        int numObjects = s.mObjects.size();
        
        printf( "Processing scene %d (%d objects)\n", i, numObjects );
        

        
        for( int j=0; j<numObjects; j++ ) {
            uniqueID oldID = *( s.mObjects.getElement( j ) );
            

            StateObject o( oldID );
        
            o.finishEdit();
            

            uniqueID newID = o.getUniqueID();
            
            if( !equal( newID, oldID ) ) {
                printf( "  Object %d outdated\n", j );

                objectsToDelete.push_back( oldID );
                
                // replace in scene
                *( s.mObjects.getElement( j ) ) = newID;
                }
            }
        
        
        uniqueID oldID = scenes[i];
        
        s.finishEdit();
        
        uniqueID newID = s.getUniqueID();

        if( !equal( newID, oldID ) ) {
            printf( "  Scene outdated\n" );
            
            scenesToDelete.push_back( oldID );
            }
        }

    if( inScenes == NULL ) {
        // we created this
        delete [] scenes;
        }
    
    

    printf( "\n\n" );
    

    int numObjects;
    uniqueID *objects;
    

    if( inObjects != NULL ) {
        numObjects = inNumObjects;
        objects = inObjects;
        }
    else {
        
        numObjects = countSearchResults( "object", "" );
        objects = new uniqueID[ numObjects ];    

        int numGotten = getSearchResults( 
            "object", "", 0, numObjects, objects );

        numObjects = numGotten;
        }


    printf( "processing %d objects\n", numObjects );
    

    for( int i=0; i<numObjects; i++ ) {
        
        uniqueID oldID = objects[i];

        StateObject o( oldID );
        
        o.finishEdit();
            

        uniqueID newID = o.getUniqueID();
        
        if( !equal( newID, oldID ) ) {
            printf( "Object %d outdated\n", i );

            objectsToDelete.push_back( oldID );
            }
        }

    if( inObjects == NULL ) {
        // we made this
        delete [] objects;
        }
    

    for( int i=0; i<objectsToDelete.size(); i++ ) {
        deleteResource( "object", *( objectsToDelete.getElement( i ) ) );
        }
    objectsToDelete.deleteAll();
    

    for( int i=0; i<scenesToDelete.size(); i++ ) {
        deleteResource( "scene", *( scenesToDelete.getElement( i ) ) );
        }
    scenesToDelete.deleteAll();
    
    
    }
