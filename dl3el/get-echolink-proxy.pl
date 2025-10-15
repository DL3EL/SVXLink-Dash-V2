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
	my $xml_data = "";
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
		if (substr($a,0,3) eq "xml") {
		    $xml_data = "?xml";
		    print "XML Data: $xml_data\n" if $verbose;
		} 
	}
 
	my $logdatei = $dir  . "/get-echolink-proxy.log";
	my $eldatei = $dir  . "/get-echolink-proxy.lst";
	open(LOG, ">$logdatei") or die "Fehler bei Logdatei: $logdatei ($dir)\n";
	printf LOG "DL3EL Echolink GProxy Finder [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	printf LOG "Par: V%s D%s\n",$verbose, $dir;

    read_echolink_proxies($eldatei,$xml_data);
    my $freeproxyip = ($xml_data eq "?xml")? find_el_proxy_xml() : find_el_proxy();
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
sub find_el_proxy_xml {
my $status = "";
my $address = "";
my $port = "";

my %ProxyTab;
my $ProxyTab;
my @ProxyTab;
my $proxyip = "";
my $free_proxies = 0;
my $free_proxy_found = 0;
	my $nn = 0;

    printf "\n\nLese Echolink Proxy: %s<br>\n",$el_content if ($verbose >=2);
    printf "Start, free_proxies: %s\n",$free_proxies if ($verbose >=1);

    @array = split (/<\/proxy>/, $el_content);
    foreach $entry (@array) {
	printf "Read Record [%s]\n",$entry if ($verbose >=2);
	$status = ($entry =~ /<status>(Ready)<\/status>/i)? $1 : "undef";
	if ($status eq "Ready") {
	    $address = ($entry =~ /<address>([\d\w.\-\_]+)<\/address>/i)? $1 : $entry;
	    $port = ($entry =~ /<port>([\d]+\.[\d]+\.[\d]+\.[\d]+)<\/port>/i)? $1 : "undef";
	    $ProxyTab{$free_proxies}{'IP'} = $address;
	    $ProxyTab{$free_proxies}{'PORT'} = $port;
	    printf "Nr:%s ADR:%s\n",$free_proxies,$ProxyTab{$free_proxies}{'IP'} if ($verbose);
	    ++$free_proxies;
	}
	++$nn;
#	    last if ($free_proxies > 10);
    }
    --$free_proxies;
    $free_proxy_found = int(rand($free_proxies)); # Random integer at least 0 and below 10
    print "gelesen: $nn, davon frei: $free_proxies, Zufallszahl: $free_proxy_found\n" if ($verbose);
    $proxyip = $ProxyTab{$free_proxy_found}{'IP'};

    if ($free_proxy_found) {
	return $proxyip;
    } else {
	print "gelesen: $nn\n" if ($verbose);
	return (-1);
    }
}

sub read_echolink_proxies {
    my $eldatei = $_[0];
    my $xml = $_[1];

    my $cmd = "wget --tries=2 --timeout=5 -O " . $eldatei . " -q http://www.echolink.org/proxylist.jsp" . $xml;
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
