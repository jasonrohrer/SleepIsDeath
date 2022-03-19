/*
 * Modification History
 *
 * 2008-September-11  Jason Rohrer
 * Created.  Copied from Cultivation.
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


// let SDL override our main function with SDLMain
#include <SDL/SDL_main.h>


// must do this before SDL include to prevent WinMain linker errors on win32
int mainFunction( int inArgCount, char **inArgs );

int main( int inArgCount, char **inArgs ) {
    return mainFunction( inArgCount, inArgs );
    }


#include <SDL/SDL.h>



#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/graphics/openGL/SceneHandlerGL.h"
#include "minorGems/graphics/Color.h"

#include "minorGems/graphics/openGL/gui/GUIPanelGL.h"
#include "minorGems/graphics/openGL/gui/GUITranslatorGL.h"
#include "minorGems/graphics/openGL/gui/TextGL.h"
#include "minorGems/graphics/openGL/gui/LabelGL.h"
#include "minorGems/graphics/openGL/gui/TextFieldGL.h"
#include "minorGems/graphics/openGL/gui/SliderGL.h"



#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"

#include "minorGems/io/file/File.h"

#include "minorGems/network/HostAddress.h"

#include "minorGems/network/upnp/portMapping.h"


#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SimpleVector.h"


#include "minorGems/util/log/AppLog.h"
#include "minorGems/util/log/FileLog.h"


#include "common.h"
#include "HighlightLabelGL.h"
#include "Connection.h"
#include "GameHalf.h"
#include "PlayerGame.h"
#include "ControllerGame.h"
#include "musicPlayer.h"
#include "Song.h"
#include "DemoCodeChecker.h"
#include "resourceManager.h"




// from musicPlayer.cpp
extern char noteToggles[PARTS][S][N][N];
extern int partLengths[PARTS];



// some settings

// size of game image
int gameWidth = 320;
int gameHeight = 240;

// size of screen for fullscreen mode
int screenWidth = 640;
int screenHeight = 480;


char hardToQuitMode = false;


double musicTrackFadeLevel = 1.0;


char autoJoinMode = false;
char autoHostMode = false;


char *autoJoinAddress = NULL;
int autoJoinPort = -1;


// ^ and & keys to slow down and speed up for testing
char enableSlowdownKeys = false;
//char enableSlowdownKeys = true;


// is demo mode on?  If so, it checks server to see if it is still allowed
// to run.
char demoMode = false;

int demoCodeLength = 10;

DemoCodeChecker *codeChecker = NULL;


// track old code checkers so that we can destroy them after they unblock
SimpleVector<DemoCodeChecker *> oldCodeCheckers;




// these are shared by other modules as extern
Connection *connection = NULL;
TextGL *largeText = NULL;
TextGL *largeTextFixed = NULL;
char *addressString = NULL;


// should we try UPNP search (blocks) on next redraw?
char tryUPNP = false;
char tryUPNPClose = false;
char upnpPortOpen = false;


// track old clients so that we can destroy them after they unblock
SimpleVector<Connection *> oldConnections;


GameHalf *game = NULL;
char isControllerGame = false;
char loadingDrawnOnce = false;



class GameSceneHandler :
    public SceneHandlerGL, public MouseHandlerGL, public KeyboardHandlerGL,
    public RedrawListenerGL, public ActionListener  { 

	public:

        /**
         * Constructs a sceen handler.
         *
         * @param inScreen the screen to interact with.
         *   Must be destroyed by caller after this class is destroyed.
         */
        GameSceneHandler( ScreenGL *inScreen );

        virtual ~GameSceneHandler();


        
        /**
         * Executes necessary init code that reads from files.
         *
         * Must be called before using a newly-constructed GameSceneHandler.
         *
         * This call assumes that the needed files are in the current working
         * directory.
         */
        void initFromFiles();

        

        ScreenGL *mScreen;


        


        
        
        
		// implements the SceneHandlerGL interface
		virtual void drawScene();

        // implements the MouseHandlerGL interface
        virtual void mouseMoved( int inX, int inY );
        virtual void mouseDragged( int inX, int inY );
        virtual void mousePressed( int inX, int inY );
        virtual void mouseReleased( int inX, int inY );

        // implements the KeyboardHandlerGL interface
        virtual char isFocused() {
            // always focused
            return true;
            }        
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
		virtual void specialKeyPressed( int inKey, int inX, int inY );
		virtual void keyReleased( unsigned char inKey, int inX, int inY );
		virtual void specialKeyReleased( int inKey, int inX, int inY );
        
        // implements the RedrawListener interface
		virtual void fireRedraw();
        

        // implements the ActionListener interface
        virtual void actionPerformed( GUIComponent *inTarget );
        
        
        
    protected:

        // sets the string on a label and re-centers it
        void setLabelString( LabelGL *inLabel, 
                             const char *inTranslationString,
                             double inScaleFactor = 1.0 );
        
        // creates a centerd label at a particular height
        HighlightLabelGL *createLabel( double inGuiY, 
                                       const char *inTranslationString,
                                       TextGL *inText = NULL );
        
        void processSelection( int inSelection );

        int mStartTimeSeconds;
        
        char mPaused;


        char mPrintFrameRate;
        unsigned long mNumFrames;
        unsigned long mFrameBatchSize;
        unsigned long mFrameBatchStartTimeSeconds;
        unsigned long mFrameBatchStartTimeMilliseconds;
        


        Color mBackgroundColor;




        // for game selection display
        char mGUIVisible;
        
        GUIPanelGL *mMainPanel;
        GUITranslatorGL *mMainPanelGuiTranslator;
        
        TextGL *mTextGL;
        TextGL *mTextGLFixedWidth;
        
        GUIPanelGL *mCurrentPanel;

        
        GUIPanelGL *mWriteFailedPanel;
        HighlightLabelGL *mWriteFailedLabel;



        GUIPanelGL *mDemoCodePanel;
        HighlightLabelGL *mEnterDemoCodeLabel;
        TextFieldGL *mEnterDemoCodeField;


        

        GUIPanelGL *mTitlePanel;
        HighlightLabelGL *mTitleLabel;


        GUIPanelGL *mVolumePanel;
        HighlightLabelGL *mVolumeLabel;
        HighlightLabelGL *mVolumeDoneLabel;

        SliderGL *mVolumeSlider;
        
        // avoid abrupt volume changes that cause clicks
        double mVolumeTarget;
        


        GUIPanelGL *mSelectPanel;
        HighlightLabelGL *mControlLocalLabel;
        HighlightLabelGL *mControlRemoteLabel;
        HighlightLabelGL *mJoinAsPlayerLabel;
        
        // track as array to make toggling easier
        HighlightLabelGL *mSelectLabels[3];
        int mSelectHighlightIndex;


        GUIPanelGL *mWaitPlayerPanel;      
        HighlightLabelGL *mWaitPlayerMessageLabel;
        HighlightLabelGL *mWaitPlayerLabel;
        HighlightLabelGL *mAddressLabel;
        HighlightLabelGL *mDoNotWaitTipLabel;
        HighlightLabelGL *mPortTipLabel;


        GUIPanelGL *mUPNPPanel;        
        HighlightLabelGL *mUPNPLabel;


        GUIPanelGL *mJoinControllerPanel;
        HighlightLabelGL *mEnterAddressLabel;
        TextFieldGL *mEnterAddressField;

        

        GUIPanelGL *mLoadingPanel;
        HighlightLabelGL *mLoadingLabel;

	};



GameSceneHandler *sceneHandler;
ScreenGL *screen;
GUITranslatorGL *guiTranslator;


// how many pixels wide is each game pixel drawn as?
int pixelZoomFactor;



int maxAddressLength = 15;




// function that destroys object when exit is called.
// exit is the only way to stop the loop in  ScreenGL
void cleanUpAtExit() {
    AppLog::info( "exiting\n" );


    if( game != NULL ) {
        delete game;
        }


    delete sceneHandler;
    delete screen;


    if( codeChecker != NULL ) {
        delete codeChecker;
        }
    
    for( int i=0; i<oldCodeCheckers.size(); i++ ) {
        delete *( oldCodeCheckers.getElement( i ) );
        }


    
    if( connection != NULL ) {
        delete connection;
        }
    
    for( int i=0; i<oldConnections.size(); i++ ) {
        delete *( oldConnections.getElement( i ) );
        }

    if( addressString != NULL ) {
        delete [] addressString;
        }
    

    if( autoJoinAddress != NULL ) {
        delete [] autoJoinAddress;
        }
    
    
    stopMusic();

    SDL_Quit();
    }




#include "StringTree.h"


int mainFunction( int inNumArgs, char **inArgs ) {
    

    // parse ip:port from first arg

    if( inNumArgs > 1 ) {
    
        autoJoinAddress = stringDuplicate( inArgs[1] );

        char *colonPointer = strstr( autoJoinAddress, ":" );
        
        if( colonPointer == NULL ) {
            // address, no port
            // don't touch the string
            }
        else {
            sscanf( &( colonPointer[1] ), "%d", &autoJoinPort );
        
            // terminate
            colonPointer[0] = '\0';
            }
        }
    



    /*
    // test tree
    StringTree *tree = new StringTree();
    
    // use strings as values, too
    char *apple = "apple";
    tree->insert( "apple", (void*)apple );
    //tree->insert( "dapper", (void*)"dapper" );
    //tree->insert( "relapse", (void*)"relapse" );
    tree->insert( "apron", (void*)"apron" );    

    char* aaron = "aaron";
    
    tree->insert( "aaron", (void*)aaron );

    tree->insert( "no", (void*)"no" );
    tree->insert( "dodo", (void*)"dodo" );
    tree->insert( "dodomanofdodo", (void*)"dodomanofdodo" );
    tree->insert( "dad", (void*)"dad" );


    tree->remove( "aaron", aaron );
    tree->remove( "apple", apple );
    
    
    char *searchString = "a";

    int numMatches = tree->countMatches( searchString );
    
    char **results = new char*[numMatches];

    for( int j=0; j<numMatches; j++ ) {
        printf( "Skipping %d gives:\n", j );
        
        int numGotten = tree->getMatches( searchString, j, 2,
                                          (void **)results );
    
        for( int i=0; i<numGotten; i++ ) {
            printf( "Test result:  %s\n", results[i] );
            }
        }
    
    delete [] results;
    
    delete tree;
    */





    // check result below, after opening log, so we can log failure
    int sdlResult = SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE );


    // do this mac check after initing SDL,
    // since it causes various Mac frameworks to be loaded (which can
    // change the current working directory out from under us)
    #ifdef __mac__
        // make sure working directory is the same as the directory
        // that the app resides in
        // this is especially important on the mac platform, which
        // doesn't set a proper working directory for double-clicked
        // app bundles

        // arg 0 is the path to the app executable
        char *appDirectoryPath = stringDuplicate( inArgs[0] );
    
        char *appNamePointer = strstr( appDirectoryPath,
                                       "SleepIsDeath.app" );

        if( appNamePointer != NULL ) {
            // terminate full app path to get parent directory
            appNamePointer[0] = '\0';
            
            chdir( appDirectoryPath );
            }
        
        delete [] appDirectoryPath;
    #endif

        

    AppLog::setLog( new FileLog( "log.txt" ) );
    AppLog::setLoggingLevel( Log::DETAIL_LEVEL );
    
    AppLog::info( "New game starting up" );
    

    if( sdlResult < 0 ) {
        AppLog::getLog()->logPrintf( 
            Log::CRITICAL_ERROR_LEVEL,
            "Couldn't initialize SDL: %s\n", SDL_GetError() );
        return 0;
        }


    // read screen size from settings
    char widthFound = false;
    int readWidth = SettingsManager::getIntSetting( "screenWidth", 
                                                    &widthFound );
    char heightFound = false;
    int readHeight = SettingsManager::getIntSetting( "screenHeight", 
                                                    &heightFound );
    
    if( widthFound && heightFound ) {
        // override hard-coded defaults
        screenWidth = readWidth;
        screenHeight = readHeight;
        }
    
    AppLog::getLog()->logPrintf( 
        Log::INFO_LEVEL,
        "Screen dimensions for fullscreen mode:  %dx%d\n",
        screenWidth, screenHeight );


    char fullscreenFound = false;
    int readFullscreen = SettingsManager::getIntSetting( "fullscreen", 
                                                         &fullscreenFound );
    
    char fullscreen = true;
    
    if( readFullscreen == 0 ) {
        fullscreen = false;
        }
    

    screen =
        new ScreenGL( screenWidth, screenHeight, fullscreen,
                      30,
                      "SleepIsDeath", NULL, NULL, NULL );

    // may change if specified resolution is not supported
    screenWidth = screen->getWidth();
    screenHeight = screen->getHeight();
    

    SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY,
                         SDL_DEFAULT_REPEAT_INTERVAL );

    pixelZoomFactor = screenWidth / gameWidth;

    if( pixelZoomFactor * gameHeight > screenHeight ) {
        // screen too short
        pixelZoomFactor = screenHeight / gameHeight;
        }
    

    screen->setImageSize( pixelZoomFactor * gameWidth,
                          pixelZoomFactor * gameHeight );
    

    //SDL_ShowCursor( SDL_DISABLE );


    sceneHandler = new GameSceneHandler( screen );

    

    // also do file-dependent part of init for GameSceneHandler here
    // actually, constructor is file dependent anyway.
    sceneHandler->initFromFiles();
    

    // hard to quit mode?
    char hardToQuitFound = false;
    int readHardToQuit = SettingsManager::getIntSetting( "hardToQuitMode", 
                                                         &hardToQuitFound );
    
    if( readHardToQuit == 1 ) {
        hardToQuitMode = true;
        }



    // auto-join mode?
    char autoJoinFound = false;
    int readAutoJoin = SettingsManager::getIntSetting( "autoJoin", 
                                                       &autoJoinFound );
    
    if( readAutoJoin == 1 || autoJoinAddress != NULL ) {
        autoJoinMode = true;
        }

    // auto-host mode?
    char autoHostFound = false;
    int readAutoHost = SettingsManager::getIntSetting( "autoHost", 
                                                       &autoHostFound );
    
    if( readAutoHost == 1 ) {
        autoHostMode = true;
        }


    if( autoJoinMode && autoHostMode ) {
        // only allow one
        AppLog::info( 
            "Both autoJoin and autoHost set... defaulting to neither" );
        autoJoinMode = false;
        autoHostMode = false;
        }
    


    setMusicLoudness( 0.5 );
    startMusic();

    
        
    // register cleanup function, since screen->start() will never return
    atexit( cleanUpAtExit );




    screen->switchTo2DMode();
    
    //glLineWidth( pixelZoomFactor );

    screen->start();

    
    return 0;
    }




GameSceneHandler::GameSceneHandler( ScreenGL *inScreen )
    : mScreen( inScreen ),
      mStartTimeSeconds( time( NULL ) ),
      mPaused( false ),
      mPrintFrameRate( false ),
      mNumFrames( 0 ), mFrameBatchSize( 100 ),
      mFrameBatchStartTimeSeconds( time( NULL ) ),
      mFrameBatchStartTimeMilliseconds( 0 ),
      mBackgroundColor( 0, 0, 0, 1 ) { 
    
    
    mVolumeTarget = 0.5;
    

    glClearColor( mBackgroundColor.r,
                  mBackgroundColor.g,
                  mBackgroundColor.b,
                  mBackgroundColor.a );
    

    // set external pointer so it can be used in calls below
    sceneHandler = this;

    
    mScreen->addMouseHandler( this );
    mScreen->addKeyboardHandler( this );
    mScreen->addSceneHandler( this );
    mScreen->addRedrawListener( this );

    
    Time::getCurrentTime( &mFrameBatchStartTimeSeconds, 
                          &mFrameBatchStartTimeMilliseconds );
    


    // set up main UI
    mMainPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                 new Color( 0,
                                            0,
                                            0, 1.0 ) );

	mMainPanelGuiTranslator = new GUITranslatorGL( mMainPanel, mScreen, 
                                                   gameWidth );

    guiTranslator = mMainPanelGuiTranslator;
    

	mScreen->addSceneHandler( mMainPanelGuiTranslator );
    mScreen->addKeyboardHandler( mMainPanelGuiTranslator );
    mScreen->addMouseHandler( mMainPanelGuiTranslator );
    
    mGUIVisible = true;


    // construct sub-panels, but only add the first one

    mWriteFailedPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                        new Color( 0,
                                                   0,
                                                   0, 1.0 ) );


    mDemoCodePanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                     new Color( 0,
                                                0,
                                                0, 1.0 ) );
    
    mTitlePanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                  new Color( 0,
                                             0,
                                             0, 1.0 ) );

    mVolumePanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                   new Color( 0,
                                              0,
                                              0, 1.0 ) );

    mSelectPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                   new Color( 0,
                                              0,
                                              0, 1.0 ) );
    
    mWaitPlayerPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                       new Color( 0,
                                                  0,
                                                  0, 1.0 ) );

    mUPNPPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                 new Color( 0,
                                            0,
                                            0, 1.0 ) );

    mJoinControllerPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                          new Color( 0,
                                                     0,
                                                     0, 1.0 ) );

    mLoadingPanel = new GUIPanelGL( 0, 0, gameWidth, gameWidth,
                                    new Color( 0,
                                               0,
                                               0, 1.0 ) );


    // make sure we can write to our main folder and resource cache folder
    FILE *testFile = fopen( "testWrite.txt", "w" );
    
    if( testFile == NULL ) {
        // failed
        mMainPanel->add( mWriteFailedPanel );
        mCurrentPanel = mWriteFailedPanel;
        }
    else {
        fclose( testFile );
        
        remove( "testWrite.txt" );
        
        if( ! testResourceCacheWritePermissions() ) {
            // failed
            mMainPanel->add( mWriteFailedPanel );
            mCurrentPanel = mWriteFailedPanel;
            }
        else {
            if( demoMode ) {
                mMainPanel->add( mDemoCodePanel );
                mCurrentPanel = mDemoCodePanel;
                }
            else {
                mMainPanel->add( mTitlePanel );
                mCurrentPanel = mTitlePanel;
                }
            }
        }


    }



GameSceneHandler::~GameSceneHandler() {
    mScreen->removeMouseHandler( this );
    mScreen->removeSceneHandler( this );
    mScreen->removeRedrawListener( this );

    // remove before deleting, since we're not sure which one is still
    // added
    mMainPanel->remove( mWriteFailedPanel );
    mMainPanel->remove( mDemoCodePanel );
    mMainPanel->remove( mTitlePanel );
    mMainPanel->remove( mVolumePanel );
    mMainPanel->remove( mSelectPanel );
    mMainPanel->remove( mWaitPlayerPanel );
    mMainPanel->remove( mUPNPPanel );
    mMainPanel->remove( mJoinControllerPanel );
    mMainPanel->remove( mLoadingPanel );


    delete mWriteFailedPanel;
    delete mDemoCodePanel;    
    delete mTitlePanel;    
    delete mVolumePanel;    
    delete mSelectPanel;
    delete mWaitPlayerPanel;
    delete mUPNPPanel;
    delete mJoinControllerPanel;
    delete mLoadingPanel;
    
    
    

    // this will recursively delete all of our selector GUI components
	delete mMainPanelGuiTranslator;

    delete mTextGL;
    delete mTextGLFixedWidth;


    }



void GameSceneHandler::setLabelString( LabelGL *inLabel, 
                                       const char *inTranslationString,
                                       double inScaleFactor ) {
    char *labelString =
        (char *)TranslationManager::translate( (char*)inTranslationString );
    
    inLabel->setText( labelString );

    TextGL *text = inLabel->getTextGL();
    

    // 1:1 aspect ratio
    // we know font is 8 pixels wide/tall
    double height = inScaleFactor * 8;
    double width = height * strlen( labelString );
    
    double actualDrawWidth =
        height * text->measureTextWidth( labelString );
          
    /*
    // round to an integer number of pixels
    actualDrawWidth = 
        round( actualDrawWidth * screenImageWidth )
        /
        screenImageWidth;
    */
      
    double centerW = gameWidth / 2;
        //0.5 * (double)( mScreen->getImageWidth() ) / screenImageHeight;
    
    double guiY = inLabel->getAnchorY();
    

    double labelX = centerW - 0.5 * actualDrawWidth;
    

    /*
    // round to an integer number of pixels
    labelX =
        round( labelX * screenImageWidth )
        /
        screenImageWidth;
    
    guiY =
        round( guiY * screenImageWidth )
        /
        screenImageWidth;
    */
    
    
    inLabel->setPosition( labelX,
                          guiY,
                          width,
                          height );
    }



HighlightLabelGL *GameSceneHandler::createLabel( 
    double inGuiY, 
    const char *inTranslationString,
    TextGL *inText ) {
    
    if( inText == NULL ) {
        inText = mTextGL;
        }

    HighlightLabelGL *returnLabel =
        new HighlightLabelGL( 0, inGuiY, 0, 0, "", inText );
    
    setLabelString( returnLabel, inTranslationString );

    return returnLabel;
    }






void GameSceneHandler::initFromFiles() {
    
    // translation language
    File languageNameFile( NULL, "language.txt" );

    if( languageNameFile.exists() ) {
        char *languageNameText = languageNameFile.readFileContents();

        SimpleVector<char *> *tokens = tokenizeString( languageNameText );

        int numTokens = tokens->size();
        
        // first token is name
        if( numTokens > 0 ) {
            char *languageName = *( tokens->getElement( 0 ) );
        
            TranslationManager::setLanguage( languageName );
            }
        else {
            // default

            // TranslationManager already defaults to English, but
            // it looks for the language files at runtime before we have set
            // the current working directory.
            
            // Thus, we specify the default again here so that it looks
            // for its language files again.
            TranslationManager::setLanguage( "English" );
            }
        
        delete [] languageNameText;

        for( int t=0; t<numTokens; t++ ) {
            delete [] *( tokens->getElement( t ) );
            }
        delete tokens;
        }
    


    // load text font
    Image *fontImage = readTGA( "font_8_16.tga" );

    if( fontImage == NULL ) {
        // default
        // blank font
        fontImage = new Image( 256, 256, 4, true );
        }
    
    mTextGL = new TextGL( fontImage,
                          // use alpha
                          true,
                          // variable character width
                          false,
                          // extra space around each character, one pixel
                          0.125,
                          // space is 4 pixels wide (out of 8)
                          0.5 );
    largeText = mTextGL;
    
    mTextGLFixedWidth = new TextGL( fontImage,
                                    // use alpha
                                    true,
                                    // fixed character width
                                    true,
                                    // extra space around each character
                                    0,
                                    // space is full char width
                                    1.0 );
    
    largeTextFixed = mTextGLFixedWidth;
    
    delete fontImage;




    // now build gui panels based on labels, which depend on textGL

    mWriteFailedLabel = createLabel( 160, "writeFailed" );

    // increase scale
    setLabelString( mWriteFailedLabel, "writeFailed" );

    mWriteFailedPanel->add( mWriteFailedLabel );



    
    mEnterDemoCodeLabel = createLabel( 175, "enterDemoCode" );
    mDemoCodePanel->add( mEnterDemoCodeLabel );


    // 1:1 aspect ratio
    // we know font is 8 pixels wide/tall
    
    double height = 8;
    double width = height * demoCodeLength;
                
    double centerW = gameWidth / 2;
    

    char *defaultCode = 
        SettingsManager::getStringSetting( "demoCode" );

    char *fieldDefault = (char*)"";
    if( defaultCode != NULL ) {
        fieldDefault = defaultCode;
        }

    mEnterDemoCodeField = new TextFieldGL( centerW - 0.5 * width,
                                           145,
                                           width,
                                           height,
                                           1,
                                           fieldDefault,
                                           mTextGLFixedWidth,
                                           new Color( 0.75, 0.75, 0.75 ),
                                           new Color( 0.75, 0.75, 0.75 ),
                                           new Color( 0.15, 0.15, 0.15 ),
                                           new Color( 0.75, 0.75, 0.75 ),
                                           demoCodeLength,
                                           true );

    mDemoCodePanel->add( mEnterDemoCodeField );


    if( demoMode ) {
        mEnterDemoCodeField->setEnabled( true );
        mEnterDemoCodeField->setFocus( true );
        mEnterDemoCodeField->lockFocus( true );
        }



    if( defaultCode != NULL ) {

        mEnterDemoCodeField->setCursorPosition( strlen( defaultCode ) );
        

        if( demoMode ) {
            
            // don't destroy this
            char *enteredCode = mEnterDemoCodeField->getText();
            
            if( strlen( enteredCode ) > 0 ) {

                // run with this code
                // disable further input
                mEnterDemoCodeField->setEnabled( false );
                mEnterDemoCodeField->setFocus( false );
                mEnterDemoCodeField->lockFocus( false );
                
                setLabelString( mEnterDemoCodeLabel, 
                                "checkingCode" );
                
                // start 
                codeChecker = new DemoCodeChecker( enteredCode );
                }
            }
        

        delete [] defaultCode;
        }
    
    


    

    mTitleLabel = createLabel( 160, "title" );

    mTitlePanel->add( mTitleLabel );



    mVolumeLabel = createLabel( 160, "volume" );

    mVolumePanel->add( mVolumeLabel );


    mVolumeDoneLabel = createLabel( 48, "volumeDone" );

    mVolumePanel->add( mVolumeDoneLabel );



    Color thumbColor( .5, .5, .5, .5 );
    Color borderColor( .35, .35, .35, .35 );
    
    mVolumeSlider = new SliderGL( 160 - 24, 104,
                                  48, 12,
                                  NULL, 0,
                                  new Color( 0, 0, 0, 1 ),
                                  new Color( 1, 0.5, 0, 1 ),
                                  thumbColor.copy(),
                                  borderColor.copy(),
                                  1, 4, 1 );
    mVolumePanel->add( mVolumeSlider );

    // volume starts in middle
    // avoid round-off errors
    mVolumeSlider->setThumbPosition( (int)(0.5 * 96) / 96.0 );
    
    mVolumeSlider->addActionListener( this );
    

    mSelectLabels[0] = 
        mControlLocalLabel = createLabel( 190, "control_local" );

    mSelectLabels[1] = 
        mControlRemoteLabel = createLabel( 160, "control_remote" );

    mSelectLabels[2] = 
        mJoinAsPlayerLabel = createLabel( 130, "joinAsPlayer" );
    
    mSelectPanel->add( mControlLocalLabel );
    mSelectPanel->add( mControlRemoteLabel );
    mSelectPanel->add( mJoinAsPlayerLabel );
    
    mSelectLabels[0]->setHighlight( true );
    mSelectHighlightIndex = 0;

    mSelectLabels[0]->addActionListener( this );
    mSelectLabels[1]->addActionListener( this );
    mSelectLabels[2]->addActionListener( this );
    


    HighlightLabelGL *escLabel = createLabel( 48, "escToQuit" );

    mSelectPanel->add( escLabel );



    char *portString = autoSprintf( "%s  %d", 
                                    TranslationManager::translate( "portTip" ),
                                    Connection::getPort() );
    mPortTipLabel = createLabel( 48, portString );
    delete [] portString;
    
    

    mWaitPlayerMessageLabel = createLabel( 205, "" );



    mWaitPlayerLabel = createLabel( 175, "waitingForPlayer" );
    
    HostAddress *local = HostAddress::getNumericalLocalAddress();


    if( local != NULL ) {
        addressString = stringDuplicate( local->mAddressString );
        delete local;
        }
    else {
        addressString = stringDuplicate( "???.???.???.???" );
        }
    
    
    mAddressLabel = createLabel( 145, addressString, 
                                 mTextGLFixedWidth );


    mDoNotWaitTipLabel = createLabel( 115, "doNotWaitTip" );

    mWaitPlayerPanel->add( mWaitPlayerMessageLabel );
    mWaitPlayerPanel->add( mWaitPlayerLabel );
    mWaitPlayerPanel->add( mAddressLabel );
    mWaitPlayerPanel->add( mDoNotWaitTipLabel );
    mWaitPlayerPanel->add( mPortTipLabel );
    
    mPortTipLabel->setEnabled( false );
    


    mUPNPLabel = createLabel( 160, "upnpTrying" );
    mUPNPPanel->add( mUPNPLabel );
    


    mEnterAddressLabel = createLabel( 175, "enterAddress" );
    mJoinControllerPanel->add( mEnterAddressLabel );


    mLoadingLabel = createLabel( 160, "loading" );

    mLoadingPanel->add( mLoadingLabel );
    


    // 1:1 aspect ratio
    // we know font is 8 pixels wide/tall
    
    height = 8;
    width = height * maxAddressLength;
                
    centerW = gameWidth / 2;
        

    char *defaultAddress = NULL;
    
    if( autoJoinAddress != NULL ) {
        defaultAddress = stringDuplicate( autoJoinAddress );
        }
    else {
        defaultAddress = 
            SettingsManager::getStringSetting( "defaultServerAddress" );
        }
    
    fieldDefault = (char*)"";
    if( defaultAddress != NULL ) {
        fieldDefault = defaultAddress;
        }
    

    mEnterAddressField = new TextFieldGL( centerW - 0.5 * width,
                                          145,
                                          width,
                                          height,
                                          1,
                                          fieldDefault,
                                          mTextGLFixedWidth,
                                          new Color( 0.75, 0.75, 0.75 ),
                                          new Color( 0.75, 0.75, 0.75 ),
                                          new Color( 0.15, 0.15, 0.15 ),
                                          new Color( 0.75, 0.75, 0.75 ),
                                          maxAddressLength,
                                          true );
    
    mEnterAddressField->setCursorPosition( strlen( fieldDefault ) );
    
    mJoinControllerPanel->add( mEnterAddressField );


    if( defaultAddress != NULL ) {
        delete [] defaultAddress;
        }
    




    }





void GameSceneHandler::drawScene() {
    /*
    glClearColor( mBackgroundColor->r,
                  mBackgroundColor->g,
                  mBackgroundColor->b,
                  mBackgroundColor->a );
    */
	
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );

    
    /*
    // sky
    glColor4d( 0.75, 0.75, 0.75, 1.0 );

    glBegin( GL_QUADS ); {
        glVertex2d( -1, 1 );
        glVertex2d( 1, 1 );
        glVertex2d( 1, -1 );
        glVertex2d( -1, -1 );
        }
    glEnd();
    */
    }



void GameSceneHandler::mouseMoved( int inX, int inY ) {
    }



void GameSceneHandler::mouseDragged( int inX, int inY ) {
    }




void GameSceneHandler::mousePressed( int inX, int inY ) {
    }



void GameSceneHandler::mouseReleased( int inX, int inY ) {
    // actually, WHY do we let people mouse-click from title?
    // rest of menu system is all keyboard-driven
    // this sets up a false expectation that the mouse will do something
    // in the menus
    /*
    if( mGUIVisible ) {

        if( mMainPanel->contains( mTitlePanel ) ) {         

            mMainPanel->remove( mCurrentPanel );
            mMainPanel->add( mVolumePanel );
            mCurrentPanel = mVolumePanel;

            // test sounds on
            for( int i=0; i<N; i++ ) {
                noteToggles[i][i] = true;
                }
            restartMusic();
            }
        return;
        }
    */
    }



void GameSceneHandler::fireRedraw() {
    
    if( mPaused ) {
        // ignore redraw event

        // sleep to avoid wasting CPU cycles
        Thread::staticSleep( 1000 );
        
        return;
        }

    
    


    mNumFrames ++;

    if( mPrintFrameRate ) {
        
        if( mNumFrames % mFrameBatchSize == 0 ) {
            // finished a batch
            
            unsigned long timeDelta =
                Time::getMillisecondsSince( mFrameBatchStartTimeSeconds,
                                            mFrameBatchStartTimeMilliseconds );

            double frameRate =
                1000 * (double)mFrameBatchSize / (double)timeDelta;
            
            AppLog::getLog()->logPrintf( 
                Log::DETAIL_LEVEL,
                "Frame rate = %f frames/second\n", frameRate );
            
            Time::getCurrentTime( &mFrameBatchStartTimeSeconds, 
                                  &mFrameBatchStartTimeMilliseconds );
            }
        }


    // process old connections to clear pending ops and destroy them
    for( int i=0; i<oldConnections.size(); i++ ) {
        Connection *c = *( oldConnections.getElement( i ) );
        
        if( c->step() == false ) {
            // done
            AppLog::info( "Destroying old connection.\n" );
            
            delete c;

            oldConnections.deleteElement( i );
            
            i--;
            }
        }



    // should we start a game?
    if( mMainPanel->contains( mLoadingPanel ) ) {
        if( loadingDrawnOnce ) {
            
            // loading panel drawn at least once
            
            if( isControllerGame ) {
                game = new ControllerGame( mScreen );
                }
            else {
                // back to default sounds (in case we're Player in a 
                // v13 game)
                setDefaultMusicSounds();
      
                game = new PlayerGame( mScreen );
                isControllerGame = false;
                }

            if( mGUIVisible ) {
                // hide menu gui to start the game
                
                mMainPanel->remove( mCurrentPanel );
                mCurrentPanel = NULL;
                        
                mScreen->removeSceneHandler( mMainPanelGuiTranslator );
                mScreen->removeKeyboardHandler( mMainPanelGuiTranslator );
                mScreen->removeMouseHandler( mMainPanelGuiTranslator );
                mGUIVisible = false;


                // add game handlers instead
                mScreen->addSceneHandler( game );
                
                // insert as first handler to catch quit events
                mScreen->addKeyboardHandler( game, true );
                mScreen->addMouseHandler( game );
                mScreen->addRedrawListener( game );
                        
                game->setScreen( screen );
                }
            }
        else {
            loadingDrawnOnce = true;
            }
        
        }




    
    if( connection != NULL ) {
        
        if( ! connection->isError() ) {

            char workLeft = connection->step();
        
            
            if( connection->isConnected() && game == NULL ) {


                if( mMainPanel->contains( mJoinControllerPanel ) ) {
                    
                    mMainPanel->remove( mJoinControllerPanel );
                    
                    mMainPanel->add( mLoadingPanel );
                    mCurrentPanel = mLoadingPanel;
                    loadingDrawnOnce = false;
                    
                    isControllerGame = false;
                    }
                else if( mMainPanel->contains( mWaitPlayerPanel ) ) {
                    mMainPanel->remove( mWaitPlayerPanel );
                    
                    mMainPanel->add( mLoadingPanel );
                    mCurrentPanel = mLoadingPanel;
                    loadingDrawnOnce = false;
                    
                    isControllerGame = true;
                    }
                // we construct the game outside the check for a connection
                // (based on presence of loading panel) so that same
                // construction code can be used even if player hits G
                // to start w/out a connection
                
                /*

                if( mMainPanel->contains( mJoinControllerPanel ) ) {
                    // got connection as player
                    
                    // back to default sounds (in case we're Player in a 
                    // v13 game)
                    setDefaultMusicSounds();


                    game = new PlayerGame( mScreen );
                    isControllerGame = false;
                    }
                else if( mMainPanel->contains( mWaitPlayerPanel ) ) {
                    // got connection as controller
                    game = new ControllerGame( mScreen );
                    isControllerGame = true;
                    }
                */
                



                // if connected, keep stepping until no network work left
                while( workLeft ) {
                    workLeft = connection->step();
                    }
                }
            
            }
        else {
            AppLog::getLog()->logPrintf( 
                Log::INFO_LEVEL,
                "Connection failed:  %s\n", connection->getErrorString() );
            
            oldConnections.push_back( connection );
            
            connection = NULL;

            
            if( mMainPanel->contains( mJoinControllerPanel ) ) {
                
                // re-enable input
                mEnterAddressField->setEnabled( true );
                mEnterAddressField->setFocus( true );
                mEnterAddressField->lockFocus( true );
                                    
                setLabelString( mEnterAddressLabel, 
                                "joiningFailed" );
                }
            }
        }
    else {
        // no connection... is a Controller game already running?
        if( game != NULL && isControllerGame ) {
        
            // are any old connections (which might block hosting on port)
            // still waiting to be destroyed?
            
            if( oldConnections.size() == 0 ) {
                
                // clear to start a new one
                AppLog::info( 
                    "Trying to receive a new Player connection...\n" );
                 // start a server
                connection = new Connection();

                game->resetTimer();
                }
            }
        }



    // process old codeCheckers to clear pending ops and destroy them
    for( int i=0; i<oldCodeCheckers.size(); i++ ) {
        DemoCodeChecker *c = *( oldCodeCheckers.getElement( i ) );
        
        if( c->step() == false ) {
            // done
            AppLog::info( "Destroying old codeChecker.\n" );
            
            delete c;

            oldCodeCheckers.deleteElement( i );
            
            i--;
            }
        }
    
    if( codeChecker != NULL ) {
        
        if( codeChecker->isError() ) {
            if( mMainPanel->contains( mDemoCodePanel ) ) {
                
                // re-enable input
                mEnterDemoCodeField->setEnabled( true );
                mEnterDemoCodeField->setFocus( true );
                mEnterDemoCodeField->lockFocus( true );
                        

                char *message = codeChecker->getErrorString();
                
                setLabelString( mEnterDemoCodeLabel, message );
                }
            oldCodeCheckers.push_back( codeChecker );
            
            codeChecker = NULL;
            }
        else {

            char checkerWorkLeft = codeChecker->step();

            if( ! checkerWorkLeft 
                && ! codeChecker->isError()
                && codeChecker->codePermitted() ) {
                
                // will have error if not permitted, which is handled above

                oldCodeCheckers.push_back( codeChecker );
                
                codeChecker = NULL;
                
                if( mMainPanel->contains( mDemoCodePanel ) ) {
                    // move on to title
                    
                    mMainPanel->remove( mCurrentPanel );
                    mMainPanel->add( mTitlePanel );
                    mCurrentPanel = mTitlePanel;
                    }
                }
            }
        
        }
    
    



    if( game != NULL ) {
        game->step();
        
        if( game->isQuitting() ) {
            
            // back out and restore menu gui
            if( !mGUIVisible ) {
                
                mScreen->addSceneHandler( mMainPanelGuiTranslator );
                mScreen->addKeyboardHandler( mMainPanelGuiTranslator );
                mScreen->addMouseHandler( mMainPanelGuiTranslator );
                mGUIVisible = true;
                
                
                
                mScreen->removeSceneHandler( game );
                mScreen->removeKeyboardHandler( game );
                mScreen->removeMouseHandler( game );
                mScreen->removeRedrawListener( game );
                }
                
            delete game;
            game = NULL;

            if( connection != NULL ) {
                oldConnections.push_back( connection );
                connection = NULL;
                }

            // clear music
            for( int si=0; si<PARTS; si++ ) {
                for( int s=0; s<S; s++ ) {
                
                    for( int i=0; i<N; i++ ) {
                        for( int j=0; j<N; j++ ) {
                            noteToggles[si][s][i][j] = false;
                            }
                        }
                    }
                }
            

            
            
            if( !upnpPortOpen ) {
                // right back to game selection menu
                mMainPanel->add( mSelectPanel );
                mCurrentPanel = mSelectPanel;
                }
            else {
                // close UPNP port first 
                                              
                mMainPanel->add( mUPNPPanel );
                mCurrentPanel = mUPNPPanel;
                setLabelString( mUPNPLabel, "upnpCloseTrying" );

                // wait for it to draw once before blocking
                tryUPNPClose = false;
                }
            }
        
        }
    

    // gradually adjust loudness toward target to avoid clicks

    double trueVolume = getMusicLoudness();
    if( mVolumeTarget != trueVolume ) {
        
        double newTrueVolume = trueVolume;
        
        if( mVolumeTarget > trueVolume ) {
            newTrueVolume += 0.06;
        
            if( newTrueVolume > mVolumeTarget ) {
                newTrueVolume = mVolumeTarget;
                }
            }
        if( mVolumeTarget < trueVolume ) {
            newTrueVolume -= 0.06;
        
            if( newTrueVolume < mVolumeTarget ) {
                newTrueVolume = mVolumeTarget;
                }
            }

        setMusicLoudness( newTrueVolume );
        }
    



    // handle auto-starts here
    if( autoHostMode && ! mMainPanel->contains( mWaitPlayerPanel ) ) {
        // done, don't re-trigger if player backs out of it
        autoHostMode = false;
        
        mEnterAddressField->setFocus( false );
        mEnterAddressField->lockFocus( false );
        mEnterAddressField->setEnabled( false );
        
        setLabelString( mWaitPlayerLabel, "waitingForPlayer" );
        
        // start as server
        connection = new Connection();

        mMainPanel->remove( mCurrentPanel );
        mMainPanel->add( mWaitPlayerPanel );
        mCurrentPanel = mWaitPlayerPanel;
        }
    
    if( autoJoinMode && ! mMainPanel->contains( mJoinControllerPanel ) ) {
        // done, don't re-trigger if player backs out of it
        autoJoinMode = false;

        char *enteredAddress = mEnterAddressField->getText();
                    
        if( strlen( enteredAddress ) > 0 ) {
            // disable further input
            mEnterAddressField->setFocus( false );
            mEnterAddressField->lockFocus( false );
            mEnterAddressField->setEnabled( false );
            
            setLabelString( mEnterAddressLabel, 
                            "joiningController" );
            
            // start as client
            connection = new Connection( enteredAddress );
        
            mMainPanel->remove( mCurrentPanel );
            mMainPanel->add( mJoinControllerPanel );
            mCurrentPanel = mJoinControllerPanel;
            }
        }


    if( mMainPanel->contains( mUPNPPanel ) && !tryUPNP && !upnpPortOpen ) {
        // try blocking op on next frame, so that panel can draw at least once
        tryUPNP = true;
        }        
    else if( tryUPNP ) {
        tryUPNP = false;

        char *externalIP;
        
        int result = mapPort( Connection::getPort(), "Sleep Is Death",
                              2000,
                              &externalIP );

        if( result == 1 ) {
            upnpPortOpen = true;
            if( externalIP != NULL ) {    
                setLabelString( mAddressLabel, externalIP );
                
                delete [] addressString;
                addressString = externalIP;
                }
            }
        
        mMainPanel->remove( mCurrentPanel );
        mMainPanel->add( mWaitPlayerPanel );
        mCurrentPanel =  mWaitPlayerPanel;
        
        mEnterAddressField->setFocus( false );
        mEnterAddressField->lockFocus( false );
        mEnterAddressField->setEnabled( false );
            
        setLabelString( mWaitPlayerLabel, "waitingForPlayer" );
        
        // start as server
        connection = new Connection();


        if( result == 1 ) {
            setLabelString( mWaitPlayerMessageLabel, "upnpSuccess" );
            mPortTipLabel->setEnabled( false );
            }
        else {
            setLabelString( mWaitPlayerMessageLabel, "upnpFailure" );
            mPortTipLabel->setEnabled( true );
            }
        }


    if( mMainPanel->contains( mUPNPPanel ) && upnpPortOpen && !tryUPNPClose ) {
        // try blocking op on next frame, so that panel can draw at least once
        tryUPNPClose = true;
        }        
    else if( tryUPNPClose ) {
        tryUPNPClose = false;

        int result = unmapPort( Connection::getPort(), 2000 );

        if( result == 1 ) {
            upnpPortOpen = false;
            }
        
        if( game == NULL ) {
            // not in the middle of a game, back out to select panel
            mMainPanel->remove( mCurrentPanel );
            mMainPanel->add( mSelectPanel );
            mCurrentPanel = mSelectPanel;


            // back to local address
            HostAddress *local = HostAddress::getNumericalLocalAddress();


            if( local != NULL ) {
                delete [] addressString;
                
                addressString = stringDuplicate( local->mAddressString );
            
                setLabelString( mAddressLabel, addressString );
                
                delete local;
                }

            }
        else {
            // quitting, but we just closed the UPNP port first
            exit( 0 );
            }
        }

    }



static unsigned char lastKeyPressed = '\0';


void GameSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    if( enableSlowdownKeys ) {         
        if( inKey == '^' ) {
            // slow
            mScreen->setMaxFrameRate( 2 );
            }
        if( inKey == '&' ) {
            // normal
            mScreen->setMaxFrameRate( 30 );
            }
        }

    
    if( mGUIVisible ) {

        if( mMainPanel->contains( mTitlePanel ) || 
            mMainPanel->contains( mVolumePanel ) ) {
            if( !hardToQuitMode ) {
                // q or escape
                if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                    exit( 0 );
                    }
                }
            else {
                // # followed by ESC
                if( lastKeyPressed == '#' && inKey == 27 ) {
                    exit( 0 );
                    }
                lastKeyPressed = inKey;
                }

            if( inKey != 'q' && inKey != 'Q' && inKey != 27 && inKey != '#' ) {
                // any other key advances to next screen
                if( mMainPanel->contains( mTitlePanel ) ) {
                    mMainPanel->remove( mCurrentPanel );
                    mMainPanel->add( mVolumePanel );
                    mCurrentPanel = mVolumePanel;

                    // test sounds on
                    // emulate old 1-grid test run (rising tones)
                    for( int i=0; i<N-1; i++ ) {
                        noteToggles[i/5][0][i%5][i] = true;
                        }
                    // last note at top
                    noteToggles[2][0][5][15] = true;
                    
                    partLengths[0] = 1;
                    partLengths[1] = 1;
                    partLengths[2] = 1;
                    restartMusic();
                    }
                else if( mMainPanel->contains( mVolumePanel ) ) {
                    mMainPanel->remove( mCurrentPanel );
                    mMainPanel->add( mSelectPanel );
                    mCurrentPanel = mSelectPanel;
                    
                    // test sounds off
                    for( int i=0; i<N-1; i++ ) {
                        noteToggles[i/5][0][i%5][i] = false;
                        }
                    noteToggles[2][0][5][15] = false;

                    partLengths[0] = 0;
                    partLengths[1] = 0;
                    partLengths[2] = 0;
                    }
                }
            }
        else if( mMainPanel->contains( mSelectPanel ) ) {
            
            if( !hardToQuitMode ) {
                // q or escape
                if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                    exit( 0 );
                    }
                }
            else {
                // # followed by ESC
                if( lastKeyPressed == '#' && inKey == 27 ) {
                    exit( 0 );
                    }
                lastKeyPressed = inKey;
                }
    
            if( inKey != 'q' && inKey != 'Q' && inKey != 27 && inKey != '#' ) {
                // any other key press triggers selection
                processSelection( mSelectHighlightIndex );
                }
            }
        else {
            if( mMainPanel->contains( mWriteFailedPanel ) ) {
                if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                    // q or escape
                    exit( 0 );
                    }
                }

            
            if( mMainPanel->contains( mDemoCodePanel ) ) {
                // look for enter
                if( inKey == 13 && mEnterDemoCodeField->isFocused() ) {
                    
                    // don't destroy this
                    char *enteredCode = mEnterDemoCodeField->getText();
                    
                    if( strlen( enteredCode ) > 0 ) {
                        // disable further input
                        mEnterDemoCodeField->setEnabled( false );
                        mEnterDemoCodeField->setFocus( false );
                        mEnterDemoCodeField->lockFocus( false );
                        
                        setLabelString( mEnterDemoCodeLabel, 
                                        "checkingCode" );
                        
                        // save this for next time
                        SettingsManager::setSetting( "demoCode",
                                                     enteredCode );

                        // start 
                        codeChecker = new DemoCodeChecker( enteredCode );
                        }
                    }
                else if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                    // q or escape
                    exit( 0 );
                    }
                }


            if( mMainPanel->contains( mJoinControllerPanel ) ) {
                // look for enter
                if( inKey == 13 && mEnterAddressField->isFocused() ) {
                    
                    // don't destroy this
                    char *enteredAddress = mEnterAddressField->getText();
                    
                    if( strlen( enteredAddress ) > 0 ) {
                        // disable further input
                        mEnterAddressField->setFocus( false );
                        mEnterAddressField->lockFocus( false );
                        mEnterAddressField->setEnabled( false );

                        
                        setLabelString( mEnterAddressLabel, 
                                        "joiningController" );
                        
                        // start as client
                        connection = new Connection( enteredAddress );
                        }
                    }
                }
            
            if( mMainPanel->contains( mWaitPlayerPanel ) ) {
                // look for "g" to go ahaead
                if( inKey == 'g' || inKey == 'G' ) {
            
                    mMainPanel->remove( mCurrentPanel );
                    mMainPanel->add( mLoadingPanel );
                    mCurrentPanel = mLoadingPanel;
                    isControllerGame = true;
                    loadingDrawnOnce = false;
                    
                    /*
                    game = new ControllerGame( mScreen );
                    isControllerGame = true;
                    
                    // hide menu gui to start the game
                
                    mMainPanel->remove( mCurrentPanel );
                    mCurrentPanel = NULL;
                    
                    mScreen->removeSceneHandler( mMainPanelGuiTranslator );
                    mScreen->removeKeyboardHandler( mMainPanelGuiTranslator );
                    mGUIVisible = false;


                    // add game handlers instead
                    mScreen->addSceneHandler( game );
                    mScreen->addKeyboardHandler( game );
                    mScreen->addMouseHandler( game );
                    mScreen->addRedrawListener( game );

                    game->setScreen( screen );
                    */
                    }
                }
            

        
            // q or escape pressed in any sub-panel
            if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
                
                // back up

                // wait for this connection to finish before destroying it

                if( connection != NULL ) {
                    oldConnections.push_back( connection );
                    connection = NULL;
                    }
                    
                mMainPanel->remove( mCurrentPanel );

                if( upnpPortOpen && mCurrentPanel == mWaitPlayerPanel ) {
                    // close upnp port first
                    AppLog::info( "Setting upnp panel\n" );
                    mMainPanel->add( mUPNPPanel );
                    mCurrentPanel = mUPNPPanel;

                    setLabelString( mUPNPLabel, "upnpCloseTrying" );
                    
                    // show upnp panel at least once before blocking
                    tryUPNPClose = false;
                    }
                else {
                    mMainPanel->add( mSelectPanel );
                    mCurrentPanel = mSelectPanel;
                    }
                
                }    
            }

            
        }
    
        

    /*
    // q or escape
    if( inKey == 'q' || inKey == 'Q' || inKey == 27 ) {
        if( !showingQuitQuestion ) {
            oldStatusText = stringDuplicate( mStatusLabel->getText() );
            
            showingQuitQuestion = true;
            
            setLabelString( mStatusLabel, "quitQuestion" );
            }
        }
    // answer to quit question?
    else if( showingQuitQuestion && ( inKey == 'y' || inKey == 'Y' ) ) {
        delete [] oldStatusText;
        oldStatusText = NULL;
        
        exit( 0 );
        }
    else if( showingQuitQuestion && ( inKey == 'n' || inKey == 'N' ) ) {
        setLabelString( mStatusLabel, oldStatusText );

        delete [] oldStatusText;
        oldStatusText = NULL;
        
        showingQuitQuestion = false;
        }
    */
    }



void GameSceneHandler::keyReleased(
	unsigned char inKey, int inX, int inY ) {

    }



void GameSceneHandler::specialKeyPressed(
	int inKey, int inX, int inY ) {

    /*
    // keep key states for later, whether we act on them now or not
    switch( inKey ) {
        case MG_KEY_LEFT:
            mLeftKeyDown = true;
            break;
        case MG_KEY_RIGHT:
            mRightKeyDown = true;
            break;
        }
    */


    switch( inKey ) {
        case MG_KEY_UP:
            if( mGUIVisible &&
                mMainPanel->contains( mSelectPanel ) ) {
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( false );
                
                mSelectHighlightIndex --;
                if( mSelectHighlightIndex < 0 ) {
                    mSelectHighlightIndex = 2;
                    }
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( true );
                }
            break;
        case MG_KEY_DOWN:
            if( mGUIVisible &&
                mMainPanel->contains( mSelectPanel )  ) {
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( false );
                
                mSelectHighlightIndex ++;
                if( mSelectHighlightIndex > 2 ) {
                    mSelectHighlightIndex = 0;
                    }
                mSelectLabels[ mSelectHighlightIndex ]->setHighlight( true );
                }
            break;
        }

	}



void GameSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {

    /*
    // keep key states for later, whether we act on them now or not
    switch( inKey ) {
        case MG_KEY_LEFT:
            mLeftKeyDown = false;
            break;
        case MG_KEY_RIGHT:
            mRightKeyDown = false;
            break;
        }
    */
	} 



void GameSceneHandler::actionPerformed( GUIComponent *inTarget ) {
    
    if( inTarget == mVolumeSlider ) {
        mVolumeTarget = mVolumeSlider->getThumbPosition();
        }
    else if( inTarget == mSelectLabels[0] ) {
        processSelection( 0 );
        }
    else if( inTarget == mSelectLabels[1] ) {
        processSelection( 1 );
        }
    else if( inTarget == mSelectLabels[2] ) {
        processSelection( 2 );
        }    
    }



void GameSceneHandler::processSelection( int inSelection ) {

    GUIPanelGL *panelToShow = NULL;
            
    switch( inSelection ) {
        case 0:
            panelToShow = mWaitPlayerPanel;
            mEnterAddressField->setFocus( false );
            mEnterAddressField->lockFocus( false );
            mEnterAddressField->setEnabled( false );
            
            setLabelString( mWaitPlayerLabel, "waitingForPlayer" );
            setLabelString( mWaitPlayerMessageLabel, "" );
            mPortTipLabel->setEnabled( true );
            
            // start as server
            connection = new Connection();

            break;
        case 1:
            mEnterAddressField->setFocus( false );
            mEnterAddressField->lockFocus( false );
            mEnterAddressField->setEnabled( false );

            if( upnpPortOpen ) {
                // already open, maybe from failed close before?
                            
                panelToShow = mWaitPlayerPanel;
                        
                setLabelString( mWaitPlayerLabel, 
                                "waitingForPlayer" );
                setLabelString( mWaitPlayerMessageLabel, "" );
                
                // start as server
                connection = new Connection();
                }
            else {
                // open it
                panelToShow = mUPNPPanel;
                            
                            
                setLabelString( mUPNPLabel, "upnpTrying" );
                            
                // wait until panel draws at least once
                tryUPNP = false;
                }
            break;
        case 2:
            panelToShow = mJoinControllerPanel;
                    
            setLabelString( mEnterAddressLabel, "enterAddress" );
            mEnterAddressField->setFocus( true );
            mEnterAddressField->lockFocus( true );
            mEnterAddressField->setEnabled( true );

            // don't pass this key press through to field
            mEnterAddressField->ignoreNextKey();
            break;
        }
            
    if( panelToShow != NULL ) {
        mMainPanel->remove( mSelectPanel );
        mMainPanel->add( panelToShow );
        mCurrentPanel = panelToShow;
        }
    }

    



