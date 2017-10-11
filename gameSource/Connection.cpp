#include "Connection.h"

#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/network/SocketClient.h"

#include "minorGems/util/log/AppLog.h"


extern int autoJoinPort;


int Connection::getPort() {

    if( autoJoinPort != -1 ) {
        return autoJoinPort;
        }
    
    char portFound;
    int port = SettingsManager::getIntSetting( "port", 
                                               &portFound );
    if( !portFound ) {
        port = 7778;
        }
    return port;
    }





Connection::Connection()
        : mSock( NULL ),
          mError( false ),
          mErrorString( NULL ) {
    
    message empty = {NULL, -1, 0, 0, 0};
    mNextOutgoingMessage = empty;
    mNextIncomingMessage = empty;
    
    
        
    mServer = new SocketServer( getPort(), 1 );
    
    }

    
    

// make a connection to an established server
Connection::Connection( char *inAddress )
        : mServer( NULL ),
          mError( false ),
          mErrorString( NULL ) {
    
    message empty = {NULL, -1, 0, 0, 0};
    mNextOutgoingMessage = empty;
    mNextIncomingMessage = empty;


    HostAddress h( stringDuplicate( inAddress ), getPort() );
    
    // non-blocking
    char timedOut;
    mSock = SocketClient::connectToServer( &h, 0, &timedOut );

    if( mSock == NULL ) {
        setError( "err_failedConnect" );
        }
    }




Connection::~Connection() {
    if( mSock != NULL ) {
        delete mSock;
        }
    if( mServer != NULL ) {
        delete mServer;
        }
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }
    }



unsigned char computeBitChecksum( unsigned char inMessage[5] ) {
    int sum = 0;
    
    for( int i=0; i<5; i++ ) {
        for( int b=0; b<8; b++ ) {
            sum += (int)( (inMessage[i] >> b) & 0x01 );
            }
        }
    return (unsigned char)sum;
    }

    
                                            

char Connection::isConnected() {
    if( mSock != NULL ) {
        return mSock->isConnected() == 1;
        }
    return false;
    }




char Connection::step() {
    
    if( mSock == NULL && mServer != NULL ) {
        char timeout;
        mSock = mServer->acceptConnection( 0, &timeout );

        if( mSock == NULL && !timeout ) {
            setError( "err_failedAccept" );
            } 
        return false;
        }
    else if( mSock != NULL && mServer == NULL ) {
        if( mSock->isConnected() == 0 ) {
            // working
            return true;
            }
        else if( mSock->isConnected() == -1 ) {
            setError( "err_failedConnect" );
            return false;
            }
        }
    else if( mSock == NULL ) {
        // maybe failed connect right in constructor
        return false;
        }
    


    // got past here, we have a connected socket
    


    if( mNextOutgoingMessage.length != -1 ) {
        // try sending more of it
        
        
        int i = mNextOutgoingMessage.nextToProcessIndex;

        /*
          // header packed right into send body

        if( i == 0 && mNextOutgoingMessage.headerBytesProcessed != 5 ) {
            // compose and send 5 header bytes

            int numBytes = mNextOutgoingMessage.length;
            
            unsigned char sendData[5];
            
            sendData[0] = mNextOutgoingMessage.channel;
            sendData[1] = ( numBytes >> 24 ) & 0xFF;
            sendData[2] = ( numBytes >> 16 ) & 0xFF;
            sendData[3] = ( numBytes >> 8 ) & 0xFF;
            sendData[4] = ( numBytes ) & 0xFF;
            
            int j = mNextOutgoingMessage.headerBytesProcessed;
            
            // non-blocking, send only the remaining bytes of header
            int numSent = mSock->send( &( sendData[j] ), 5 - j, false );

            if( numSent == -1 ) {
                setError( "err_failedSend" );
                }
            else if( numSent > 0 ) {
                mNextOutgoingMessage.headerBytesProcessed += numSent;
                }
            }
        */
        //else {
        if( true ) {
            
            // send data

            // non-blocking
            int numSent = mSock->send( &( mNextOutgoingMessage.data[ i ] ), 
                                       mNextOutgoingMessage.length - i,
                                       false,
                                       false );  // realtime, no buffering
            if( numSent < 0 ) {
                if( numSent == -1 ) {
                    setError( "err_failedSend" );
                    }
                else {
                    // would block
                    // no more work to do right now
                    return false;
                    }
                }
            else {
                i += numSent;
                AppLog::getLog()->logPrintf( Log::DETAIL_LEVEL,
                                             "Socket sent %d/%d bytes",
                                             numSent, 
                                             mNextOutgoingMessage.length );
                
                if( i == mNextOutgoingMessage.length ) {
                    // done with it!
                    delete [] mNextOutgoingMessage.data;
                    mNextOutgoingMessage.data = NULL;
                    mNextOutgoingMessage.length = -1;
                    mNextOutgoingMessage.nextToProcessIndex = 0;
                    mNextOutgoingMessage.headerBytesProcessed = 0;
                    }
                else {
                    // more to go
                    mNextOutgoingMessage.nextToProcessIndex = i;
                    }
                }        
            }
        return true;
        }
    else {
        // is there another in our queue that we can start sending?
        if( mQueuedOutgoing.size() > 0 ) {
            // grab next
            message *m = mQueuedOutgoing.getElement( 0 );
            
            mNextOutgoingMessage = *m;
            
            mQueuedOutgoing.deleteElement( 0 );
            
            return true;
            }
        }
    



    if( mNextIncomingMessage.length != -1 &&
        mNextIncomingMessage.headerBytesProcessed == 6 ) {
        // try receiving more of it
        
        
        int i = mNextIncomingMessage.nextToProcessIndex;
        
        // non-blocking
        int numReceived = mSock->receive( &( mNextIncomingMessage.data[ i ] ), 
                                          mNextIncomingMessage.length - i,
                                          0 );

        if( numReceived < 0 ) {
            if( numReceived == -1 ) {
                setError( "err_failedReceive" );
                }
            else {
                // timeout
                // no more work right now
                return false;
                }
            }
        else {
            i += numReceived;
            
            if( i == mNextIncomingMessage.length ) {
                
                // push it onto queue
                mQueuedIncoming.push_back( mNextIncomingMessage );
                
                // clear it to make room for next one
                mNextIncomingMessage.data = NULL;
                mNextIncomingMessage.length = -1;
                mNextIncomingMessage.nextToProcessIndex = 0;
                mNextIncomingMessage.headerBytesProcessed = 0;
                }
            else {
                // more to go
                mNextIncomingMessage.nextToProcessIndex = i;
                }
            } 
        return true;
        }
    else {
        // try to receive header of next incoming
        

        // fill buffer with partial stuff already stored in next message
        int numBytes = mNextIncomingMessage.length;

        unsigned char recvData[6];
            
        recvData[0] = mNextIncomingMessage.channel;
        recvData[1] = ( numBytes >> 24 ) & 0xFF;
        recvData[2] = ( numBytes >> 16 ) & 0xFF;
        recvData[3] = ( numBytes >> 8 ) & 0xFF;
        recvData[4] = ( numBytes ) & 0xFF;
        recvData[5] = mNextIncomingMessage.bitCheckSum;

        // fetch remaining part
        int j = mNextIncomingMessage.headerBytesProcessed;
        

        // non-blocking, receive only the remaining bytes of header
        int numReceived = mSock->receive( &( recvData[j] ), 6 - j, 0 );
        
        
        
        if( numReceived == -1 ) {
            setError( "err_failedReceive" );
            }
        else if( numReceived > 0 ) {
            mNextIncomingMessage.headerBytesProcessed += numReceived;
            
            // stick back into length field
            
            mNextIncomingMessage.channel = recvData[0];
            
            mNextIncomingMessage.length = 
                recvData[1] << 24 |
                recvData[2] << 16 |
                recvData[3] << 8 |
                recvData[4];
            mNextIncomingMessage.bitCheckSum = recvData[5];
            

            if( mNextIncomingMessage.headerBytesProcessed == 6 ) {
                // done!
                
                // verify checksum
                if( computeBitChecksum( recvData ) != 
                    mNextIncomingMessage.bitCheckSum ) {
                
                    AppLog::error( "Error:  Message checksum failed" );
                    
                    // clear it to make room for next one,
                    // to ensure that processing of this one stops
                    mNextIncomingMessage.data = NULL;
                    mNextIncomingMessage.length = -1;
                    mNextIncomingMessage.nextToProcessIndex = 0;
                    mNextIncomingMessage.headerBytesProcessed = 0;

                    setError( "err_receiveCorrupted" );
                    }
                else {
                    // checksum correct
                
                    // make a data buffer
                    mNextIncomingMessage.data = 
                        new unsigned char[ mNextIncomingMessage.length ];
                    }
                
                }

            return true;
            }
        }
    
    


    return false;
    }
 
 
 
 
void Connection::sendMessage( unsigned char *inData, int inLength,
                              unsigned char inChannel ) {

    // pack the header right into the message to avoid two consecutive sends 
    // (which interacts poorly with the Nagle algorithm)
    message m = { new unsigned char[ inLength + 6 ], 
                  inLength + 6, 0, 0, inChannel, 
                  0 };  // empty checksum

    // first 6 bytes of data are header
    m.data[0] = m.channel;
    m.data[1] = ( inLength >> 24 ) & 0xFF;
    m.data[2] = ( inLength >> 16 ) & 0xFF;
    m.data[3] = ( inLength >> 8 ) & 0xFF;
    m.data[4] = ( inLength ) & 0xFF;
    m.data[4] = ( inLength ) & 0xFF;

    m.bitCheckSum = computeBitChecksum( m.data ); 
    m.data[5] = m.bitCheckSum;
    
    // remainder are data payload
    memcpy( &( m.data[6] ), inData, inLength );
            
    mQueuedOutgoing.push_back( m );
    }



unsigned char *Connection::receiveMessage( int *outLength, 
                                           unsigned char inChannel ) {

    int queueSize = mQueuedIncoming.size();
    
    if( queueSize > 0 ) {
        
        // skip messages that don't match our channel
        int i = 0;
        char chanMatch = false;
        while( !chanMatch && i<queueSize ) {
            message *m = mQueuedIncoming.getElement( i );
            
            if( m->channel == inChannel ) {
                chanMatch = true;
                }
            else {
                i++;
                }
            }
        
        if( chanMatch ) {

            message *m = mQueuedIncoming.getElement( i );
        
            *outLength = m->length;
        
            unsigned char *returnValue = m->data;
        
            mQueuedIncoming.deleteElement( i );
        
            return returnValue;
            }
        else {
            return NULL;
            }
        }
    
    return NULL;
    }



 

char Connection::isError() {
    return mError;
    }



void Connection::clearError() {
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }
    mErrorString = NULL;
    mError = false;
    }



char *Connection::getErrorString() {
    return mErrorString;
    }



void Connection::setError( const char *inErrorTranslationKey ) {
    mError = true;
    
    if( mErrorString != NULL ) {
        delete [] mErrorString;
        }

    mErrorString = stringDuplicate( TranslationManager::translate( 
                                        (char*)inErrorTranslationKey ) );
    }


