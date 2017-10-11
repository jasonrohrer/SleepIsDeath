#!/usr/bin/perl -w 


my $wgetPath = "/usr/bin/wget"; 

my $numArgs = $#ARGV + 1;

if( $numArgs != 4 ) {
    usage();
    }

open( LIST_FILE, $ARGV[0] ) or usage();

print "opening $ARGV[0] for reading\n";

while( <LIST_FILE> ) {
    chomp;
    print "LINE: $_\n";
    (my $email, my $name) = split( /,\W*/ );
    $name =~ s/ /\+/g;

    print "  email = ($email), name = ($name)\n";

    my $url = "http://sleepisdeath.net/ticketServer/server.php?action=sell_ticket&security_data=$ARGV[2]&email=$email&name=$name&reference=manual&tags=$ARGV[1]&security_hash=$ARGV[3]";
    print "  url = $url\n";
    $result = `$wgetPath "$url" -q -O -`;

    print "  result = $result\n";
    }



sub usage {
    print "Usage:\n";
    print "  bulk.pl list_file_name tag_name security_data security_hash\n";
    print "Example:\n";
    print "  bulk.pl list.txt april16 abc 9f66c4044d8f39a9c06341b70fa791f6\n";
    print "List file must have one person per line in the following format:\n";
    print "  bob\@test.com, Bob Babbs\n";
    die;
    }
