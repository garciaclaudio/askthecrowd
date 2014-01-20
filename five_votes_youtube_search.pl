use warnings;
use strict;

use Data::Dumper;
use WebService::GData::YouTube;
 
my $yt = new WebService::GData::YouTube();
 
$yt->query()->q("Chico Che")->limit(50,0);
 
#or set your own query object
#$yt->query($myquery);
 
my $videos = $yt->search_video();

#
# XXX, discard too recent (may be struck down soon)?
# Better implement cron job later to automatically clean up deceased videos
#

#
# Sorting formula:
# genre has to be Music
#
# (likes - 2*dislikes) / views
#
# views > 1000
#


my @sorted

= map{ $_->[0] } 

sort { $a->[1] <=> $b->[1] }

map { 
    my $view_count = $_->view_count;
    my $num_likes = $_->rating->num_likes;
    my $num_dislikes = $_->rating->num_dislikes;
    my $score = ( $num_likes - $num_dislikes ) / $view_count;
    [ $_, $score ]
}

grep { $_->view_count > 100 }

grep { $_->genre eq 'Music' }

@$videos;

foreach my $v ( @sorted ) {

    my $title = $v->title;
    my $duration = $v->duration;
    my $view_count = $v->view_count;
    my $num_likes = $v->rating->num_likes;
    my $num_dislikes = $v->rating->num_dislikes;
    my $genre = $v->genre;
    my $score = ( $num_likes - $num_dislikes ) / $view_count;

    print "TITLE: $title\n";
    print "Score: $score\n";
    print "Duration: $duration\n";
    print "Views: $view_count\n";
    print "Num likes: $num_likes\n";
    print "Num dislikes: $num_dislikes\n";
#    print Dumper( $rating );

    print "Genre: $genre\n\n";
}


#print STDERR Dumper( $videos );
