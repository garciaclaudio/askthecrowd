use warnings;
use strict;

use Data::Dumper;
use WebService::GData::YouTube;

#
#1st line:
#question text|question desc
#Following lines:
#search term|answer text|description (opt)
#

my $input_file = $ARGV[0];

print "Reading from $input_file\n";

die "no input" if ! $input_file;

open(my $fh, '<', $input_file ) or die $!;

my $first_line = <$fh>; chomp $first_line;
my ($question, $question_desc) = split('\|', $first_line);

print "QUESTION/DESC: $question / $question_desc\n";

my %output;

$output{question} = $question;
$output{question_desc} = $question_desc;


my $yt = new WebService::GData::YouTube();

while( <$fh> ) { 
    chomp;
    next if ! $_;

    my ($search_term, $answer_text, $answer_desc) = split( '\|', $_ );

    $answer_text ||= $search_term;
    $answer_desc ||= '';

    print "Search term/text/desc: $search_term / $answer_text / $answer_desc\n";

    my %page;

    $page{question} = $answer_text;
    $page{question_desc} = $answer_desc;

    $yt->query()->q($search_term)->limit(5,0);

    my $videos = $yt->search_video();

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


    my @answers = map{
        { video_id => $_->video_id,
          answer_text => $_->title,
          answer_desc => $_->description,
        }
    }
    @sorted;
    
    $page{answers} = \@answers;

    foreach my $v ( @sorted ) {
        my $title = $v->title;
        my $video_id = $v->video_id;
        my $duration = $v->duration;
        my $description = $v->description;
        my $view_count = $v->view_count;
        my $num_likes = $v->rating->num_likes;
        my $num_dislikes = $v->rating->num_dislikes || 0;
        my $genre = $v->genre;
        my $score = ( $num_likes - $num_dislikes ) / $view_count;
        my $category = $v->category;
        my $uploaded = $v->uploaded;        
        print "    TITLE: $title\n";
        print "    Score: $score\n";
        print "    Duration: $duration\n";
        print "    Views: $view_count\n";
        print "    Num likes: $num_likes\n";
        print "    Num dislikes: $num_dislikes\n";
        print "    video id: $video_id\n";
        print "    Desc: $description\n";    
#        print Dumper( $uploaded );
        print "    Genre: $genre\n\n";
    }

    push( @{ $output{answers} }, \%page );
}

print Dumper( \%output );

__END__


my $yt = new WebService::GData::YouTube();

$yt->query()->q("Chico Che")->limit(3,0);
 
#or set your own query object
#$yt->query($myquery);
 
my $videos = $yt->search_video();

#
# ==100 composers, de golpe, y ahi te la llevas
# El mapa de la música nueva y antigua de México (por mexicanos y extranjeros, en Español y otros idiomas) --
#
#
#1st line:
#question text|question desc
#Following lines:
#search term|question text|description (opt)
#

#
# Cómo generarlo / subirlo? JSON
# un árbol es fácil: una lista de 100 nombres, con el título para la págia índice, se busca en youtube, se genera un JSON con los resultados.
# En el servidor: Se crean las subpáginas, luego la página índice apuntando a cada una de las subpágs.
#

#
# Una categorización: las páginas nodo deben compartir un identificador, que permita referenciarlas desde distintas categorías (e.g. Beethoven dirigido por Bernstein es apuntado por Beethoven en compositores y Bernstein en Directores). También las páginas índices deben tener un identificador, para no re-crearlas si ya existen. Se pone en una BDD. AQUI VOY.
#
# 
#

#
# XXX, check for embed permission? [later if neeeded]
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
    my $video_id = $v->video_id;
    my $duration = $v->duration;
    my $description = $v->description;
    my $view_count = $v->view_count;
    my $num_likes = $v->rating->num_likes;
    my $num_dislikes = $v->rating->num_dislikes;
    my $genre = $v->genre;
    my $score = ( $num_likes - $num_dislikes ) / $view_count;
    my $category = $v->category;
    my $uploaded = $v->uploaded;

#    my $location = $v->location;
#    my $keywords = $v->keywords;

    print "TITLE: $title\n";
    print "Score: $score\n";
    print "Duration: $duration\n";
    print "Views: $view_count\n";
    print "Num likes: $num_likes\n";
    print "Num dislikes: $num_dislikes\n";
    print "video id: $video_id\n";
    print "Desc: $description\n";

    print Dumper( $uploaded );
    print "Genre: $genre\n\n";
}



