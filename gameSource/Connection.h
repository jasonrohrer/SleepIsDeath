
#include "minorGems/network/Socket.h"
#include "minorGems/network/SocketServer.h"
#include "minorGems/util/SimpleVector.h"



typedef struct message {
        unsigned char *data;
        int length;
        int nextToProcessIndex;
        int headerBytesProcessed;
        
        unsigned char channel;
        unsigned char bitCheckSum;
    } message;


class Connection {
  public:
    
    // start a server
    Connection();
    

    // make a connection to an established server
    Connection( char *inAddress );
    

    ~Connection();


    // get the port that connections use
    static int getPort();
        

    char isConnected();
    

    // perform another step of any pending network operations
    // returns true if work remains to be done
    // returns false if client is done with all pending work
    char step();
    

    
    void sendMessage( unsigned char *inData, int inLength, 
                      unsigned char inChannel=0 );
    

    unsigned char *receiveMessage( int *outLength, unsigned char inChannel=0 );
    

    char isError();
        
    void clearError();
    
    // destroyed internally
    char *getErrorString();

  protected:
    
    
    SocketServer *mServer;

    Socket *mSock;
    

    SimpleVector<message> mQueuedOutgoing;
    SimpleVector<message> mQueuedIncoming;
    
    // in progress, not queued
    message mNextOutgoingMessage;
    message mNextIncomingMessage;
    
    unsigned char mReceivedHeader[4];
    int mNumHeaderBytesReceived;
    
    

    char mError;
    char *mErrorString;
    
    void setError( const char *inErrorTranslationKey );

    };

