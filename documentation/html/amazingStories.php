<?php include( "header.php" );


$deadline = "2010-04-19 23:59:59";



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
            
        
    echo "in <b>";

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
    echo "</b>";
    }

?>

<font size=6>I'm looking for some <i>Amazing</i> stories...</font><br>
<table border=0 width="100%" cellpadding=5><tr><td bgcolor="#222222">



<font size=5>...and I'm willing to pay you for them.</font><br>
<br>
I'm looking for the best examples of what can be done with <i>Sleep Is Death</i>.  If you're really pushing at the boundaries, discovering fresh techniques, and helping to reinvent how this tool can be used, please submit an example of your work to this challenge.

    
<br>
<br>
<br>
<br>

<font size=5>The challenge</font><br><br>
By my definition, an amazing story has these properties:
<ul>
<li>Uses all new, custom-designed resources.  Does <b>not</b> cobble together scenes out of the pre-made resources or slightly modified pre-made resources.
<li>Has a consistent and unique visual style.
<li>Breaks away from scene-layout conventions in some way. (Are people always two-tiles tall? Is the view always top-down?  What else can we do with this toolset?)
<li>Has good synergy between Player and Controller.
<li>Has well-developed, well-acted characters from both players (no OMG or LOLZ, k?).
<li>Has an interesting, engaging story arc that incorporates the Player's decisions and push-back.
<li>Is not overly long (120 turns max).
</ul>
Got a story like this?  Have it saved as a flip-book?  Great!




<br>
<br>
<br>
<br>

<font size=5>The offer</font><br><br>
I'm looking for up to <i>five</i> of the very best stories to feature here on the main site.  If your story is chosen, I will send you <font color=red>$200 US</font> through PayPal.  <br>
<br>
Everyone who submits a story that meets the criteria (in my generous opinion--and I don't have time to argue with anyone, so please don't try) will receive a <font color=red>free extra download code</font> that you can pass on to a friend for the April 16th release.


<br>
<br>
<br>
<br>

<font size=5>The submission</font><br><br>
Your submission must include:
<ol>
<li>A ZIP'ed copy of your story's flip-book folder.
<li>Flip- book images MUST be the <b>standard</b> 640x416 in size (if your Player has changed her screen size from the default, please have her change it back to 640x480 so as not to invoke the v13 flip-book sizing glitch).
<li>A resource pack (.pak file) containing all the new scenes and objects that you used in the story.
<li>A title.
<li>The name (or handle) of both the Controller and the Player who participated in your story (for credit).
<li>Your PayPal email address.
</ol>
Send all this, via email (with attachments) to:<br><br>
<center><font size=4>sleepisdeath77  (AT)  gmail.com</font></center>




<br>
<br>
<br>

<font size=5>The deadline</font><br><br>
(Extended!)<br>

Your submission must be received at the above email address by Midnight, New York Time, on the night of Mondy, April 20, 2010.<br><br>
<center>Yes, that's <font color=red>
<?php
$timeLeft = getTimeLeft();
if( $timeLeft > 0 ) {
    
    showTimeLeft( $timeLeft );
    }
else {
    echo "already passed (too late!)";
    }
?>
</font>.</center>
<br>

Results will be posted on the main site along with the v14 release on Tuesday, April 20, 2010, at 8pm New York time.  If your story has been chosen, you will be paid by PayPal before the results are posted.


<br>
<br>
<br>
<br>

<font size=5>The fine print</font><br><br>

I'm the judge and the only judge.  All my decisions are final.  This is a contest of skill, not a lottery.  There is no entry fee.  If such contests are illegal in your region, please don't submit anything.<br>
<br>
If the unthinkable should happen, and I get hundreds of entries, I will try to look at them all, but I reserve the right to only look at the first 100 that are received.<br>
<br>
Your flip-book will be shared with the world through this website.  Your resource pack, however, will only be downloadable from this website by people who have a valid download code for the game.  You retain ownership over your resource pack, and you can distribute it on your own as you see fit.  If we must invoke copyright talk:  if your submission is chosen, you retain ownership over it, but you grant me a non-exclusive forever-license to modify and distribute your submitted material in conjunction with any and all promotion of <i>Sleep Is Death</i>.  

</td></tr></table>
<br>
<br>

<?php include( "footer.php" ); ?>
