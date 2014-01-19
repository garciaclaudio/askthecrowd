use warnings;
use strict;

use Data::Dumper;
use WebService::GData::YouTube;
 
my $yt = new WebService::GData::YouTube();
 
$yt->query()->q("Chico Che")->limit(3,0);
 
#or set your own query object
#$yt->query($myquery);
 
my $videos = $yt->search_video();

foreach my $v ( @$videos ) {

    my $title = $v->title;
    my $duration = $v->duration;
    my $view_count = $v->view_count;
    my $favorite_count = $v->favorite_count;
    my $num_likes = $v->rating->num_likes;
    my $num_dislikes = $v->rating->num_dislikes;
    my $genre = $v->genre;

    print "TITLE: $title\n";
    print "Duration: $duration\n";
    print "Views: $view_count\n";
    print "Fav: $favorite_count\n";
    print "Num likes: $num_likes\n";
    print "Num dislikes: $num_dislikes\n";
#    print Dumper( $rating );

    print "Genre: $genre\n\n";
}


#print STDERR Dumper( $videos );
