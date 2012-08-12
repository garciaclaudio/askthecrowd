#!/usr/bin/perl -w

# perl make_pdf.pl --conf_file=foo  (escribe a foo.pdf)

eval 'exec /usr/bin/perl -w -S $0 ${1+"$@"}'
    if 0; # not running under some shell
use strict;
use Data::Dumper;
use List::Util qw( shuffle );

use Getopt::Long;
use DBD::SQLite;
use DBI qw(:sql_types);

my $dbfile = '/home/claudio/Documents/data.db3';

my $dbh = DBI->connect("dbi:SQLite:dbname=$dbfile","","");

my $conf_file;

GetOptions( 'conf_file=s' => \$conf_file );

$conf_file || die "must supply conf file name";

open FOO, $conf_file;

my %selected_images;
my %en_selected_images;
my @needed;

my $title;
my $secret;

while(<FOO>) {
    if( /^TITLE:(.+)/ ) {
        $title = $1;
        next;
    }
    if( /^SECRET:(.+)/ ) {
        $secret = $1;
        next;
    }
    my ($line_num, $id, $letter, $desc, $en_desc) = split( q{,}, $_ );
    next if ! $id;
    chomp $desc;
    chomp $en_desc;
    push @needed, [$line_num, $id, $letter, $desc, $en_desc];
    print "EN DESC:  $en_desc\n";
    $selected_images{$desc} = $id;
    if( $en_desc ) {
        $en_selected_images{$en_desc} = $id;
    }
}
close FOO;

my $ids = join q{,}, map{ $_->[1] } @needed;

print "Bringing: $ids\n";

# bring from db
my $rows = $dbh->selectall_arrayref("SELECT id,tag,group_tag,image FROM images WHERE id IN($ids)");

foreach my $row ( @$rows ) {
    my( $id, $tag, $group_tag, $image) = @$row;
    print "Got $id, $tag\n";
    my $label = 'img_' . $id;
    open FOO, ">/tmp/$label.jpg";
    print FOO $image;
    close FOO;
}

if( ! $secret ) {
    make_pdf( \%selected_images, $conf_file );
    if( %en_selected_images ) {
        print STDERR "Making EN version\n";
        make_pdf( \%en_selected_images, $conf_file . '_en' );
    }
} else {
    make_secret_pdf( \%selected_images, $conf_file, $secret, $title );
    if( %en_selected_images ) {
        print STDERR "Making EN version\n";
        make_secret_pdf( \%en_selected_images, $conf_file . '_en', $secret, $title );
    }
}

sub make_pdf {
    my ($selected_images, $name) = @_;

    my @foo = split( '/', $name );
    print "NAME IS $name\n";
    $name = $foo[-1];
    print "NAME IS $name\n";

    open BAR, ">/tmp/$name.html";

    print BAR <<ENDY;
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
    print BAR qq{<td width="10%"><table cellpadding="15">\n};
    foreach my $i ( 0 .. 8 ) {
	my $img = $iii[ $i ];
	print BAR qq{<tr><TD><IMG width="70" src="/tmp/img_$img.jpg"></TD></tr>\n};
    }
    print BAR qq{</table></td>\n};

    # then, 18 words
    print BAR qq{<td width="60%" align="center"><table cellpadding="15" align="center">\n};
    foreach my $i ( 0 .. 17 ) {
	my $word = $www[ $i ];
        if( $word ) {
            print BAR qq{<tr align="center"><td align="center">$word</td></tr>\n};
        }
    }
    print BAR qq{</table></td>\n};

    # a spacer column
    print BAR q{<td width="10%">&nbsp;</td>\n};

    # last, another column of 9 images
    print BAR qq{<td width="20%"><table cellpadding="15">\n};
    foreach my $i ( 9 .. 17 ) {
	my $img = $iii[ $i ];
        if( $img ) {
            print BAR qq{<tr><TD><IMG width="70" src="/tmp/img_$img.jpg"></TD></tr>\n};
        }
    }
    print BAR qq{</table></td>\n};

    print BAR "</TR></table></body></html>\n";

    close BAR;

    `htmldoc -f ./ejercicios_pdf/${name}.pdf  /tmp/${name}.html --webpage --size a4 --no-numbered --fontsize 18`;
}



sub make_secret_pdf {
    my ($selected_images, $name, $secret, $title) = @_;

    chomp $secret;
    chomp $title;

    my $num_chars = ( grep{/[^|]/} split(/ */, $secret) );
    my $num_images =  keys (%$selected_images);

    if( $num_chars > $num_images ) {
        die "Not enough images ($num_images) for secret message ($num_chars)!\n";
    } elsif ( $num_chars < $num_images ) {
        # trim
        $selected_images = { map{ $_, $selected_images->{$_} } ( keys %$selected_images )[0..$num_chars-1] };
    }

    use Data::Dumper;
    print STDERR Dumper($selected_images);

   # cada imagen tiene una letra asignada, etc.

    my @foo = split( '/', $name );
    print "NAME IS $name\n";
    $name = $foo[-1];
    print "NAME IS $name\n";

    open BAR, ">/tmp/$name.html";

    print BAR <<ENDY;
<HTML>
<HEAD>
<STYLE TYPE="text/css">
td { font-size: 22pt }
p { font-size: 22pt }
</STYLE>
</HEAD>
<BODY>
ENDY

    my @www = shuffle keys %$selected_images;

    my @lines = split(q{\|}, $secret);

    my %chars_for_images;

    if( $title ) {
        print BAR "<p>$title</p>\n";
    }

    foreach my $line ( @lines ) {

        print STDERR "DOING LINE: $line\n";

        print BAR qq{<TABLE cellpadding="5"><TR>\n};
        foreach my $char ( split( //, $line ) ) {

            if( $char eq ' ' ) {
                print STDERR "    print a space\n";
                print BAR qq{    <TD width="65"></TD>\n};
            } else {                
                my $desc = pop @www;
                $chars_for_images{ $desc } = $char;
                my $img = $selected_images->{$desc};

                print STDERR "    print $char, will have img: $img, for desc: $desc\n";
                print BAR qq{    <TD><IMG width="60" src="/tmp/img_${img}.jpg"></TD>\n};
            }
        }

        print BAR qq{</TR><TR>\n};

        # lines under images
        foreach my $char ( split( //, $line ) ) {
            if( $char eq ' ' ) {
                print BAR qq{    <TD width="65"></TD>\n};
            } else {                
                print BAR qq{    <TD align="center">___</TD>\n};
            }
        }

        print BAR qq{</TR></TABLE>\n\n};

        # add some space
        print BAR qq{<br/>\n};

    }

    # add some space
    print BAR qq{<br/><br/>\n};

    my @first_col = keys %chars_for_images;
    my @second_col = splice @first_col, scalar(@first_col)/2;

    print BAR qq{<TABLE width="100%" cellpadding="5"><TR>\n};

    foreach my $col ( \@first_col, undef, \@second_col ) {

        print STDERR "Doing one col...\n";

        if( ! $col ) {
            # spacer col
            print BAR qq{    <TD width="50"></td>\n};
            next;
        }

        print BAR qq{    <TD valign="top" ><TABLE cellpadding="10">\n};

        foreach my $desc ( @$col ) {
            my $char = $chars_for_images{$desc};
            print STDERR "    ($char).  $desc\n";
            print BAR qq{        <tr><td><font color="red">$char<font></td><td><nobr>$desc<nobr></td></tr>\n};
        }
        print BAR qq{    </TABLE></TD>\n};
    }

    print BAR qq{</TR></TABLE>\n\n};

    print BAR "</BODY></HTML>\n";

    close BAR;

    `htmldoc -f ./ejercicios_pdf/${name}.pdf  /tmp/${name}.html --webpage --size a4 --no-numbered --fontsize 18`;
}

__END__


    my @www = shuffle keys %$selected_images;
    my @iii = shuffle values %$selected_images;

    my $img_index;
    foreach my $line ( @lines ) {
        foreach my $char ( '' $line ) {
        }
    }

    # first, one column of 9 images
    print BAR qq{<td width="10%"><table cellpadding="15">\n};
    foreach my $i ( 0 .. 8 ) {
	my $img = $iii[ $i ];
	print BAR qq{<tr><TD><IMG width="70" src="/tmp/img_$img.jpg"></TD></tr>\n};
    }
    print BAR qq{</table></td>\n};

    # then, 18 words
    print BAR qq{<td width="60%" align="center"><table cellpadding="15" align="center">\n};
    foreach my $i ( 0 .. 17 ) {
	my $word = $www[ $i ];
        if( $word ) {
            print BAR qq{<tr align="center"><td align="center">$word</td></tr>\n};
        }
    }
    print BAR qq{</table></td>\n};

    # a spacer column
    print BAR q{<td width="10%">&nbsp;</td>\n};

    # last, another column of 9 images
    print BAR qq{<td width="20%"><table cellpadding="15">\n};
    foreach my $i ( 9 .. 17 ) {
	my $img = $iii[ $i ];
        if( $img ) {
            print BAR qq{<tr><TD><IMG width="70" src="/tmp/img_$img.jpg"></TD></tr>\n};
        }
    }
    print BAR qq{</table></td>\n};

    print BAR "</TR></table></body></html>\n";

    close BAR;

    `htmldoc -f ./ejercicios_pdf/${name}.pdf  /tmp/${name}.html --webpage --size a4 --no-numbered --fontsize 18`;




















