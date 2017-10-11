<?php
include_once( "seedBlogs.php" );
include_once( "header.php" );
?>

</CENTER>



<table width="100%" border=0><tr>

<td align=left>

<!-- hard-coded clean RSS link -->
<table BORDER=0 CELLSPACING=0 CELLPADDING=1><tr><td BGCOLOR=#898E79><table BORDER=0 CELLSPACING=0 CELLPADDING=1><tr><td BGCOLOR=#FFFFFF><table BORDER=0 CELLSPACING=0 CELLPADDING=2><tr><td BGCOLOR=#FF6600><a HREF="http://www.sleepisdeath.net/rss.xml"><font COLOR=#FFFFFF><b>RSS 2.0</b></font></a></td></tr></table></td></tr></table></td></tr></table>

</td>

<td align=center>
Alternative: <a href="http://twitter.com/sleepisdeath77">@sleepisdeath77</a> on Twitter

<td align=right>

<?php
seedBlogs_showSearchBox(
    // 10 characters wide
    10,
    // show the Search button
    true );
?>

</td>
</tr>
</table>


<?php
seedBlog(
    // name of this seed blog in the database
    "news",
    // 1 = show intro text below headlines
    // 0 = show only headlines
    1,
    // 1 = show author for each post
    // 0 = hide author
    1,
    // 1 = show creation date for each post
    // 0 = hide dates
    1,
    // 2 = allow custom order tweaking with up/down widgets
    // 1 = order by creation date (newest first)
    // 0 = order by expiration date (oldest first)
    1,
    // show at most 5 posts
    5,
    // skip none of them (start with first post)
    0,
    // show the archive link
    1,
    // hide the submission link from public
    0 );
/*
    // opening tags for each story block
    "<TABLE WIDTH=100% cellspacing=0 cellpadding=0>",
    // closing tags for each story block
    "</TABLE>",
    // opening tags for headlines
    "<TR><TD colspan=2 BGCOLOR=\"#ffffff\">" .
    "<FONT SIZE=5>",
    // closing tags for headlines
    "</FONT></td></tr>",
    // opening tags for each text block
    "<tr><td colspan=2 bgcolor=\"#cccccc\" height=1></td></tr>" .
    "<TR><td bgcolor=\"#cccccc\" width=1></td>" .
    "<TD BGCOLOR=\"#ffffff\">" .
    "<table border=0 width=\"100%\" cellspacing=0 " .
    "cellpadding=5><tr><td>",
    // closing tags for each text block
    "</td></tr></table></TD></TR>",
    // story separator
    "<BR><BR><BR>" );
*/
?>

<BR>




<BR>




<!-- see hard-coded clean rss link in footer.php
<?php
// show an RSS feed button here
seedBlogRSSButton(
    // the name of the blog in the database
    "stories",
    // title of the RSS channel
    "Arthouse Games",
    // description of the channel
    "insisting that our medium can reach beyond entertainment.",
    // include at most 10 features in the feed
    10,
    // show authors
    1,
    // show post dates
    1 );
?>
-->

<table width="100%" border=0><tr>

<td align=right>
[<a href="newsLogin.php">login</a>]
</td>
</tr>
</table>

<CENTER>

<?php include_once( "footer.php" ); ?>

