#!/usr/bin/perl -w

eval 'exec /usr/bin/perl -w -S $0 ${1+"$@"}'
    if 0; # not running under some shell
use strict;
use Tk;
use Tk::JPEG;
use Data::Dumper;
use List::Util qw( shuffle );

use Getopt::Long;
use WWW::Mechanize;
use DBD::SQLite;
use DBI qw(:sql_types);

my $dbfile = '/home/claudio/Documents/data.db3';

my $dbh = DBI->connect("dbi:SQLite:dbname=$dbfile","","");

#
# perl make_exercise.pl
#


my $num_imgs = 18;
GetOptions( 'num_images=s' => \$num_imgs );

my %images;
my %images_by_id;

my $rows = $dbh->selectall_arrayref("SELECT id,tag,group_tag,image FROM images");

foreach my $row ( @$rows ) {
    my( $id, $tag, $group_tag, $image) = @$row;
    my $img = { id => $id, group_tag => $group_tag, image_blob => $image };
    push @{ $images{$tag} }, $img;
    $images_by_id{$id} = $img;
}

my @shuffled_tags = shuffle keys %images;;


my $mw  = MainWindow->new();

my $old_frame;

sub make_sub {
    my $c = shift;

    return sub{ return $c };
};


sub make_output {
    my $selected = shift;
    open FOO, ">nuevo_archivo";
    my $i=1;
    foreach my $id ( @$selected ) {
        print FOO "$i,$id,,\n";
        $i++;
    }
    close FOO;
}


my @selected;

sub add_new_frame {
    my $new_frame = $mw->Frame();

    my $selected_tag = pop @shuffled_tags;

    my @selected_images = @{ $images{$selected_tag} };

    my $selected = -1;

    my(@pl) = qw/-side top -padx .5m -pady .5m/;

    my(@left) = qw/-side left -padx .5m -pady .5m/;

    $new_frame->Label( -text => $selected_tag )->pack(@pl);

    $new_frame->Button(
		-text    => "mostrar otras",
		-width   => 10,
		-command => sub {
				  $old_frame->destroy;
				  add_new_frame( $mw );
			      },
	       )->pack(@pl);

    while( my @doing = splice( @selected_images, 0, 2 ) ) {

	my $inner_frame = $new_frame->Frame()->pack(@left);

	foreach my $img (@doing) {
            my $label = 'image' . $img->{id};
            open FOO, ">/tmp/$label.jpg";
            print FOO $img->{image_blob};
            close FOO;
	    $inner_frame->Photo( $label, -file => "/tmp/$label.jpg" );
	    $inner_frame->Label( -image => $label )->pack(@pl);

	    my $subby = make_sub( $img->{id} );

	    $inner_frame->Button(
			       -text    => "seleccióname",
			       -width   => 10,
			       -command => sub { $selected = &$subby;
				      print "SELECTED $selected\n";
                                      push @selected, $selected;
                                      if( @selected == $num_imgs ) {
                                          make_output(\@selected);
                                          exit;
                                      }
                                },
                          )->pack(@pl);
	}
    }

    $new_frame->pack();
    $mw->minsize( $mw->width(),
		  $mw->height() );
    $old_frame = $new_frame;
}

add_new_frame( $mw );

MainLoop;



__END__


