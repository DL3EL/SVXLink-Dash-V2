#!/usr/bin/perl
use v5.28;

use Time::Piece;

my $verbose = 0;
my $cmd = "";
my $data = "";
my $shari = "";
my $rssi = "";
my $txctcss = "";
my $rxctcss = "";
my $port = "";
my @CTCSS = (
  "None", "67.0", "71.9", "74.4", "77.0", "79.7", "82.5", "85.4", "88.5",
  "91.5", "94.8", "97.4", "100.0", "103.5", "107.2", "110.9", "114.8", "118.8",
  "123.0", "127.3", "131.8", "136.5", "141.3", "146.2", "151.4", "156.7",
  "162.2", "167.9", "173.8", "179.9", "186.2", "192.8", "203.5", "210.7",
  "218.1", "225.7", "233.6", "241.8", "250.3"
);

    my $log_time = act_time();
    my $total = $#ARGV + 1;
    my $counter = 1;
    my $dir = "";
    my $readmachine = "";
    foreach my $a(@ARGV) {
	print "Arg # $counter : $a\n" if ($verbose == 7);
	$counter++;
	if (substr($a,0,2) eq "v=") {
	    $verbose = substr($a,2,1);
	    print LOG "Debug On, Level: $verbose\n" if $verbose;
	} 
	if (substr($a,0,2) eq "d=") {
	    $dir = substr($a,2,length($a));
            print "Dir: $dir\n" if $verbose;
	} 
	if (substr($a,0,2) eq "r=") {
	    $readmachine = substr($a,2,1);
	} 
	if (substr($a,0,2) eq "p=") {
	    $port = substr($a,2,length($a));
            print "Dir: $port\n" if $verbose;
	} 
    }

    $shari = $dir  . "/sa818";
    print "[$log_time] SHARI $shari\n" if ($verbose >= 1);

    $cmd = sprintf("python3 %s/sa818-running.py -p %s -q",$shari,$port);
    print "[$log_time] [$cmd]\n" if ($verbose >= 1);
    $cmd = qx($cmd);
    print "[$log_time] [$cmd]\n" if ($verbose >= 1);
# RSSI=020
    $rssi = ($cmd =~ /.*RSSI=([\d]+)/s)? $1 : "undef";
# +DMOREADGROUP:1,430.5750,430.5750,0000,1,0002
    $data = ($cmd =~ /.*\+DMOREADGROUP:(\d),([\d-|\.]+),([\d-|\.]+),([\w]+),(\d),([\w]+)/s)? $1 : "undef";
    my $bandwidth = $1;
    my $tx = $2;
    my $rx = $2;
    my $ctcss = 0;
    $txctcss = $4;
print "TXCTCSS: $txctcss\n" if ($verbose >= 1);
if ((substr($txctcss,3,1) ne "N") && (substr($txctcss,3,1) ne "I")) {
    $txctcss = $CTCSS[$4];
    $ctcss = 1;
} 
    my $squelch = $5;
    $rxctcss = $6;
print "RXCTCSS: $txctcss\n" if ($verbose >= 1);
if ((substr($rxctcss,3,1) ne "N") && (substr($rxctcss,3,1) ne "I")) {
    $rxctcss = $CTCSS[$6];
    $ctcss = 1;
} 
    if (!$verbose) {
	if ($readmachine eq "") {
	    if ($ctcss) {
		$txctcss = $txctcss . "Hz" if ($4 ne "0000");
		$rxctcss = $rxctcss . "Hz" if ($6 ne "0000");
	    }
	    printf "%s/RSSI:%s<br>RX:%s/TX:%s",$2,$rssi,$rxctcss,$txctcss;
	    exit 0;
	} else {
	    print "$tx,";
	    print "$rx,";
	    printf "%s,", $txctcss;
	    print "$squelch,";
	    printf "%s,", $rxctcss;
	    print "$bandwidth\n";
	    exit 0;
	}
    } else {
	    print "\n$2\n" if ($verbose >= 0);
	    print "Channelspace: [$1]\n" if ($verbose >= 1);
	    print "QRG: [$2]\n" if ($verbose >= 0);
	    print "QRG_In: [$3]\n" if ($verbose >= 1);
	    if ($ctcss) {
		printf "TXCTCSS: [%shz]\n", $txctcss;
	    } else {
		printf "TX_DCS: [%s]\n", $txctcss;
	    }
	    print "Squelch: [$5]\n" if ($verbose >= 0);
	    if ($ctcss) {
		printf "RXCTCSS: [%shz]\n", $rxctcss;
	    } else {
		printf "RX_DCS: [%s]\n", $rxctcss;
	    }
	    printf "CMD: %s\n",$cmd;
	    $data = ($cmd =~ /.*RSSI=([\d]+)/s)? $1 : "undef";
	    printf "RSSI: %s\n",$data;
    }

sub trim_cr {
	my $string = $_[0];
	$string = shift;
	$string =~ s/\n//g;
	return $string;
}

sub act_time {
	my $tm = localtime(time);
	return (sprintf("%02d:%02d:%02d",$tm->hour, $tm->min, $tm->sec));
}


