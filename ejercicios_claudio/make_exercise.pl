#!/usr/bin/perl -w

eval 'exec /usr/bin/perl -w -S $0 ${1+"$@"}'
    if 0; # not running under some shell
use strict;
use Tk;
use Tk::JPEG;
use Data::Dumper;
use List::Util qw( shuffle );

use LWP::Simple;
use Getopt::Long;
use WWW::Mechanize;
use DBD::SQLite;
use DBI qw(:sql_types);

my $dbfile = '/home/claudio/Documents/data.db3';

my $dbh = DBI->connect("dbi:SQLite:dbname=$dbfile","","");

#
# perl make_exercise.pl
#


my %images;

my $rows = $dbh->selectall_arrayref("SELECT id,tag,group_tag,image FROM images");

foreach my $row ( @$rows ) {
    my( $id, $tag, $group_tag, $image) = @$row;
    push @{ $images{$tag} }, { id => $id, group_tag => $group_tag, image_blob => $image };
}


my $mw  = MainWindow->new();

my $old_frame;

sub make_sub {
    my $c = shift;

    return sub{ return $c };
};


sub add_new_frame {

    my $new_frame = $mw->Frame();

    # XXX, fix this
    my ($selected_tag) = (keys %images);

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
#				      print "SELECTED $selected\n";
#						 $selected_images{$active_word} = $selected;
#
#						 if( scalar keys %selected_images == 18 ) {
#
#						     make_pdf( \%selected_images );
#
#						     %selected_images = ();
#						 }
#
#						 next_word();
						 $old_frame->destroy;
						 add_new_frame( $mw );
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


foreach my $word ( @word_list ) {
    my @images = get_images( $words_file, $word );
}





sub get_images {
    my ($words_file, $word) = @_;

    my $mech = WWW::Mechanize->new();

    $mech->get( 'http://images.google.com' );

    $mech->submit_form(
        form_number => 1,
        fields      => { q => $word }
    );

    my @imgs = grep { $_->url =~ /gstatic/ } @{ $mech->images };

    my @doing =  splice @imgs, 0, 10;

    my @result;

    my $img_index = 0;

    foreach my $i (@doing) {
	my $url = $i->url;	
#	print STDERR "URL: $url\n";
	next if( $url !~ /gstatic/ );

	my $img = get $url;

        print "Inserting $word\n";

        my $sth = $dbh->prepare("INSERT INTO images (tag, group_tag, image) VALUES (?,?,?)");
        $sth->bind_param(1, $word);
        $sth->bind_param(2, $words_file);
        $sth->bind_param(3, $img, SQL_BLOB);
        $sth->execute();
    }

    return @result;
}



__END__




#my $mech = WWW::Mechanize->new();

my %selected_images;

my $active_word = '';
my $active_word_en = '';
my $word_index = 0;
my $img_index = 0;
my @imgs;


my $mw  = MainWindow->new();

my $pdf_name;

my $do_eng = 0;

GetOptions( 'pdf_name=s' => \$pdf_name,  'do_english' => \$do_eng );

die "must provide pdf_name" if ! $pdf_name;

print STDERR "Will output to: $pdf_name\n";

sub next_word {
    @imgs = ();
}

sub get_next_few {

    my @doing;

    if( @imgs ) {

	@doing =  splice @imgs, 0, 6;
    }
    else {

	$active_word = $word_list[$word_index++];

        if( $do_eng ) {
          ($active_word, $active_word_en) = split(q{|}, $active_word );
        }

	return if ! $active_word;

	my $mech = WWW::Mechanize->new();

#        $mech->proxy(['http', 'ftp'], 'http://webproxy.corp.booking.com:3128/');

	$mech->get( 'http://images.google.com' );

	$mech->submit_form(
			   form_number => 1,
			   fields      => {
					   q => $active_word_en || $active_word,
					  }
			  );

	@imgs = grep { $_->url =~ /gstatic/ } @{ $mech->images };
	@doing =  splice @imgs, 0, 6;
    }

    my @result;

    foreach my $i (@doing) {

	my $url = $i->url;
	
#	print STDERR "URL: $url\n";

	next if( $url !~ /gstatic/ );

	my $img = get $url;

	open FOO, ">/tmp/img_$img_index.jpg";

	push( @result, $img_index );
	$img_index++;

	print FOO $img;
	close FOO;
    }

    return @result;
}


sub make_sub {
    my $c = shift;

    return sub{ return $c };
};


my $pdf_index = 1;
sub make_pdf {
    my $selected_images = shift;

    print "making pdf $pdf_index\n";

    open FOO, ">/tmp/result_$pdf_index.html";

    print FOO <<ENDY;
<HTML>
<HEAD>
<STYLE TYPE="text/css">
td { font-size: 30pt }
</STYLE>
</HEAD>
<BODY>
<TABLE cellpadding="15" width="100%">
<TR>
ENDY


    my @www = shuffle keys %$selected_images;
    my @iii = shuffle values %$selected_images;

    # first, one column of 9 images
    print FOO qq{<td width="10%"><table cellpadding="15">\n};
    foreach my $i ( 0 .. 8 ) {
	my $img = $iii[ $i ];
	print FOO qq{<tr><TD><IMG width="70" src="/tmp/img_$img.jpg"></TD></tr>\n};
    }
    print FOO qq{</table></td>\n};

    # then, 18 words
    print FOO qq{<td width="60%" align="center"><table cellpadding="15" align="center">\n};
    foreach my $i ( 0 .. 17 ) {
	my $word = $www[ $i ];
        if( $word ) {
            print FOO qq{<tr align="center"><td align="center">$word</td></tr>\n};
        }
    }
    print FOO qq{</table></td>\n};

    # a spacer column
    print FOO q{<td width="10%">&nbsp;</td>\n};

    # last, another column of 9 images
    print FOO qq{<td width="20%"><table cellpadding="15">\n};
    foreach my $i ( 9 .. 17 ) {
	my $img = $iii[ $i ];
        if( $img ) {
            print FOO qq{<tr><TD><IMG width="70" src="/tmp/img_$img.jpg"></TD></tr>\n};
        }
    }
    print FOO qq{</table></td>\n};

    print FOO "</TR></table></body></html>\n";

    close FOO;

    `htmldoc -f ./${pdf_name}_${pdf_index}.pdf  /tmp/result_${pdf_index}.html --webpage --size a4 --no-numbered --fontsize 18`;

    $pdf_index++;
}

my $old_frame;
sub add_new_frame {

    my $new_frame = $mw->Frame();

    my @ids = get_next_few();

    my $selected = -1;

    my(@pl) = qw/-side top -padx .5m -pady .5m/;

    my(@left) = qw/-side left -padx .5m -pady .5m/;

    $new_frame->Label( -text => $active_word )->pack(@pl);

    $new_frame->Button(
		-text    => "brincarse",
		-width   => 10,
		-command => sub { $selected = -1;
				  next_word();
#				  print "SELECTED $selected\n";
				  $old_frame->destroy;
				  add_new_frame( $mw );
			      },
	       )->pack(@pl);


    while( my @doing = splice( @ids, 0, 2 ) ) {

	my $inner_frame = $new_frame->Frame()->pack(@left);

	foreach my $c (@doing) {

	    $inner_frame->Photo( "image$c", -file => "/tmp/img_$c.jpg" );
	    $inner_frame->Label( -image => "image$c" )->pack(@pl);

	    my $subby = make_sub( $c );

	    $inner_frame->Button(
			       -text    => "seleccióname",
			       -width   => 10,
			       -command => sub { $selected = &$subby;
#				      print "SELECTED $selected\n";
						 $selected_images{$active_word} = $selected;

						 if( scalar keys %selected_images == 18 ) {

						     make_pdf( \%selected_images );

						     %selected_images = ();
						 }

						 next_word();
						 $old_frame->destroy;
						 add_new_frame( $mw );
					     },
			      )->pack(@pl);
	    $c++;
	}
    }

#
#    $new_frame->Button(
#		-text    => "mostrar mas",
#		-width   => 10,
#		-command => sub { $selected = -1;
##				  print "SELECTED $selected\n";
#				  $old_frame->destroy;
#				  add_new_frame( $mw );
#			      },
#	       )->pack(@pl);
#


    $new_frame->pack();

    $mw->minsize( $mw->width(),
		  $mw->height() );

    $old_frame = $new_frame;
}

add_new_frame( $mw );

MainLoop;

make_pdf( \%selected_images );

# add frame

__END__



use Data::Dumper;

my @foo = get_next_few();
print $active_word . "\n";
print Dumper( \@foo );

@foo = get_next_few();
print $active_word . "\n";
print Dumper( \@foo );

next_word();

@foo = get_next_few();
print $active_word . "\n";
print Dumper( \@foo );



__END__









sub make_sub {
    my $c = shift;

    return sub{ return $c };
};

foreach my $w ( @word_list ) {

    my $mech = WWW::Mechanize->new();

    $mech->get( 'http://images.google.com' );

    $mech->submit_form(
		       form_number => 1,
		       fields      => {
				       q    => $w,
				      }
		      );

    my @imgs = grep { $_->url =~ /gstatic/ } @{ $mech->images };

    my $selected = 0;
    my $c = 0;


 SHOW_MORE:
    while( ( my @doing = splice @imgs, 0, 3 ) && $selected == 0 ) {

	my $mw  = MainWindow->new();

	$mw->Label(-text => $w)->pack(@pl);

	foreach my $i (@doing) {

	    my $url = $i->url;
	
	    print STDERR "URL: $url\n";

	    next if( $url !~ /gstatic/ );

	    my $img = get $url;

	    open FOO, ">/tmp/t$c.jpg";

	    print FOO $img;
	    close FOO;

	    $mw->Photo( "image$c", -file => "/tmp/t$c.jpg" );
	    $mw->Label( -image => "image$c" )->pack(@pl);

	    my $subby = make_sub( $c );

	    $mw->Button(
			   -text    => "choose me",
			   -width   => 10,
			   -command => sub { $selected = &$subby;
					     $mw->destroy;
					 },
			  )->pack(@pl);

	    $c++;
	}

	$mw->Button(
		       -text    => "show more",
		       -width   => 10,
		       -command => sub { $selected = 0;
					 $mw->destroy;
				     },
		      )->pack(@pl);

	MainLoop;
    }

    print "SELECTED: $selected\n";
}


__END__



use vars qw/$TOP/;

sub image1 {

    # This demonstration script displays two image widgets.

    my($demo) = @_;
    $TOP = $MW->WidgetDemo(
        -name     => $demo,
        -text     => 'This demonstration displays two images, each in a separate label widget.',
        -title    => 'Image Demonstration #1',
        -iconname => 'image1',
    );

    my(@pl) = qw/-side top -padx .5m -pady .5m/;
    $TOP->Photo('image1a', -file => Tk->findINC('demos/images/earth.gif'));
    $TOP->Label(-image => 'image1a')->pack(@pl);

    $TOP->Button(
		 -text    => "choose me",
		 -width   => 10,
		 -command => sub { print "hello"},
		)->pack(@pl);

    $TOP->Photo('image1b', -file => Tk->findINC('demos/images/earthris.gif'));
    $TOP->Label(-image => 'image1b')->pack(@pl);

    $TOP->Button(
		 -text    => "no, choose me",
		 -width   => 10,
		 -command => sub { print "hello"},
		)->pack(@pl);


} # end image1

1;

