
BEGIN {
    $ENV{HTTP_proxy}='http://webproxy.corp.booking.com:3128/';
}
use LWP::Simple;

my $url= 'http://t2.gstatic.com/images?q=tbn:ANd9GcQuKDOD--BgNNH0Rgxp-XtMqT_x7DPKJw1lChs2QRPwdBTjS1HbnNrDPTY';
my $img = get $url;

open FOO, ">/tmp/osito.jpg";
print FOO $img;
close FOO;
