#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use Time::Piece;
#use File::stat;
use IO::Socket::INET;
use IO::Select;
use Data::Dumper;
use open qw(:std :utf8);
use utf8; # Erlaubt Umlaute im Quellcode selbst
######
# Aufruf: sudo -u svxlink /var/www/html/dl3el/aprs-metar-wx.pl
######
my $entry;
my @array;
my $login = "";
my $verbose = 0;
my $aprs = "";
my $call = "";
my $data = "";
my $rr = 0;
my $message_time = "";
my $log_time = "";
my $write2file = "";
my $tm = localtime(time);
my $version = "1.72";
my $dir = "";
my $conf = "";

# --- Konfiguration ---
my $server         = 'euro.aprs2.net';
my $port           = 14580;
my $app_name       = "DL3EL APRS Client " . $version;

my $aprs_login  = "";
my $fmn_call  = "";
my $aprs_passwd = "";
my $aprs_msg_call = "";
my $aprs_msg_call_length = 0;
my $aprs_msg_call_wc = 0;

my $hispaddr = "";
my $dbv = "";
my $qrg = "";
my $rxctcss = "";
my $radioinfo = "";

my $keepalive = 0;

my $pckt_nr = 0;
#my $srccall = "";
#my $srcdest = "";
#my $destcall = "";
my $aprsgroups = "";

# flush after every write
$| = 1;

my $blocking = 0;
#my ($socket,$client_socket);
my $socket;
my $selector    = IO::Select->new(\*STDIN);
# Koordinaten & Intervall
# Koordinaten & Intervall
my $locator 	  = "";
my $aprs_lat      = "5001.00N"; 
my $aprs_lon      = "00800.00E";
my $aprs_sym      = "-";
my $aprs_filter   = "";
my $aprs_follow   = "";
my $aprs_lat_dec  = ""; 
my $aprs_lon_dec  = "";
my $interval = 1800;
my $last_beacon = 0;
my $tg_status = "";
my $old_tgstatus = "";
my $old_aprs_lat      = "5001.00N"; 
my $old_aprs_lon      = "00800.00E";
my $old_locator 	  = "";

my $exit_script = 0;

	$dir = ($0 =~ /(.*)aprs-metar-wx.pl/i)? $1 : "undef";
	printf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV $dir $conf\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year if ($verbose >= 1);
 	my $logdatei = $dir  . "aprs-is.log";
 	my $wxdatei = $dir  . "aprs-is.wx";
	my $aprs_follow_pos = $dir  . "aprs-follow.pos";

	my $total = $#ARGV + 1;
	my $counter = 1;
	foreach my $a(@ARGV) {
		print "Arg # $counter : $a\n" if ($verbose == 7);
		$counter++;
		if (substr($a,0,2) eq "v=") {
		    $verbose = substr($a,2,1);
		    print "Debug On, Level: $verbose\n" if $verbose;
		} 
		if (substr($a,0,2) eq "l=") {
		    $login = substr($a,2,length($a-2));
		    print "login: $login\n" if $verbose;
        }
		if (substr($a,0,2) eq "c=") {
		    $fmn_call = substr($a,2,length($a));
		    print "fmn_call: $fmn_call\n" if $verbose;
        }
		if (substr($a,0,2) eq "d=") {
		    $dir = substr($a,2,length($a-2));
		    $ dir = $dir . "/dl3el";
		    print "dir: $dir\n" if $verbose;
        }
		if (substr($a,0,3) eq "db=") {
		    $dbv = substr($a,3,length($a)-3);
#		    $dbv = substr($a,3,length($a));
		    print "DB-Version: $dbv\n" if $verbose;
        }
		if (substr($a,0,4) eq "qrg=") {
		    $qrg = substr($a,4,length($a)-4);
		    print "QRG: $qrg\n" if $verbose;
        }
		if (substr($a,0,4) eq "rct=") {
		    $rxctcss = substr($a,4,length($a)-4);
		    print "RXCTCSS: $rxctcss\n" if $verbose;
        }
		if (substr($a,0,3) eq "ri=") {
		    $radioinfo = substr($a,3,length($a)-3);
		    print "Radioinfo: $radioinfo\n" if $verbose;
        }
	}
# best to create a crontab entry for the receiver to start at system boot
# @reboot /var/www/html/dl3el/aprs-is-msg.pl
# "/var/www/html" should be replaced with the running directroy of the svxlink dasboard
# all necessary definitgion have to made in the file /var/www/html/dl3el/aprs-is-msg.conf
#/var/www/html/FM-Funknetz/dl3el/aprs-is-msg.pl c=DL3EL-HS db=v5.30g by DL3EL(s)(a) ri=(432.9500MHz, T71.9)
# DL3EL APRS-IS Message Receiver [v1.71] Start: 15:11:31 am 18.02.2026 (v5.30g by DL3EL(s)(a))

#if ($verbose eq "9") {
#my $aprs_lat = "5109.25N"; 
#my $aprs_lon = "01839.46E";
#	process_nodesinfo($aprs_lat,$aprs_lon); 
#	exit;
#}	

    printf "LOG: %s Logdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
	STDOUT->autoflush(1);
	open(LOG, ">>$logdatei") or die "Fehler bei Logdatei: $logdatei\n";
    printf LOG "LOG: %s LOGGINGdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    close LOG;
#	printf LOG "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n$0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	$write2file = sprintf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\nCalled via: $0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	print_file($logdatei,$write2file) if ($verbose >= 0);
###

#!/usr/bin/perl
use strict;
use warnings;
use LWP::UserAgent;
use POSIX qw(strftime);

# --- KONFIGURATION ---
my $pos_file    = '/var/www/html/dl3el/aprs-follow.pos';
my $MetarFile = $dir  . "metar.csv";

# 1. Position aus Datei lesen
open(my $fh, '<', $pos_file) or die "Datei $pos_file nicht gefunden: $!";
my $raw_pos = <$fh>;
close($fh);
chomp($raw_pos);

# Format: 5009.20N^00839.42E -> Split am '^'
my ($lat_aprs, $lon_aprs) = split(/\^/, $raw_pos);

# Umwandlung für API (Dezimalgrad für die Suche)

my $lat_dec = aprs_to_decimal(substr($lat_aprs, 0, 7), substr($lat_aprs, 7, 1));
my $lon_dec = aprs_to_decimal(substr($lon_aprs, 0, 8), substr($lon_aprs, 8, 1));


	my $cmd = "wget -O " . $MetarFile . " -q \"http://relais.dl3el.de/cgi-bin/metar.pl?sel=latlondec&lat_dec=" . $lat_dec . "&lon_dec=" . $lon_dec . "&v=0\"";
	$cmd = `$cmd 2>&1`;
	open($fh, '<', $MetarFile) or die "Datei $MetarFile nicht gefunden: $!";
	my $raw_metar = <$fh>;
	close($fh);
	chomp($raw_metar);

# my $content = $raw_metar;
print "Original: [$raw_metar]\n" if ($verbose >= 1);
my $decoded = metar_to_aprs($raw_metar);
my $icao = "EDDF";
my $dist = 10;
	$message_time = act_time();

	$write2file = sprintf "[$message_time]^%s\n",$decoded if ($verbose >= 0);
	print_file($wxdatei,$write2file) if ($verbose >= 0);
	printf "APRS: %s %s\n",$decoded,$write2file if ($verbose >= 1);


sub metar_to_aprs {
my ($raw_string) = @_;

    # 1. Splitten des Strings am '^'
    # Format: METAR^LAT^LON^TIME^
    my ($metar, $lat_aprs, $lon_aprs, $raw_time,$dist) = split(/\^/, $raw_string);
    
    # Validierung
    return "Fehler: Unvollständiger Datenstring" unless $metar && $lat_aprs && $lon_aprs && $raw_time;

    # 2. Absender (ICAO) extrahieren
    my ($icao) = $metar =~ /\b([A-Z]{4})\b/;
    $icao //= "WX_ERR"; # Fallback falls kein ICAO gefunden

    # 3. Zeitstempel formatieren
    # Aus '251250' wird '251250z'
    my $aprs_time = $raw_time . "z";

    # 4. Wind (Richtung, Speed, Gusts)
    my ($w_dir, $w_spd, $w_gst) = ("undef", "undef", "undef");
    if ($metar =~ /\b(\d{3})(\d{2})(?:G(\d{2}))?KT\b/) {
        $w_dir = $1;
        $w_spd = sprintf("%03d", $2);
        $w_gst = sprintf("%03d", $3 || 0);
    }

    # 5. Temperatur (Celsius -> Fahrenheit)
    my $temp_c = "undef";
    if ($metar =~ /\b(M?\d{2})\/(M?\d{2})\b/) {
        my $c = $1;
        $c =~ s/M/-/; 
        $temp_c = $c;
    }

    # 6. Luftdruck (hPa -> Zehntel hPa)
    my $press_hpa = "undef";
    if ($metar =~ /Q(\d{4})/) {
        $press_hpa = $1;
    }

#[25.03.2026 00:15:01]^DB0MW-13^21.42km^&#127777;11.1c^&#128200;1012.5hpa^&#128167;59%^&#128168;0.4m/s^<span style='display:inline-block; transform:rotate(279deg);'>&uarr;</span>279°^0.9m/s^
		 my $decoded = $icao . "^" . $dist . "^";
			$decoded .= "&#127777;" .$temp_c . "c^" if ($temp_c ne "undef");
			$decoded .= "&#128200;" . $press_hpa . "hpa^" if ($press_hpa ne "undef");
			$decoded .= "&#128168;" . $w_spd . "kt^" if ($w_spd ne "undef");
			if ($w_dir ne "undef") {
				my $pfeil = sprintf "<span style='display:inline-block; transform:rotate(%sdeg);'>&uarr;</span>",$w_dir ;
				$decoded .= $pfeil . $w_dir . "°^" ;
			}	
			$decoded .= $w_gst . "kt^" if ($w_gst ne "undef");
			$decoded .= $aprs_time . "^";
			return $decoded;


    # 7. APRS Paket zusammensetzen
    # @[Zeit][Lat]/[Lon]_[WindDir]/[WindSpd]g[Gust]t[Temp]b[Press]
#    return sprintf("%s>APRS,TCPIP*:@%s%s/%s_%s/%sg%st%sb%s",
#        $icao, $aprs_time, $lat_aprs, $lon_aprs, 
#        $w_dir, $w_spd, $w_gst, $temp_f, $press
#    );
# printf "Metar2APRS1: %s\n",$metar;
}


####
# --- Funktion ---
sub aprs_to_decimal {
    my ($val, $dir) = @_;
    my $deg = substr($val, 0, index($val, '.') - 2);
    my $min = substr($val, index($val, '.') - 2);
    my $dec = $deg + ($min / 60);
    return ($dir =~ /[SW]/) ? $dec * -1 : $dec;
}


sub print_file {
	my $file = $_[0];
	my $data = $_[1];
	
	open(FILE, ">>$file") or die "Fehler bei Logdatei: $file\n";
	print FILE $data;
	print "[PF] $data" if ($verbose >= 1);
    close FILE;
}
sub act_time {
	$tm = localtime(time);
	return (sprintf("%02d.%02d.%04d %02d:%02d:%02d",$tm->mday, $tm->mon,$tm->year,$tm->hour, $tm->min, $tm->sec));
}
