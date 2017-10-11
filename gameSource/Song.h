#ifndef SONG_INCLUDED
#define SONG_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"


// length of a song
#define S   9

// number of instruments (rows) in a song
#define SI  8




class Song {
    public:

        // empty song with default name
        Song();
        

        // song loaded from file
        Song( uniqueID inID );

        // song loaded from data string
        Song( uniqueID inID, unsigned char *inData, int inLength,
              char inFromNetwork, char inSaveWholeSong );
        
        


        // changes the tile in a given grid location
        void editSongPhrase( int inX, int inY, uniqueID inMusicID );
        
        void editRowLength( int inY, int inLength );
        
        void editTimbre( int inY, uniqueID inTimbreID );
        
        // 0-255
        void editRowLoudness( int inY, int inLoudness );

        // 0 = left, 127=center, 255=right
        void editRowStereo( int inY, int inStereo );
        
        void editScale( uniqueID inScale );
        
        // 0 slow, 1 normal, 2 fast
        void editSpeed( int inSpeed );
        

        // name has at most 10 chars
        void editSongName( const char *inName );
        
        // gets ID of tile on grid
        uniqueID getPhrase( int inX, int inY );
        
        int  getRowLength( int inY );
        
        uniqueID getTimbre( int inY );

        int getRowLoudness( int inY );
        
        int getRowStereo( int inY );
        

        uniqueID getScale();

        int getSpeed();
        

        char *getSongName();
        


        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );
        

        // recursively saves to current resource pack
        void saveToPack();


        // image version of getSprite, never fetched from cache
        Image *getImage();
        

        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Song getDefaultResource();
        
        char *getName() {
            return getSongName();
            }


        // blank song
        static Song *sBlankSong;
        
        static void staticInit();
        static void staticFree();
        
        
        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );



        // sets a new song into music player, only updating parts that have
        // changed (for efficiency)
        static void setInPlayer( Song inOldSong, Song inNewSong,
                                 char inForceUpdate = false );
        


    protected:
        void setupDefault();
        
        void initFromData( unsigned char *inData, int inLength, 
                           char inFromNetwork, char inSaveWholeSong );


        
        void readFromBytes( unsigned char *inBytes, int inLength );
        

        void makeUniqueID();

        
        // all phrases are exactly one grid square in size
        uniqueID mPhrases[SI][S];
        
        unsigned char mRowLengths[SI];

        uniqueID mTimbres[SI];
        
        unsigned char mRowLoudness[SI];
        
        unsigned char mRowStereo[SI];
        

        uniqueID mScale;
        
        unsigned char mSpeed;
        


        char mName[11];
                
        uniqueID mID;
        
    };



#endif
