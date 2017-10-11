#!/usr/bin/perl -w 


my $wgetPath = "/usr/bin/wget"; 

my $numArgs = $#ARGV + 1;

if( $numArgs < 2 ) {
    usage();
    }

open( LIST_FILE, $ARGV[0] ) or usage();

# print "opening $ARGV[0] for reading\n";

my $hits = 0;

while( <LIST_FILE> ) {
    chomp;
    # print "LINE: $_\n";
    (my $email, my $name) = split( /,\W*/ );
    #$name =~ s/ /\+/g;


	my $thisHitCount = 0;
	for( my $argIndex=1; $argIndex<$numArgs; $argIndex++ ) {
		my $otherListFile = $ARGV[ $argIndex ];
		
		my $result = `grep -c $email $otherListFile`;

		if( $result > 0 ) {
		    $hits ++;
			$thisHitCount++;
		    }
        }
	if( $thisHitCount == 0 ) {
		# not a repeat
		print "$email, $name\n";
	    }
	   
    }


sub usage {
    print "Usage:\n";
    print "  filterOutDuplicates.pl list_file_name older_list [older_list ...]\n";
    print "Example:\n";
    print "  filterOutDuplicates.pl list.txt fullSail1/list1.txt fullSail2/list2.txt\n";
    print "List files must have one person per line in the following format:\n";
    print "  bob\@test.com, Bob Babbs\n";
    die;
    }
