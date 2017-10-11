<?php

// no caching
header('Cache-Control: no-store, no-cache, must-revalidate');
header('Cache-Control: post-check=0, pre-check=0', FALSE);
header('Pragma: no-cache');

error_reporting( E_ALL );


include( "header.php" );


$deadline = "2010-04-08 23:59:59";
//$deadline = "2010-02-19 10:06:59";


?>





<?php

function showDownloadForm() {
    ?>
      Access your download:<br>
            <FORM ACTION="ticketServer/server.php" METHOD="post">
    <INPUT TYPE="hidden" NAME="action" VALUE="show_downloads">
      Enter Code:   
    <INPUT TYPE="text" NAME="ticket_id" MAXLENGTH=25 SIZE=12>
    <INPUT TYPE="Submit" VALUE="Next">
    </FORM>
      
<?php
    }



function showPayLinks( $inSimple ) {
    ?>
 <center>
      <a href="https://sites.fastspring.com/jasonrohrer/instant/sleepisdeathticket">
      <font size=5>Buy downloads</font><br>
      <img src="fs_cards.png" width=280 height=45 border=0><?php
      if( !$inSimple ) {

          echo "<br>";
          
          echo "<img src=\"fs_button05.png\" width=210 height=60 border=0>";
          }
?>
      </a>
      </center>
<?php
    }



function showPayForm( $inSimple ) {
    $referrer = "";
    
    if( isset( $_SERVER['HTTP_REFERER'] ) ) {

        // pass it through without a regex filter
        // because we can't control its safety in the end anyway
        // (user can just edit URL sent to FastSpring).

        // don't URL encode, because it's fed into a FORM variable below
        $referrer = $_SERVER['HTTP_REFERER'];
        }
    
    ?>
<center><table border=0><tr><td> 
<ul> 
      <li>Unlimited downloads for two people
      <li>Access to all future updates
      <li>Tech support included
      <li>Support me and my family directly<br>(so I can make more games)
      </ul>
</td></tr></table></center>                                          
      <font size=6>What's it worth to you?</font><br><br>                           
 <center>
      
      <form action="nameYourPrice.php" METHOD="post">
      Name your price: $<input type="text" name="price" VALUE="17.00" SIZE=7 MAXLENGTH=20><br>
      <input type="hidden" name="referrer" VALUE="<?php echo $referrer;?>">
      <input type="image" src="fs_cards.png" width=280 height=45 title="Buy Downloads"><br>
      <input type="image" src="fs_button05.png" width=210 height=60 title="Buy Downloads">
      </form>
      </center>
<?php
    }




function showPayFormFixedPrice( ) {
    $referrer = "";
    
    if( isset( $_SERVER['HTTP_REFERER'] ) ) {

        // pass it through without a regex filter
        // because we can't control its safety in the end anyway
        // (user can just edit URL sent to FastSpring).
                                          
        $referrer = urlencode( $_SERVER['HTTP_REFERER'] );
        }
    
    ?>
<center><table border=0><tr><td> 
<ul> 
      <li>Unlimited downloads for two people
      <li>Access to all future updates
      <li>Tech support included
      <li>Support me and my family directly<br>(so I can make more games)
      </ul>
</td></tr></table></center>                                          
      <font size=6>Available now for $14</font><br><br>                           
 <center>
      <a href="https://sites.fastspring.com/jasonrohrer/instant/sleepisdeathticket?referrer=<?php echo $referrer;?>">
      <img src="fs_cards.png" width=280 height=45 border=0><br>
      <img src="fs_button05.png" width=210 height=60 border=0></a>
      
      </center>
<?php
    }



function getTimeLeft() {
    global $deadline;
    
    
    date_default_timezone_set( "America/New_York" );

    $deadlineTimestamp = strtotime( $deadline );

    $currentTimestamp = time();

    
    if( $currentTimestamp < $deadlineTimestamp ) {
        return $deadlineTimestamp - $currentTimestamp;
        }
    else {
        return 0;
        }
    }



function showTimeLeft( $inTimeLeft ) {
            
    $d = $inTimeLeft;

    $hours = (int)( $d / 3600 );

    $seconds = (int)( $d % 3600 );
    $minutes = (int)( $seconds / 60 );
    $seconds = (int)( $seconds % 60 );
            
    $days = (int)( $hours / 24 );
    $hours = (int)( $hours % 24 );
            
        
    echo "Only <b>";

    if( $days > 1 ) {
        echo "$days days";
        }
    else {
        $hours += 24 * $days;
            
        if( $hours > 1 ) {
            echo "$hours hours";
            }
        else {
            $minutes += 60 * $hours;

            if( $minutes > 1 ) {
                echo "$minutes minutes";
                }
            else {
                $seconds += 60 * $minutes;

                echo "$seconds seconds";
                }
            }
        }
    echo "</b> left to pre-order";
    }



function showLogo( $inImageFile, $inText ) {

    echo "<table border=0><tr><td align=center>
          <img src=\"$inImageFile\"><br>
          <font size=1>$inText</font>
          </td></tr></table>";
    }


?>


<center>
<font size=6>Sleep Is Death (Geisterfahrer)</font><br>
a storytelling game for two players by
<a href="http://hcsoftware.sf.net/jason-rohrer">Jason Rohrer</a><br>
<br>

[<a href="news.php">news</a>] -- 
[<a href="videos.php">videos</a>] --  
[<a href="community.php">community</a>] -- 
[<a href="myStories.php">jason's stories</a>] -- 
[<a href="stories.php">other stories</a>]
</center>



<center>

<table border=0 width=75%>
<tr>

<td valign=top align=center width=50%>
<font size=7>90/100</font><br>
<b>Editor's Choice</b><br>
PC Gamer UK<br>
June 2010
</td>

<td valign=top align=center width=50%>
<font size=7>5/5</font><br>
Excellent<br>
<a href="http://gamernode.com/reviews/9088-sleep-is-death/index.html">GamerNode</a>
</td>

</tr>
</table>

<br>

<font size=5>
"I had stared right into the eye of the future,<br>
and was left with nothing to do about it but<br>
wait for the rest of the industry to catch up."</font><br>
--Justin McElroy, <a href="http://www.joystiq.com/2010/04/05/hands-on-sleep-is-death">Joystiq Preview</a><br><br>

<!--
<br>

<font size=5>
"A really nice system.</font><br>
Simple enough for casual players to dabble with, but with enough<br>
underlying features for master storytellers to have a field day"<br>
--Mike Rose, <a href="http://www.indiegames.com/blog/2010/04/preview_sleep_is_death_jason_r.html">IndieGames Preview</a><br><br>



<font size=5>"As much as I enjoyed playing Sleep is Death<br>
as a player in Jason Rohrer's story,<br>
I enjoyed it even more as a Controller."</font><br>
--Anthony Burch, <a href="http://www.destructoid.com/preview-sleep-is-death-controller-mode--169556.phtml">Destructoid Preview 2</a> (Controller side)<br><br>

<br>
-->

"We already have movies, yes.
We already even have plenty of video games.<br>
<font size=6>We've never had anything like this."</font><br>
--Leigh Alexander, <a href="http://kotaku.com/5507753/a-video-game-death-made-for-me">Kotaku Preview</a><br><br>


<font size=5>"No other videogame has offered me so much."</font><br>
--Anthony Burch, Destructoid Previews [<a href="http://www.destructoid.com/preview-sleep-is-death-player-mode--169272.phtml">1</a>] [<a href="http://www.destructoid.com/preview-sleep-is-death-controller-mode--169556.phtml">2</a>]<br><br>



<font size=5>"In this blocky technical artifact, I discovered something<br>
alarmingly dark, personal, and beautiful.<br>
And we had made it together."</font> <br>--Michael Thomsen, <a href="http://pc.ign.com/articles/107/1079328p1.html">IGN.com Preview</a><br><br>

<!--
<font size=5>"It was acting on parts of my brain that I'd<br>
never used in a game environment before"</font> <br>--Michael Thomsen, <a href="http://pc.ign.com/articles/107/1079328p1.html">IGN Preview</a><br><br>
-->

<font size=5>"Something that no game has done to me before"</font> <br>--Brandon Boyer, <a href="http://boingboing.net/features/rohrer.html">In-depth preview on Boing Boing</a><br><br>

<font size=5>"Incredibly awesome"</font> --<a href="http://www.flickr.com/photos/rich_lem/4334191744/">Richard Lemarchand</a> (Lead Designer, Uncharted 2)
</center>
<br>
<br>

<center>
<a href="slideShow"><img src="slideShowLead.png" width="144" height="208" border=0></a>

<!-- preload everythign for the slide show -->

<img src="slideShow/next.png" width="0" height="0">
<img src="slideShow/prev.png" width="0" height="0">
<img src="slideShow/1.png" width="0" height="0">

</center>

<br>




<a name="order"></a>
<font size=5>Two for One</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
When you buy the game, you're buying it for two people.  You can share your download link with a friend, as a gift.
<br>
<br>
This is v16, the <b>official public release</b>.



<!--
<a name="order"></a>
<font size=5>Name Your Price</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
You and a friend (two for the price of one---see below) can download the game right now for <b>whatever you can afford to pay for it</b>.<br>
<br>

Note that the minimum price is $1.75 (to cover payment processing fees and download bandwidth).<br>
<br>
This is v16, the <b>official public release</b>.



<font size=5>How to get the game (</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
You and a friend (two for the price of one---see below) can download the game right now for <b>$7</b>.  This is v14, the <b>official public release</b>.
-->
<!--
You can wait until the release to buy the game for <b>$14</b>.  You can also pre-order the game right now for <b>$9</b>.    By pre-ordering, along with paying less, you will get the game one week earlier:<br>
<br>
<center>
<table border=0 cellpadding=4 cellspacing=1>
<tr><td></td><td align=right>$14.00 US</td><td> - </td><td>Download on or after Friday, April 16, 2010</td></tr>
<tr><td colspan=4 align=center></td></tr>
<tr><td>(pre-order)</td><td align=right>$9.00 US</td><td> - </td><td>Download on Friday, April 9, 2010</td></tr>
</table>
</center>
-->
</td></tr></table>

<center>
<table border=0><tr>
<td><?php showLogo( "noDRM.png", "No DRM" ); ?></td>
<td><?php showLogo( "noTie.png", "No middle-person" ); ?></td>
<td><?php showLogo( "crossPlatform.png", "Cross-platform" ); ?></td>
<td><?php showLogo( "openSource.png", "Open Source" ); ?></td>
</tr></table>                                  
</center>                          
                                  

<center>
<table border=0 cellpadding=2><tr><td bgcolor="#222222">
<table border=0 cellpadding=5><tr><td bgcolor="#000000">
<center>                         
<font size=5 color=red>
<?php
/*
$timeLeft = getTimeLeft();
if( $timeLeft > 0 ) {
    
    showTimeLeft( $timeLeft );
    }
else {
    echo "Pre-Orders are now closed";
    }
*/
?>
</font>
</center>

<?php
   //showPayLinks( false );
//showPayForm( false );
showPayFormFixedPrice();
/*
if( $timeLeft > 0 ) {
    echo "<br>";
    // only credit card button
    showPayLinks( false );
    }
else {
    //echo "<br>";
    //echo "<center>Downloads will be availble soon.</center>";
    
    showDownloadForm();

    echo "<br><center><font size=5>Orders for April 16 are open</font></center>";

    echo "<br>";
    // only credit card button
    showPayLinks( false );
    }
*/
?>
</td></tr></table>
</td></tr></table>
</center>
<br>
<br>


<font size=5>What you get</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
<!-- <i>Two for the price of one</i><br>
<br>
-->
Immediately after your payment is processed, you will receive an email with an access link for <b>two</b> people (the game is two-player only).  You and a friend will then be able to download all of the following DRM-free distributions:
<center>
<table border=0><tr><td>
<ul>
<li>Windows build</li>
<li>MacOS build (10.2 and later, PPC/Intel)</li>
<li>Full source code bundle (which can also be compiled on GNU/Linux)</li>
<li>Several resource packs, including one by Spore artist <a href="galvin.php">Shannon Galvin</a></li>
</ul>
</td></tr></table>
</center>
The price also includes downloads of all future updates.<br>
<br>
You can take a look at the <a href="requirements.php">system requirements</a>.
</td></tr></table>
<br>
<br>

<center>
<table border=0 cellpadding=2><tr><td bgcolor="#222222">
<table border=0 cellpadding=5><tr><td bgcolor="#000000">
<center> 
<?php
    showDownloadForm();
?>
</center>
</td></tr></table>
</td></tr></table>
</center>

<font size=5>Credits</font><br>

<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">
This game was commissioned as part of the <a href="http://arthistoryofgames.com/">Art History of Games Conference</a>, which was co-sponsored by <a href="http://lcc.gatech.edu">Georgia Tech</A> and <a href="http://www.scad.edu/">SCAD</a>.<br>
<br>
Development was also made possible by the support of Jeff Roberts.
<br>
<br>
All design, programming, graphics, fonts, and sound by Jason Rohrer.

The graphics were made with <a href="http://mtpaint.sourceforge.net/">mtPaint</a>. The <a href="http://www.libsdl.org/">SDL</a> library provides cross-platform screen, sound, and user input.  <a href="http://www.libpng.org/pub/png/libpng.html">libpng</a> and <a href="http://www.zlib.net/">zlib</a> enable PNG output.  <a href="http://www.mingw.org/">MinGW</a> was used to build the game for Windows.  

</td></tr></table>
<br>
<br>

<center>
<a href="galvin.php"><img src="galvin1.png" width="408" height="338" border=0></a>
</center>
<br>
<br>



<?php include( "footer.php" ); ?>
