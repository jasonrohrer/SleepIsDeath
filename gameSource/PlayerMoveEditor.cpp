#include "PlayerMoveEditor.h"
#include "labels.h"

#include "minorGems/util/TranslationManager.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/util/log/AppLog.h"
#include "minorGems/io/file/File.h"
#include "minorGems/io/file/FileOutputStream.h"
#include "minorGems/graphics/converters/PNGImageConverter.h"

#include "minorGems/system/Time.h"


#include <stdio.h>




extern int gameWidth, gameHeight;


extern TextGL *largeTextFixed;




PlayerMoveEditor::PlayerMoveEditor( ScreenGL *inScreen )
        : Editor( inScreen, true, false ),  // no side panel
          mMovesDisabled( false ) {
    
    AppLog::info( "Constructing player move editor\n" );
    
    // hide the close button for this editor, since it's the base editor
    mCloseButton->setEnabled( false );



    mStateDisplay = new GameStateDisplay( (gameWidth - G*P)/2, 
                                          gameWidth - 48 - G * P );
    mMainPanel->add( mStateDisplay );
    mStateDisplay->addActionListener( this );
    
    // always hold focus, since there are no other keyboard-enabled
    // components on screen
    mStateDisplay->setFocus( true );
    mStateDisplay->lockFocus( true );
    


    mToolSet = new MoveToolSet( mStateDisplay->getAnchorX(), 42 );
    mMainPanel->add( mToolSet );    
    mToolSet->addActionListener( this );
    


    mSpeechDeleteButton = new DeleteButtonGL( 
        mToolSet->getAnchorX() + mToolSet->getWidth(),
        mToolSet->getAnchorY() + ( mToolSet->getHeight() - 8 ) / 2,
        8,
        8 );
    mMainPanel->add( mSpeechDeleteButton );
    mSpeechDeleteButton->addActionListener( this );

    mSpeechDeleteButton->setEnabled( false );
    
    mSpeechDeleteButton->setToolTip( "tip_delete_playerSpeech" );
    


    mActionDeleteButton = new DeleteButtonGL( 
        mToolSet->getAnchorX() - 8,
        mToolSet->getAnchorY() + ( mToolSet->getHeight() - 8 ) / 2,
        8,
        8 );
    mMainPanel->add( mActionDeleteButton );
    mActionDeleteButton->addActionListener( this );

    mActionDeleteButton->setEnabled( false );

    mActionDeleteButton->setToolTip( "tip_delete_playerAction" );    



    mTipDisplay = new FixedTipDisplay( mToolSet->getAnchorX() +
                                         mToolSet->getWidth() + 20, 
                                       48 );
    mMainPanel->add( mTipDisplay );


    mGameStateToEdit = NULL;

    GameState *state = new GameState();
    
    setGameStateToEdit( state );
    
    mStateDisplay->showGrid( false );


    
    mSendButton = new SendButtonGL(
        4,
        44, 
        16, 16 );
    
    mMainPanel->add( mSendButton );
    
    mSendButton->addActionListener( this );
    
    mSendButton->setToolTip( "tip_playerSend" );


    mPracticeStopButton = new SpriteButtonGL(
        new Sprite( "practiceStop.tga", true ),
        1,
        300,
        44, 
        16, 16 );
    
    mMainPanel->add( mPracticeStopButton );
    
    mPracticeStopButton->addActionListener( this );
    
    mPracticeStopButton->setToolTip( "tip_practiceStop" );
    mPracticeStopButton->setEnabled( false );
    



    mFlipBookButton = new SelectableButtonGL( 
        new Sprite( "flipBook.tga", true ),
        1,
        298, 42, 20, 20 );
    
    mMainPanel->add( mFlipBookButton );
    // hide for now, don't let player turn flip books off (except manually
    // in settings folder)
    mFlipBookButton->setEnabled( false );
    
    
    char flipBookFound = false;
    int readFlipBook = SettingsManager::getIntSetting( "flipBook", 
                                                       &flipBookFound );
    
    char flipBook = false;
    
    if( flipBookFound && readFlipBook == 1 ) {
        flipBook = true;
        }

    mFlipBookButton->setSelected( flipBook );
    
    mFlipBookButton->addActionListener( this );
    
    mFlipBookButton->setToolTip( "tip_flipBook" );
    

    mFlipBookImageNumber = 1;
    
    File flipBookDir( NULL, "flipBooks" );
    
    if( !flipBookDir.exists() ) {
        flipBookDir.makeDirectory();
        }
    
    if( flipBookDir.exists() && flipBookDir.isDirectory() ) {
        
        int numFiles;
        File **childFiles = flipBookDir.getChildFiles( &numFiles );

        int largestNumber = 0;
        
        for( int i=0; i<numFiles; i++ ) {
            
            if( childFiles[i]->isDirectory() ) {
                
                char *name = childFiles[i]->getFileName();
                
                int number;
                
                int numRead = sscanf( name, "%d", &number );
                
                if( numRead == 1 ) {
                    
                    if( number > largestNumber ) {
                        largestNumber = number;
                        }
                    }
                delete [] name;
                }
            delete childFiles[i];
            }
        delete [] childFiles;

        mFlipBookFolderNumber = largestNumber + 1;        
        }
    else {
        mFlipBookFolderNumber = -1;
        }
    
    postConstruction();
    }



PlayerMoveEditor::~PlayerMoveEditor() {
    
    if( mGameStateToEdit != NULL ) {
        delete mGameStateToEdit;
        }
    
    }



void PlayerMoveEditor::setGameStateToEdit( GameState *inGameState ) {
    if( mGameStateToEdit != NULL ) {
        delete mGameStateToEdit;
        }
    
    mGameStateToEdit = inGameState;

    
    mStateDisplay->setState( mGameStateToEdit->copy() );


    
    if( !mMovesDisabled ) {    
        // make sure speech toggles match current state
        mStateDisplay->mEditingSpeech =
            (  mToolSet->getSelected() == playerSpeak );
        }
    
    if( mGameStateToEdit->getSelectedSpeechLength() > 0 ) {
        mSpeechDeleteButton->setEnabled( true );
        }
    else {
        mSpeechDeleteButton->setEnabled( false );
        }

    if( !mMovesDisabled ) {
        // action toggles too
        mStateDisplay->mEditingAction =
            (  mToolSet->getSelected() == playerAct );
        }
    
    if( mGameStateToEdit->getSelectedActionLength() > 0 ) {
        mActionDeleteButton->setEnabled( true );
        }
    else {
        mActionDeleteButton->setEnabled( false );
        }

    
    if( !mMovesDisabled ) {
        mToolSet->setMoveAllowed( ! mGameStateToEdit->isObjectZeroFrozen() );
        }
    }




void PlayerMoveEditor::setMovesDisabled( char inDisabled ) {

    char oldDisabled = mMovesDisabled;
    
    mMovesDisabled = inDisabled;
    
    mToolSet->setEnabled( !mMovesDisabled );

    if( !mMovesDisabled && oldDisabled ) {
        // just enabled
        
        // force back to MOVE mode
        mToolSet->setSelected( playerMove );
        
        mTipDisplay->setTip( 
            (char*)TranslationManager::translate( "instruction_playerMove" ) );
        
        mToolSet->setMoveAllowed( ! mGameStateToEdit->isObjectZeroFrozen() );
        }
    else if( mMovesDisabled && !oldDisabled ) {
        // just disabled
        mSpeechDeleteButton->setEnabled( false );
        mActionDeleteButton->setEnabled( false );

        // hide any empty actions or speech
        mStateDisplay->mEditingSpeech = false;
        mStateDisplay->mEditingAction = false;
        
        // hide instruction tip
        mTipDisplay->setTip( NULL );
        }
    }



void PlayerMoveEditor::clearNonPlayerSpeech() {
    mGameStateToEdit->deleteAllNonPlayerSpeech();

    // no need to redo all sprites
    mStateDisplay->updateSpritePositions( 
        mGameStateToEdit->copy() );
    }


extern char practiceMode;
extern char practiceStop;


void PlayerMoveEditor::enableSend( char inEnabled ) {
    mSendButton->setEnabled( inEnabled );

    
    mPracticeStopButton->setEnabled( inEnabled && practiceMode );
        
    /*
    if( !inEnabled ) {
        mSendButton->resetPressState();
        }
    */
    }


void PlayerMoveEditor::actionPerformed( GUIComponent *inTarget ) {
    // superclass
    Editor::actionPerformed( inTarget );
    

    if( inTarget == mToolSet ) {
        // tool change?
        
        // if speech tool, switch keyboard to main display
        
        mStateDisplay->mEditingSpeech =
            (  mToolSet->getSelected() == playerSpeak );

        if( mGameStateToEdit->getSelectedSpeechLength() > 0 ) {
            mSpeechDeleteButton->setEnabled( true );    
            }
        else {
            mSpeechDeleteButton->setEnabled( false );
            }        
        
        char wasEditingAction = mStateDisplay->mEditingAction;
        

        mStateDisplay->mEditingAction =
            (  mToolSet->getSelected() == playerAct );

        if( mGameStateToEdit->getSelectedActionLength() > 0 ) {
            mActionDeleteButton->setEnabled( true );    
            }
        else {
            mActionDeleteButton->setEnabled( false );

            // if we're newly editing a blank action again, recenter it
            if( !wasEditingAction && mStateDisplay->mEditingAction ) {
                AppLog::detail( "Newly editing an action... clearing it\n" );
                
                mGameStateToEdit->resetActionAnchor( 
                    mGameStateToEdit->getSelectedObject() );

                mStateDisplay->updateSpritePositions( 
                    mGameStateToEdit->copy() );
                }
            }


        char *instructionTransKey;
        
        switch( mToolSet->getSelected() ) {
            case playerAct:
                instructionTransKey = (char*)"instruction_playerAct";
                break;
            case playerMove:
                instructionTransKey = (char*)"instruction_playerMove";
                break;
            case playerSpeak:
                instructionTransKey = (char*)"instruction_playerSpeak";
                break;
            }        
        
        mTipDisplay->setTip( 
            (char*)TranslationManager::translate( instructionTransKey ) );
        }
    else if( inTarget == mSpeechDeleteButton ) {
        mGameStateToEdit->deleteAllCharsFromSelectedSpeech();

        // no need to redo all sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );

        mSpeechDeleteButton->setEnabled( false );
        }
    else if( inTarget == mActionDeleteButton ) {
        mGameStateToEdit->deleteAllCharsFromSelectedAction();
        mGameStateToEdit->resetActionAnchor( 
            mGameStateToEdit->getSelectedObject() );
        

        // no need to redo all sprites
        mStateDisplay->updateSpritePositions( 
            mGameStateToEdit->copy() );

        mActionDeleteButton->setEnabled( false );
        }
    else if( inTarget == mFlipBookButton ) {
        // togglem
        mFlipBookButton->setSelected( ! mFlipBookButton->getSelected() );

        // save
        int flipBook = 0;
        if( mFlipBookButton->getSelected() ) {
            flipBook = 1;
            }
        
        SettingsManager::setSetting( "flipBook", flipBook );
        }
    else if( inTarget == mSendButton ) {
        mSendButton->setEnabled( false );
        fireActionPerformed( this );
        }
    else if( inTarget == mPracticeStopButton ) {
        // turn practice mode off
        practiceStop = true;
        mSendButton->setEnabled( false );
        
        // end practice player turn
        fireActionPerformed( this );
        }
    else if( inTarget == mStateDisplay && !mMovesDisabled ) {
        
        switch( mToolSet->getSelected() ) {
            case playerAct: {

                if( mStateDisplay->mLastActionKeyPress ) {
                    unsigned char key = mStateDisplay->mLastKeyPressed;
                    
                    if( key == 127 || key == 8 ) {
                        // backspace and delete                        
                        mGameStateToEdit->deleteCharFromSelectedAction();
                        }
                    else if( (key >= 32 && key <= 126)
                             || key >= 160 ) {
                        // allowed range, ascii and extended ascii
                        mGameStateToEdit->addCharToSelectedAction(
                            (char)key );
                        }   
                    if( mGameStateToEdit->getSelectedActionLength() > 0 ) {
                        mActionDeleteButton->setEnabled( true );
                        }
                    else {
                        mActionDeleteButton->setEnabled( false );
                        }

                    // no need to redo all sprites
                    mStateDisplay->updateSpritePositions( 
                        mGameStateToEdit->copy() );
                    }
                else {
                    // mouse movement with action tool
                    // adjust action anchor
                    
                    mGameStateToEdit->moveSelectedActionAnchor(
                        mStateDisplay->mLastPixelClickX,
                        mStateDisplay->mLastPixelClickY );

                    // no need to generate all new sprites
                    mStateDisplay->updateSpritePositions( 
                        mGameStateToEdit->copy() );
                    }
                                
                }
                break;
            case playerMove: {
                if( !mStateDisplay->mLastActionKeyPress ) {
                    // move even in response to mouse dragging
                    
                    int x = mStateDisplay->mLastGridClickX;
                    int y = mStateDisplay->mLastGridClickY;
                
                    if( ! mGameStateToEdit->mRoom.getWall( x, G - y - 1 ) ) {
                        
                        // move whole player object only
                        mGameStateToEdit->moveSelectedObject( x, y );
                        
                        // no need to generate all new sprites
                        mStateDisplay->updateSpritePositions( 
                            mGameStateToEdit->copy() );
                        }
                    }
                
                }
                break;
            case playerSpeak: {
                /*
                if( mStateDisplay->mLastActionKeyPress ) {
                    unsigned char key = mStateDisplay->mLastKeyPressed;
                    
                    if( key == 127 || key == 8 ) {
                        mGameStateToEdit->deleteCharFromSelectedSpeech();
                        }
                    else if( key >= 32 && key <= 126 ) {
                        mGameStateToEdit->addCharToSelectedSpeech(
                            (char)key );
                        }   
                    if( mGameStateToEdit->getSelectedSpeechLength() > 0 ) {
                        mSpeechDeleteButton->setEnabled( true );
                        }
                    else {
                        mSpeechDeleteButton->setEnabled( false );
                        }

                    // no need to redo all sprites
                    mStateDisplay->updateSpritePositions( 
                        mGameStateToEdit->copy() );
                    }
                else {
                    // mouse movement with speech tool

                    // ignore it!
                    }
                */
                }
                break;
            }


        
        if( mToolSet->getSelected() != playerAct ) {
            // typing adds to speech even in move mode
            if( mStateDisplay->mLastActionKeyPress ) {
                unsigned char key = mStateDisplay->mLastKeyPressed;
                
                if( key == 127 || key == 8 ) {
                    mGameStateToEdit->deleteCharFromSelectedSpeech();
                    }
                else if( ( key >= 32 && key <= 126 ) 
                         || key >= 160 ) {
                    // allowed range, ascii and extended ascii
                    mGameStateToEdit->addCharToSelectedSpeech(
                        (char)key );
                    }   
                if( mGameStateToEdit->getSelectedSpeechLength() > 0 ) {
                    mSpeechDeleteButton->setEnabled( true );
                    }
                else {
                    mSpeechDeleteButton->setEnabled( false );
                    }

                // no need to redo all sprites
                mStateDisplay->updateSpritePositions( 
                    mGameStateToEdit->copy() );
                }
            }
        
        
        }
    
    
    }



void PlayerMoveEditor::editorClosing() {
    }




extern int screenWidth;
extern int screenHeight;



static void writeHTMLFile( int inPageNumber, int inMaxPageNumber,
                      File *inDestDir ) {

    char *pageFileName = autoSprintf( "%05d.html", inPageNumber );

    File *thisPageFile = 
        inDestDir->getChildFile( pageFileName );

    delete [] pageFileName;
    
    // pure HTML output
                    
                    
    File templateDir( NULL, "templates" );
    
    File *tempFile = 
        templateDir.getChildFile( "x.html" );
    
    char *xText = tempFile->readFileContents();
    delete tempFile;
    
    tempFile = 
        templateDir.getChildFile( "prevButton.html" );
    
    char *prevText = tempFile->readFileContents();
    delete tempFile;
    
    tempFile = 
        templateDir.getChildFile( "nextButton.html" );
    
    char *nextText = tempFile->readFileContents();
    delete tempFile;
    
    tempFile = 
        templateDir.getChildFile( "preloadNext.html" );
    
    char *preloadText = tempFile->readFileContents();
    delete tempFile;


    char *xNumber = autoSprintf( "%05d", inPageNumber );

    // default to going back to beginning
    char *yNumber = autoSprintf( "%05d", 1 );

    if( inPageNumber < inMaxPageNumber ) {
        // there's a next button
        char *nextNumber = 
            autoSprintf( "%05d", inPageNumber + 1 );

        char found;
        
        char *temp = replaceOnce( nextText, "#Y",
                                  nextNumber,
                                  &found );
        delete [] nextText;
        nextText = temp;

        temp = replaceOnce( preloadText, "#Y",
                            nextNumber,
                            &found );
        delete [] preloadText;
        preloadText = temp;

        delete [] yNumber;
        yNumber = nextNumber;
        }
    else {
        // no next
        delete [] nextText;
        nextText = stringDuplicate( "" );
        
        delete [] preloadText;
        preloadText = stringDuplicate( "" );
        }


    if( inPageNumber > 1 ) {
        // there's a prev button
    
        char *prevNumber = 
            autoSprintf( "%05d", inPageNumber - 1 );

        char found;
        
        char *temp = replaceOnce( prevText, "#W",
                                  prevNumber,
                                  &found );
        delete [] prevText;
        prevText = temp;

        delete [] prevNumber;
        }
    else {
        // no prev
        delete [] prevText;
        prevText = stringDuplicate( "" );
        }

    SimpleVector<char *> targets;
    SimpleVector<char *> subs;

    targets.push_back( (char*)"#X" );
    subs.push_back( xNumber );

    targets.push_back( (char*)"#Y" );
    subs.push_back( yNumber );

    targets.push_back( (char*)"#PREV" );
    subs.push_back( prevText );

    targets.push_back( (char*)"#NEXT" );
    subs.push_back( nextText );

    targets.push_back( (char*)"#PRELOAD" );
    subs.push_back( preloadText );
    
                       

    char *finalText = replaceTargetListWithSubstituteList(
        xText, &targets, &subs );
    

    delete [] xText;
    delete [] prevText;
    delete [] nextText;
    delete [] preloadText;
    delete [] yNumber;
    delete [] xNumber;
    
    thisPageFile->writeToFile( finalText );
    
    delete thisPageFile;
    delete [] finalText;
    }



// to map screen coordinates to take same screen shot regardless of window
// resolution
extern GUITranslatorGL *guiTranslator;

extern int pixelZoomFactor;



void PlayerMoveEditor::saveFlipBookImage() {

    if( mFlipBookButton->getSelected() &&
        mFlipBookFolderNumber > 0 ) {
        
        
        
        File flipBookDir( NULL, "flipBooks" );
    
    
        if( flipBookDir.exists() && flipBookDir.isDirectory() ) {
            char *thisBookName = autoSprintf( "%05d", mFlipBookFolderNumber );
            
            File *thisBookDir = flipBookDir.getChildFile( thisBookName );
            
            delete [] thisBookName;
            
        
            if( ! thisBookDir->exists() ) {
                thisBookDir->makeDirectory();
                
                // copy PHP templates into place
                
                File templateDir( NULL, "templates" );
                
                const char *namesToCopy[6] = { "header.php", "footer.php", 
                                               "index.php", "index.html",
                                               "next.png", "prev.png" };
                
                for( int i=0; i<6; i++ ) {
                    char success = false;
                    
                    File *templateFile = 
                        templateDir.getChildFile( namesToCopy[i] );
                    
                    if( templateFile->exists() ) {
                            
                        File *destFile = 
                            thisBookDir->getChildFile( namesToCopy[i] );
                            
                        if( destFile != NULL ) {
                            templateFile->copy( destFile );
                            success = true;
                            delete destFile;
                            }    
                        }

                    if( !success ) {
                        AppLog::getLog()->logPrintf( 
                            Log::ERROR_LEVEL,
                            "Failed to copy Flip Book template file %s\n",
                            namesToCopy[i] );
                        }
                        

                    delete templateFile;
                    }  
      

                }
            

            if( thisBookDir->exists() && thisBookDir->isDirectory() ) {    
                
                
                

                // NOTE:  this is a bit of a hack
                // the GUI coordinate trans only can map from screen to 
                // GUI space.  We have GUI space coords, and we want to
                // map to screen space to trim our screen shot.
                
                // SO, hack is just too loop over all possible values, 
                // translating each one until we find a match.

                // This is actually fast enough (especially compared to the
                // image conversion/processing), even though it seems wasteful
                // Not as wasteful (and bug-prone) as me trying to write
                // inverse coordinate translation functions.

                int w = screenWidth;
                int h = screenHeight;
                
                int yBottom = (int)mStateDisplay->getAnchorY();
                
                int hSkip = 0;
                while( guiTranslator->translateY( screenHeight - hSkip ) 
                       < yBottom ) {
                    hSkip ++;
                    }

                if( pixelZoomFactor > 1 && pixelZoomFactor % 2 != 0 ) {
                    hSkip -= 1;
                    }
                
                int yTop = yBottom + (int)mStateDisplay->getHeight();
                

                int hTop = hSkip;
                
                while( guiTranslator->translateY( screenHeight - hTop ) 
                       < yTop ) {
                    hTop ++;
                    }
                
                if( pixelZoomFactor > 1 && pixelZoomFactor % 2 != 0 ) {
                    hTop -= 1;
                    }
                
                h = hTop - hSkip;
                
                AppLog::getLog()->logPrintf( 
                    Log::DETAIL_LEVEL,
                    "H to grab = %d, skipping %d\n", h, hSkip );



                int xLeft = 0;
                
                int wSkip = 0;
                while( guiTranslator->translateX( wSkip ) < xLeft ) {
                    wSkip ++;
                    }
                int xRight = gameWidth;
                
                int wRight = wSkip;
                
                while( guiTranslator->translateX( wRight ) < xRight ) {
                    wRight ++;
                    }
                
                if( pixelZoomFactor > 1 && pixelZoomFactor % 2 != 0 ) {
                    wRight -= 1;
                    }
                
                w = wRight - wSkip;
                
                AppLog::getLog()->logPrintf( 
                    Log::DETAIL_LEVEL,
                    "W to grab = %d, skipping %d\n", w, wSkip );


                unsigned char *rgbBytes = new unsigned char[ w * h * 3 ];

                // w and h might not be multiples of 4
                int oldAlignment;
                glGetIntegerv( GL_PACK_ALIGNMENT, &oldAlignment );
                //printf( "Old alignment = %d\n", oldAlignment );
                
                glPixelStorei( GL_PACK_ALIGNMENT, 1 );
                
                glReadPixels( wSkip, hSkip, w, h, 
                              GL_RGB, GL_UNSIGNED_BYTE, rgbBytes );

                glPixelStorei( GL_PACK_ALIGNMENT, oldAlignment );
                

                int baseWidth = 320;
                int baseHeight = 208;
                
                // pixel doubling
                int pageMultiple = 2;
                int pageWidth = baseWidth * pageMultiple;
                int pageHeight = baseHeight * pageMultiple;
                
                
                int screenMultiple;
                
                char resizeFailure = false;
                

                // make sure w and h are even multiples of the base
                if( h % baseHeight != 0 ||
                    w % baseWidth != 0 ) {
                    
                    AppLog::getLog()->logPrintf( 
                            Log::ERROR_LEVEL,
                            "Trimmed screen is not an even multiple of flip"
                            " book base size: base=%dx%d, trimmed screen="
                            " %dx%d\n", baseWidth, baseHeight, w, h );

                    resizeFailure = true;
                    }
                else {    
                    screenMultiple = h / baseHeight;
                    
                    if( screenMultiple != w / baseWidth ) {
                        AppLog::error( 
                            "Error: flip book output:  multiple for screen"
                            " widht not equal to multiple for height" );
                        
                        resizeFailure = true;
                        }                    
                    }
                

                if( resizeFailure ) {
                    // default to actual size in this case, instead of 
                    // trying to shrink
                    pageWidth = w;
                    pageHeight = h;

                    baseWidth = w;
                    baseHeight = h;

                    pageMultiple = 1;
                    screenMultiple = 1;
                    }
                

                Image pageImage( pageWidth, pageHeight, 3, false );

                double *channels[3];
                int c;
                for( c=0; c<3; c++ ) {
                    channels[c] = pageImage.getChannel( c );
                    }

                // image of screen is upside down
                int outputRow = 0;
                for( int y=pageHeight-1; y>=0; y-- ) {
                    int screenY = (y / pageMultiple) * screenMultiple;
                    
                    for( int x=0; x<pageWidth; x++ ) {
                        
                        int outputPixelIndex = outputRow * pageWidth + x;
                        
                        int screenX = (x / pageMultiple) * screenMultiple;
                        

                        int screenPixelIndex = screenY * w + screenX;
                        int byteIndex = screenPixelIndex * 3;
                        
                        for( c=0; c<3; c++ ) {
                            channels[c][outputPixelIndex] =
                                rgbBytes[ byteIndex + c ] / 255.0;
                            }
                        }
                    outputRow++;
                    }
                
                delete [] rgbBytes;
                

                char *thisPNGName = 
                    autoSprintf( "%05d.png", mFlipBookImageNumber );

                char *thisPageName = 
                    autoSprintf( "%05d.html", mFlipBookImageNumber );

                
                // update list PHP file with latest count                
                File *listFile = 
                    thisBookDir->getChildFile( "frameList.php" );

                SimpleVector<char> listAccume;
                
                listAccume.appendElementString( 
                    "<?php $frameList = array( " );
                for( int p=1; p<=mFlipBookImageNumber; p++ ) {
                    char *frameString = autoSprintf( "\"%05d\"", p );
                    
                    listAccume.appendElementString( frameString );
                    
                    delete [] frameString;
                    
                    if( p!= mFlipBookImageNumber ) {
                        // not last, add comma between
                        listAccume.appendElementString( ", " );
                        }
                    }
                listAccume.appendElementString( " ); ?>" );
                
                char *listString = listAccume.getElementString();
                
                listFile->writeToFile( listString );
                delete [] listString;
                
                delete listFile;
                


                int thisImageNumber = mFlipBookImageNumber;
                

                mFlipBookImageNumber++;

                

                File *thisBookImagesDir = 
                    thisBookDir->getChildFile( "images" );

                if( ! thisBookImagesDir->exists() ) {
                    thisBookImagesDir->makeDirectory();    
                    }
                
                if( thisBookImagesDir->exists() && 
                    thisBookImagesDir->isDirectory() ) {
                    
                    AppLog::getLog()->logPrintf( 
                        Log::INFO_LEVEL,
                        "Flipbook output %s\n", thisPNGName );

                    File *thisPNGFile = 
                        thisBookImagesDir->getChildFile( thisPNGName );
                

                    FileOutputStream pageOutput( thisPNGFile );


                    PNGImageConverter png;

                    double t = Time::getCurrentTime();
                    
                    png.formatImage( &pageImage, &pageOutput );
                    
                    AppLog::getLog()->logPrintf( 
                        Log::DETAIL_LEVEL, "Converter took %f seconds\n", 
                        Time::getCurrentTime() - t );
                
                    
                    delete thisPNGFile;

                    
                    // now HTML templates

                    // new one
                    writeHTMLFile( thisImageNumber, 
                                   thisImageNumber,
                                   thisBookImagesDir );
                    
                    if( thisImageNumber > 1 ) {
                        // redo last one, since there's a "next" now
                        writeHTMLFile( thisImageNumber - 1,
                                       thisImageNumber,
                                       thisBookImagesDir );
                        }
                                        
                    }
                delete [] thisPNGName;
                delete [] thisPageName;

                delete thisBookImagesDir;
                }
            
            
            delete thisBookDir;
            }
        
        
            
        }
    
    }

