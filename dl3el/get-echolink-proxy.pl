#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use Time::Piece;
use File::stat;

my $entry;
my @array;
my %CurrentLoginsTab;
my @DataTabFN;
my %DataTabFN;
my %DataTabMonTG;
my %DataTabActTG;
my $callfound;
my $nn;
my $call;
my $rawcall;
my $tg1;
my $tg2;
my $el_content;
my $log_time = "";
my @valid_mon_tgs;
my $vmt_org = 0;
my $vmt = 0;
my $reload = 0;
my $srch_pref = "";;

my $verbose = 0;
my $tm = localtime(time);
my $eldatei_upd = 0;
my $version = "1.00";

	my $cmd = "pwd";
	my $dir =`$cmd`;
	$dir = trim_cr($dir);
#	my $logdatei = $dir  . "/get-echolink-proxy.log";
#	my $eldatei = $dir  . "/get-echolink-proxy.lst";
	STDOUT->autoflush(1);
	my $total = $#ARGV + 1;
	my $counter = 1;
	foreach my $a(@ARGV) {
		print "Arg # $counter : $a\n" if ($verbose == 1);
		$counter++;
		if (substr($a,0,2) eq "v=") {
		    $verbose = substr($a,2,1);
		    print "Debug On, Level: $verbose\n" if $verbose;
		} 
		if (substr($a,0,2) eq "d=") {
		    $dir = substr($a,2,length($a));
		    print "Dir: $dir\n" if $verbose;
		} 
	}
 
	my $logdatei = $dir  . "/get-echolink-proxy.log";
	my $eldatei = $dir  . "/get-echolink-proxy.lst";
	open(LOG, ">$logdatei") or die "Fehler bei Logdatei: $logdatei ($dir)\n";
	printf LOG "DL3EL Echolink GProxy Finder [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	printf LOG "Par: V%s D%\n",$verbose, $dir;

    read_echolink_proxies($eldatei);
    my $freeproxyip = find_el_proxy();
    print "$freeproxyip\n";
    close LOG;
    exit 0;

sub find_el_proxy {
my $elproxy;
my $proxy_found = 0;
my $proxyip = "";
my $proxyip_found = 0;
my $proxystatus = "";
my $free_proxy_found = 0;
my $call;
my $node;
my @array;
my @array_proxy;
my $entry_proxy = "";
my $cl_found = 0;
my $dlcall;
my $ssid;
my $srch_pref = "ready";

    printf "\n\nLese Echolink Proxy: %s<br>\n",$el_content if ($verbose >=2);

	@array = split (/\<tr class=\"normal-row\"\>/, $el_content);

	foreach $entry (@array) {
	    printf "Read Record [%s]\n",$entry if ($verbose >=2);
	    @array_proxy = split (/\&nbsp\;<\/td>/, $entry);
	    $proxy_found = 0;
	    $proxyip_found = 0;
	    $proxystatus = "";
	    $free_proxy_found = 0;
	    foreach $entry_proxy (@array_proxy) {
		printf "Read Proxy [%s]:($proxyip_found,$free_proxy_found)\n",$entry_proxy if ($verbose >=2);
		if (!$proxy_found) {
		    $elproxy = ($entry_proxy =~ /.*<td>([\w \#]+)/i)? $1 : "undef";
		    if ($elproxy ne "undef") {
			printf "Proxy found[%s]\n",$elproxy if ($verbose >=2);
			$proxy_found = 1;
		    }    
		} else {
		    if (!$proxyip_found) {
			$proxyip = ($entry_proxy =~ /<td>([\d\.]+)/i)? $1 : "undef";
			if ($proxyip ne "undef") {
			    print "get IP: $1\n" if ($verbose >0);
			    $proxyip_found = 1;
			}
		    } else {
			$proxystatus = ($entry_proxy =~ /<td><font color=\"(Green)\">Ready&nbsp;/i)? $1 : "undef";
			if ($proxystatus ne "undef") {
			    print "free Proxy found: $proxyip\n" if ($verbose >0);
			    $free_proxy_found = 1;
			    last;
			}
		    }

		}    
	    }
	last if $free_proxy_found;
        }    
	if ($free_proxy_found) {
	    return $proxyip;
	} else {
	    return (-1);;
	}
}

sub read_echolink_proxies {
    my $eldatei = $_[0];

    my $cmd = "wget --tries=2 --timeout=5 -O " . $eldatei . " -q http://www.echolink.org/proxylist.jsp";
#    my $tx ="";
    my $tx =`$cmd`;
    printf "$log_time aktuelle Echolink Proxies<br>\n($cmd)\n($eldatei)\n" if ($verbose>2);

    open(INPUT, $eldatei) or die "xxxx $dir Fehler bei Eingabedatei: $eldatei [$0: open $eldatei: $!]\n";
    {
        local $/;#	
        $el_content = <INPUT>;
        printf "$log_time Echolink Daten aus Cache geholt<br>%s\n",$el_content if ($verbose>3);
    }    
    close(INPUT);
}    
sub file_date_time {
	my $eldatei = $_[0];
    my $tm = localtime(stat($eldatei)->mtime);
	return (sprintf("%02d.%02d.%04d %02d:%02d:%02d",$tm->mday, $tm->mon,$tm->year,$tm->hour, $tm->min, $tm->sec));
}
sub act_date_time {
	my $tm = localtime(time);
	return (sprintf("%02d.%02d.%04d %02d:%02d:%02d",$tm->mday, $tm->mon,$tm->year,$tm->hour, $tm->min, $tm->sec));
}

sub act_time {
	my $tm = localtime(time);
	return (sprintf("%02d:%02d:%02d",$tm->hour, $tm->min, $tm->sec));
}

sub trim_plus {
	my $string = $_[0];
	$string = shift;
	$string =~ s/\+//g;
	return $string;
}

sub trim_cr {
	my $string = $_[0];
	$string = shift;
	$string =~ s/\n//g;
	return $string;
}
