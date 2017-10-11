#ifndef MUSIC_INCLUDED
#define MUSIC_INCLUDED


#include "color.h"
#include "common.h"
#include "uniqueID.h"
#include "Sprite.h"
#include "musicPlayer.h"





class Music {
    public:

        // blank music
        Music();
        

        // music loaded from resource manager
        Music( uniqueID inID );
        
        // music loaded from data string
        Music( uniqueID inID, unsigned char *inData, int inLength );
        

        // changes the on/off status of a music note
        void editMusic( int inX, int inY, char inNoteOn );
        
        
        char getNoteOn( int inX, int inY );
        
        
        // name has at most 10 chars
        void editMusicName( const char *inName );


        // destroyed by caller
        char *getMusicName();
        

        // finishes the edit, generates a new unique ID, saves result
        void finishEdit( char inGenerateNewID=true );

        // recursively saves to current resource pack
        void saveToPack();


        // get an image of this music
        Image *getImage();


        // implements ResourceType functions as needed by ResourcePicker
        uniqueID getUniqueID();
        Sprite *getSprite( char inUseTrans=false, char inCacheOK=true );
        static const char *getResourceType();
        static Music getDefaultResource();
        
        char *getName() {
            return getMusicName();
            }
        
        
        void print();
        

                // blank sprite
        static Music *sBlankMusic;

        static void staticInit();
        static void staticFree();

    protected:
        
        void setupDefault();
        

        char initFromData( unsigned char *inData, int inLength );
        

        // result destroyed by caller
        unsigned char *makeBytes( int *outLength );


        void makeUniqueID();

        
        // notes on or off, indexed as [y][x]
        char mNotes[N][N];
        
        const static int mNoteDataLength = N * N;
        
        char mName[11];

        
        uniqueID mID;
        
        
    };


#endif
