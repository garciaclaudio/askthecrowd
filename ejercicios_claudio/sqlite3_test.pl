#!/usr/bin/perl -w

eval 'exec /usr/bin/perl -w -S $0 ${1+"$@"}'
    if 0; # not running under some shell
use strict;
use Tk;
use Tk::JPEG;
use Data::Dumper;
use List::Util qw( shuffle );

use DBI qw(:sql_types);
use DBD::SQLite;

#BEGIN {
#    $ENV{HTTP_proxy}='http://webproxy.corp.booking.com:3128/';
#}

my $dbfile = 'data.db3';

my $dbh = DBI->connect("dbi:SQLite:dbname=$dbfile","","");

#my $pic_blob = `cat /home/claudio/Pictures/Dibujo_de_Carlos_Garcia_Delgado.jpg`;
#
#my $sth = $dbh->prepare("INSERT INTO images (tag, group_tag, image) VALUES ('hello','world', ?)");
#$sth->bind_param(1, $pic_blob, SQL_BLOB);
#$sth->execute();

my ($id, $tag, $group_tag, $blob) = $dbh->selectrow_array('SELECT id, tag, group_tag, image FROM images LIMIT 1');

print "$id, $tag, $group_tag\n";

open FOO, ">/tmp/img_${id}.jpg";

print FOO $blob;

close FOO;



