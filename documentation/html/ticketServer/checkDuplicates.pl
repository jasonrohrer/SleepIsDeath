#!/usr/bin/perl -w 


my $wgetPath = "/usr/bin/wget"; 

my $numArgs = $#ARGV + 1;

if( $numArgs < 2 ) {
    usage();
    }

open( LIST_FILE, $ARGV[0] ) or usage();

print "opening $ARGV[0] for reading\n";

my $hits = 0;

while( <LIST_FILE> ) {
    chomp;
    # print "LINE: $_\n";
    (my $email, my $name) = split( /,\W*/ );
    $name =~ s/ /\+/g;


	for( my $argIndex=1; $argIndex<$numArgs; $argIndex++ ) {
		my $otherListFile = $ARGV[ $argIndex ];
		
		my $result = `grep -c $email $otherListFile`;

		if( $result > 0 ) {
			print "$email, $name\n";
            $hits ++;
		    }
        }
    }

print "+++ Total hits:  $hits\n";

sub usage {
    print "Usage:\n";
    print "  checkDuplicates.pl list_file_name older_list [older_list ...]\n";
    print "Example:\n";
    print "  checkDuplicates.pl list.txt fullSail1/list1.txt fullSail2/list2.txt\n";
    print "List files must have one person per line in the following format:\n";
    print "  bob\@test.com, Bob Babbs\n";
    die;
    }
