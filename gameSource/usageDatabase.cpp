// database tracking which resources use which other resources 

#include "usageDatabase.h"
#include "resourceDatabase.h"
#include "common.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/SimpleVector.h"

#include "Scene.h"
#include "StateObject.h"
#include "SpriteResource.h"
#include "Room.h"
#include "Tile.h"
#include "Song.h"
#include "Scale.h"
#include "Music.h"
#include "TimbreResource.h"



// for platform-independent ptr -> unsigned int casting
#include <stdint.h>


typedef struct usageRecord {
        uniqueID id;
        
        SimpleVector<uniqueID> usedByList;
        SimpleVector<uniqueID> usesList;
    } usageRecord;




#define B 2000

class UsageRecordHashTable {
    public:
        ~UsageRecordHashTable();
        
        void insert( usageRecord *inRecord );
        
        // does not destroy record
        void remove( usageRecord *inRecord );
        
        usageRecord *lookup( uniqueID inID );

    protected:
        
        SimpleVector<usageRecord *> mBins[B];
        
    };


UsageRecordHashTable::~UsageRecordHashTable() {
    for( int b=0; b<B; b++ ) {
        SimpleVector<usageRecord *> *v = &( mBins[b] );
        
        int numHits = v->size();
        
        for( int i=0; i<numHits; i++ ) {
            delete *( v->getElementFast( i ) );
            }
        }
    }


// use first 4 ID bytes as an int
#define getHashBin( id )              \
    (uintptr_t)( id.bytes[0] |        \
                 id.bytes[1] << 8 |   \
                 id.bytes[2] << 16 |  \
                 id.bytes[3] << 24 ) % B
        
        
void UsageRecordHashTable::insert( usageRecord *inRecord ) {
    
    int b = getHashBin( inRecord->id );

    mBins[b].push_back( inRecord );
    }



void UsageRecordHashTable::remove( usageRecord *inRecord ) {
    int b = getHashBin( inRecord->id );
    
    mBins[b].deleteElementEqualTo( inRecord );
    }


        
usageRecord *UsageRecordHashTable::lookup( uniqueID inID ) {
    int b = getHashBin( inID );
    
    SimpleVector<usageRecord *> *v = &( mBins[b] );
    
    int numHits = v->size();
    
    for( int i=0; i<numHits; i++ ) {
        usageRecord *record = *( v->getElementFast( i ) );

        if( equal( record->id, inID ) ) {
            return record;
            }
        }

    return NULL;
    }



static usageRecord *addUsage( uniqueID inID,
                              uniqueID inUsesID, usageRecord *inIDRecord );



static UsageRecordHashTable *table;


void initUsageDatabase() {
    table = new UsageRecordHashTable();


    printf( "Building usage database\n" );
    int addUsageCalls = 0;
    
    // scenes use objects and rooms

    int numScenes = countSearchResults( "scene", "" );
    uniqueID *scenes = new uniqueID[ numScenes ];    

    int numGotten = getSearchResults(  "scene", "", 0, numScenes, scenes );

    numScenes = numGotten;

    for( int i=0; i<numScenes; i++ ) {
        Scene s( scenes[i] );

        // fetch once and use over and over
        usageRecord *sceneRecord = NULL;
        
        sceneRecord = addUsage( scenes[i], s.mRoom, sceneRecord );
        addUsageCalls++;
        

        int numObjects = s.mObjects.size();
        
        
        for( int j=0; j<numObjects; j++ ) {
            addUsage( scenes[i], *( s.mObjects.getElementFast( j ) ),
                      sceneRecord );
            addUsageCalls++;
            }
        }
    delete [] scenes;


    // objects use sprites
    int numObjects = countSearchResults( "object", "" );
    uniqueID *objects = new uniqueID[ numObjects ];    

    numGotten = getSearchResults(  "object", "", 0, numObjects, objects );

    numObjects = numGotten;

    for( int i=0; i<numObjects; i++ ) {
        StateObject o( objects[i] );

        int numSprites = o.getNumLayers();

        // fetch once and use over and over
        usageRecord *objectRecord = NULL;

        
        for( int j=0; j<numSprites; j++ ) {
            
            objectRecord = addUsage( objects[i], o.getLayerSprite( j ),
                                     objectRecord );
            addUsageCalls++;
            }
        }
    delete [] objects;



    // rooms use tiles
    int numRooms = countSearchResults( "room", "" );
    uniqueID *rooms = new uniqueID[ numRooms ];    

    numGotten = getSearchResults(  "room", "", 0, numRooms, rooms );

    numRooms = numGotten;

    for( int i=0; i<numRooms; i++ ) {
        Room r( rooms[i] );


        // fetch once and use over and over
        usageRecord *roomRecord = NULL;

        for( int y=0; y<G; y++ ) {
            for( int x=0; x<G; x++ ) {
            
                roomRecord = addUsage( rooms[i], r.getTile( x, y ),
                                       roomRecord );
                addUsageCalls++;
                }
            }
        }
    delete [] rooms;



    // songs use timbres, music phrases, and scales

    int numSongs = countSearchResults( "song", "" );
    uniqueID *songs = new uniqueID[ numSongs ];    

    numGotten = getSearchResults(  "song", "", 0, numSongs, songs );

    numSongs = numGotten;

    for( int i=0; i<numSongs; i++ ) {
        Song s( songs[i] );
        
        
        // fetch once and use over and over
        usageRecord *songRecord = NULL;

        songRecord = addUsage( songs[i], s.getScale(), songRecord );
        addUsageCalls++;

        for( int y=0; y<SI; y++ ) {
            
            addUsage( songs[i], s.getTimbre( y ), songRecord );
            addUsageCalls++;
        
            for( int x=0; x<S; x++ ) {
                addUsage( songs[i], s.getPhrase( x, y ), songRecord );
                addUsageCalls++;
                }
            }
        }
    delete [] songs;




    printf( "Done building usage database, %d usages added\n", addUsageCalls );
    }
    



void freeUsageDatabase() {
    delete table;
    }





// returns record associated with inID
// takes such a record as input (or NULL
static usageRecord *addUsage( uniqueID inID,
                              uniqueID inUsesID, usageRecord *inIDRecord ) {
    
    usageRecord *r = inIDRecord;
    
    if( r == NULL ) {
        r = table->lookup( inID );
        }
    
    if( r == NULL ) {
        //printf( "Adding new record\n" );
        r = new usageRecord;
        r->id = inID;
        
        table->insert( r );
        
        }
    r->usesList.push_back( inUsesID );
    // allow duplicates, because checking for them is too slow
    /*
    else {
        // record exists.  Make sure this usage not already tracked
        //printf( "Record exists\n" );
        
        int numUses = r->usesList.size();
        
        char found = false;
        for( int i=0; i<numUses && !found; i++ ) {
            if( equal( inUsesID, *( r->usesList.getElementFast( i ) ) ) ) {
                found = true;
                }
            }
        if( !found ) {
            // new addition
            r->usesList.push_back( inUsesID );
            }
        }
    */
    usageRecord *returnValue = r;
    
    
    r = table->lookup( inUsesID );
    
    if( r == NULL ) {
        r = new usageRecord;
        r->id = inUsesID;

        table->insert( r );
        }
    r->usedByList.push_back( inID );
    // allow duplicates, because checking for them is too slow
    /*
    else {
        // record exists.  Make sure this usage not already tracked
        
        int numUsedBy = r->usedByList.size();
        
        char found = false;
        for( int i=0; i<numUsedBy && !found; i++ ) {
            if( equal( inID, *( r->usedByList.getElementFast( i ) ) ) ) {
                found = true;
                }
            }
        if( !found ) {
            // new addition
            r->usedByList.push_back( inID );
            }
        }
    */
    return returnValue;
    }




void addUsage( uniqueID inID,
               uniqueID inUsesID ) {
    addUsage( inID, inUsesID, NULL );
    }




// remove all usages associated with inID
// note that because inID is a hash of the using object, removing one
// usage from that object means that the ID of the object will change
// Thus, we would never be removing a usage from inID unless we were deleting
// inID entirely, which means we'd want to remove ALL usages from inID
void removeUsages( uniqueID inID ) {
    // first, make sure it's not used by anything

    usageRecord *r = table->lookup( inID );
    
    if( r != NULL ) {

        if( r->usedByList.size() == 0 ) {

            // walk over all resources that inID uses
            int numUses = r->usesList.size();
            
            for( int i=0; i<numUses; i++ ) {
            
                uniqueID *usesID = r->usesList.getElementFast( i );
                

                // remove inID from each of these IDs' usedBy lists
                usageRecord *r2 = table->lookup( *usesID );
                
                if( r2 != NULL ) {
                    
                    // might contain duplicates, so check all
                    for( int j=0; j<r2->usedByList.size(); j++ ) {
                        
                        if( equal( 
                                inID, 
                                *( r2->usedByList.getElementFast( j ) ) ) ) {
                        
                            r2->usedByList.deleteElement( j );
                            j--;
                            }
                        
                        }
                    

                    }
                
                
                }
            
            table->remove( r );
            
            delete r;            
            }
        else {
            AppLog::error( "Error:  Trying to remove a resource that is "
                           "still used" );
            }
        }

    }




char isUsed( uniqueID inID ) {
    usageRecord *r = table->lookup( inID );
    
    if( r != NULL ) {

        if( r->usedByList.size() > 0 ) {
            return true;
            }
        }
    return false;
    }



uniqueID getUser( uniqueID inID ) {
    usageRecord *r = table->lookup( inID );
    
    if( r != NULL ) {

        if( r->usedByList.size() > 0 ) {
            
            return *( r->usedByList.getElement( 0 ) );
            }
        }
    return defaultID;
    }

    

    
