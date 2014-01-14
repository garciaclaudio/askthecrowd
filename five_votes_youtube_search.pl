use warnings;
use strict;

use Data::Dumper;
use WebService::GData::YouTube;
 
my $yt = new WebService::GData::YouTube();
 
$yt->query()->q("Chico Che")->limit(10,0);
 
#or set your own query object
#$yt->query($myquery);
 
my $videos = $yt->search_video();

print STDERR Dumper( $videos );
