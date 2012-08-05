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
while(<FOO>) {
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


make_pdf( \%selected_images, $conf_file );
if( %en_selected_images ) {
    print STDERR "Making EN version\n";
    make_pdf( \%en_selected_images, $conf_file . '_en' );
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


















