#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use Time::Piece;
#use File::stat;
use IO::Socket::INET;
use IO::Select;

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
my $version = "1.50";
my $dir = "";
my $conf = "";

# --- Konfiguration ---
my $call_with_ssid = "DL3EL-6";
my $server         = 'euro.aprs2.net';
my $port           = 14580;
my $app_name       = "DL3EL APRS Client " . $version;

my $aprs_login  = "";
my $fmn_call  = "";
my $aprs_passwd = "";
my $aprs_msg_call = "";

my $hispaddr = "";
my $dbv = "";
my $keepalive = 0;

my $pckt_nr = 0;
my $srccall = "";
my $srcdest = "";
my $destcall = "";
my $aprsgroups = "";

# flush after every write
$| = 1;

my $blocking = 0;
#my ($socket,$client_socket);
my $socket;
my $selector    = IO::Select->new(\*STDIN);
# Koordinaten & Intervall
my $interval = 1800;
my $last_beacon = 0;

my $exit_script = 0;

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
		    $dbv = substr($a,3,length($a-3));
		    $dbv = substr($a,3,length($a));
		    print "DB-Verison: $dbv\n" if $verbose;
        }
	}
# best to create a crontab entry for the receiver to start at system boot
# @reboot /var/www/html/dl3el/aprs-is-msg.pl
# "/var/www/html" should be replaced with the running directroy of the svxlink dasboard
# all necessary definitgion have to made in the file /var/www/html/dl3el/aprs-is-msg.conf

	$dir = ($0 =~ /(.*)aprs-is-msg.pl/i)? $1 : "undef";
	$conf = $dir . "aprs-is-msg.conf";
	printf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV $dir $conf\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year if ($verbose >= 1);
 	my $logdatei = $dir  . "aprs-is.log";
 	my $msgdatei = $dir  . "aprs-is.msg";
 	my $aprs_txdatei = $dir  . "aprs-tx.msg";
	my $aprs_ok_datei = $dir  . "aprs-login.ok";
	my $aprs_exit_datei = $dir  . "aprs.exit";
	my $dbversionFile = $dir  . "dbversion.upd";
	unlink $aprs_ok_datei;

    printf "LOG: %s Logdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    printf "MSG: %s Logdatei: %s\n",$dir,$msgdatei if ($verbose >= 1);
	STDOUT->autoflush(1);
	open(LOG, ">>$logdatei") or die "Fehler bei Logdatei: $logdatei\n";
	open(MSG, ">>$msgdatei") or die "Fehler bei LOGGINGdatei: $msgdatei\n";
    printf LOG "LOG: %s LOGGINGdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    printf MSG "MSG: %s Logdatei: %s\n",$dir,$msgdatei if ($verbose >= 1);
    close MSG;
    close LOG;
#	printf LOG "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n$0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
#	printf MSG "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	$write2file = sprintf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\nCalled via: $0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	print_file($logdatei,$write2file) if ($verbose >= 0);
	$write2file = sprintf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	print_file($msgdatei,$write2file) if ($verbose >= 0);
	if ((!read_config($conf)) && ($login eq "")) {
		$write2file = sprintf "DL3EL APRS-IS Message Receiver [v$version]: no valid config found, terminating\n";
		print_file($logdatei,$write2file) if ($verbose >= 0);
		print_file($msgdatei,$write2file) if ($verbose >= 0);
		exit 0;
	}

eval {
MainLoop:
	$rr = 0;
	while (1) {
		if (!$socket || !$socket->connected) {
				$write2file = sprintf "[$log_time] new connect necessary\n";
				print_file($logdatei,$write2file) if ($verbose >= 0);
			if (!connect_aprs()) { sleep 10; next; }
		}
	    my $datastring = '';
	    $hispaddr = recv($socket, $datastring, 512, 0); # blocking recv
	    if (!defined($hispaddr)) {
			if ($blocking) {
				$log_time = act_time();
				print("recv failed: $!\n") if ($verbose >= 1);
				print LOG ("[$log_time] recv failed: $!\n");
				$write2file = sprintf "[$log_time] recv failed: $!\n";
				print_file($logdatei,$write2file) if ($verbose >= 1);
			} else {	
				# if non-blocking, nothing received, jusz wait 2s to keep cpu load down
				print "." if ($verbose >= 2);
				sleep 1; 
			}	
	    } else {
			if (length($datastring)) {
				$log_time = act_time();
				++$rr;
				$srccall = "";
				$srcdest = "";
				$destcall = "";
				$write2file = sprintf "[$log_time] recv successful ($datastring) Laenge:%s, $rr\n",length($datastring);
				print_file($logdatei,$write2file) if ($verbose >= 2);
			}	
	    }
# check if something to send
		aprs_tx($aprs_txdatei); 
		if (time() - $last_beacon >= $interval) {
			send_keepalive($aprs_login);
			$last_beacon = time();
		}

		if (-e $aprs_exit_datei) {
			unlink $aprs_exit_datei;
			$write2file = sprintf "[$log_time] APRS shutdown (aprs.exit)\n";
			print_file($logdatei,$write2file);
			$exit_script = 1;
			last;
		}	
	}	
	$socket->close();
	unlink $aprs_ok_datei;
};	
	if ($exit_script) {
		exit 0;
	} else {
		print "Folgender Fehler ist aufgetreten: $@\n" if($@);
		goto MainLoop;
	}

sub parse_aprs {
	my $raw_data = trim_cr($_[0]);
	my $ack  = "";
	my $nn = 0;
	my $d5 = $5;
	$message_time = act_time();
	$write2file = sprintf "[$message_time] working on: [$raw_data]\n" if ($verbose >= 1);
	print_file($logdatei,$write2file) if ($verbose >= 0);
#	@array = split (/\n/, $raw_data);
#	foreach $entry (@array) {
#		printf "Data $nn: %s\n",$entry;
#		$write2file = sprintf "[$message_time] Data $nn: %s\n",$entry if ($verbose >= 2);
#		print_file($logdatei,$write2file) if ($verbose >= 2);
#		++$nn;
#	}	
# msg
# DL3EL-8>APDR16,TCPIP*,qAC,T2ERFURT::DL3EL    :google.com{36
# other
# WA1GOV-10>APX219,TCPIP*,qAC,T2PERTH::DL3EL    :ISS over JO40HD AOS 02Jan26 21:28 UTC AZ 199 MaxEL/AZ 15/139
#     1       2                          3         4 
#	my $payload = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*::([\w-]+)[ :]+(.*)\{([\d]+)/i)? $4 : "undef";
	my $payload = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*::([\w-]+)[ :]+(.*)([\{]*)/i)? $4 : "undef";
	if ($payload ne "undef") {
		$srccall = $1;
		$srcdest = $2;
		$destcall = $3;
	} else {
		$write2file = sprintf "[$message_time] invalid Message %s\n",$raw_data if ($verbose >= 2);
		print_file($logdatei,$write2file) if ($verbose >= 2);
		return 0;
	}	
	if ($5 ne "") {
# prüfen, ob wir das überhaupt noch brauchen
		$d5 = $5;
		$write2file = sprintf "[$message_time] RX 1:%s 2:%s 3:%s 4:%s 5:%s\n",$srccall,$srcdest,$destcall,$payload,$d5;
		print_file($logdatei,$write2file) if ($verbose >= 2);
		$ack = ($raw_data =~ /.*\{([\d]+)/i)? $1 : "undef";
		if ($ack eq "undef") {
			$ack = ($raw_data =~ /.*(\:ack)([\d]+)/i)? $1 : "undef";
			if ($ack eq ":ack") {
				$write2file = sprintf "[$message_time] no ack to be send\n";
			} else {
				 $write2file = sprintf "[$message_time] new condition: %s (raw: %s [$d5])\n",$ack,$raw_data;
				 $ack = ":ack";
			}
		} else {	
			$write2file = sprintf "[$message_time] Payload need ack: %s\n",$ack;
		}	
# bis hier prüfen, ob wir das überhaupt noch brauchen
	} else {
		$write2file = sprintf "[$message_time] check for ack R[%s], P[%s]\n",$raw_data,$payload if ($verbose >= 2);
		print_file($logdatei,$write2file) if ($verbose >= 2);
		$ack = ($raw_data =~ /.*\{([\d]+)/i)? $1 : "undef";
		if ($ack eq "undef") {
			$write2file = sprintf "[$message_time] no ack to be send [$ack][$payload]\n" if ($verbose >= 2);
			if (substr($payload,0,3) ne "ack") {
				$ack = "msg";
			} else { 
				$ack = ":ack";
			}	
		} else {
			$write2file = sprintf "[$message_time] ack to be send [$ack] P[$payload]\n" if ($verbose >= 2);
			print_file($logdatei,$write2file) if ($verbose >= 2);
# delete ack from payload
			$payload = ($payload =~ /(.*)\{/i)? $1 : "undef";
			$write2file = sprintf "[$message_time] ack to be send P[$payload]\n" if ($verbose >= 2);
		}
	}	
	print_file($logdatei,$write2file) if ($verbose >= 2);
#	if ((defined $1) && (defined $2) && (defined $3) && ($ack ne "") && ($ack ne ":ack")) {
	if (($ack ne "") && ($ack ne ":ack")) {
# ack first
		$pckt_nr = $ack;
		$write2file = sprintf "[$message_time] Message to %s from %s: %s (%s), will be ack'd\n",$destcall,$srccall,$payload,$ack if ($verbose >= 2);
		print_file($logdatei,$write2file) if ($verbose >= 2);
		send_ack($srccall,$srcdest,$destcall,$ack);
	}
#	if (($payload ne "undef") && ($ack ne ":ack")){
	if ($ack ne ":ack") {
		$pckt_nr = $ack;
#		$srccall = $1;
#		$srcdest = $2;
#		$destcall = $3;

		if (((substr($srccall,0,5) eq "DL3EL") && ($payload eq "?update?")) || ($destcall eq "FMNUPD")) {
#			$destcall = "FMNUPD";
#		} 
#		if ($destcall eq "FMNUPD") {
			$write2file = sprintf "[$message_time] neues Update steht bereit!\n",$payload;
			print_file($msgdatei,$write2file);
            $payload = "update";
			open(ANSWER, ">$dbversionFile") or do {
						$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_txdatei): $!\n";
						print_file($logdatei,$write2file);
						die "ERROR in Filehandling: $!\n"; };
						# die "Fehler bei Datei: $aprs_txdatei\n";
			printf ANSWER $payload;
			close ANSWER;
		}	
		if ($payload eq "?wx?") {
			my $metar = "curl -s \"http://relais.dl3el.de/cgi-bin/adds.pl?ctrcall=$srccall\" 2>&1";
			$write2file = sprintf "[$message_time] Metar: %s\n",$metar;
			print_file($logdatei,$write2file) if ($verbose >= 2);
			$metar = `$metar`;
			if ($metar eq "") {
				$metar = "kein Flughafen im Umkreis von 50km";
			}	
			$write2file = sprintf "[$message_time] Metar: [%s]\n",$metar;
			print_file($logdatei,$write2file);
			prepare_answer_buffer($srccall,$metar);

		}	
		if ($payload eq "?aprs?") {
			$write2file = sprintf "[$message_time] Antwort fuer Payload %s vorbereiten\n",$payload if ($verbose >= 2);
			print_file($msgdatei,$write2file) if ($verbose >= 2);
			open(ANSWER, ">$aprs_txdatei") or do {
						$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_txdatei): $!\n";
						print_file($logdatei,$write2file);
						die "ERROR in Filehandling: $!\n"; };
						# die "Fehler bei Datei: $aprs_txdatei\n";
			printf ANSWER "%s\^APRS @ FM-Funknetz Dashbord %s",$srccall,$dbv;
			close ANSWER;
			$write2file = sprintf "[$message_time] Antwort (APRS @ FM-Funknetz Dashbord von DL3EL, %s) an %s vorbereitet\n",$dbv,$srccall if ($verbose >= 2);
			print_file($msgdatei,$write2file) if ($verbose >= 2);
		} else {
			$write2file = sprintf "[$message_time] Message to %s from %s: [%s] (%s) D:[%s]\n",$destcall,$srccall,$payload,$ack,$srcdest;
			print_file($logdatei,$write2file);
			$write2file = sprintf "[$message_time] Message to %s from %s: [%s]\n",$destcall,$srccall,$payload;
			print_file($msgdatei,$write2file);
			system('touch', $msgdatei . ".neu");
		}
	} else {
		$write2file = sprintf "no action: [$raw_data]\n" if ($verbose >= 2);
		print_file($logdatei,$write2file) if ($verbose >= 2);
		print_file($msgdatei,$write2file);
	}
}

sub send_ack {
	my $a_srccall = $_[0];
	my $a_srcdest = $_[1];
	my $a_destcall = $_[2];
	my $tx_ack = (defined $_[3])? ":ack" . $_[3] : "";
# alt: WIDE1-1
	my $path = "TCPIP*";

		while (length($a_srccall) < 9) {
			$a_srccall = $a_srccall . " ";
		}	
# DL9SAU: answer_message = Tcall + ">" + MY_APRS_DEST_IDENTIFYER + "::" + String(msg_from) + ":ack" + String(q+1);
		$data = sprintf ("%s>%s,%s::%s%s\n",$a_destcall,$a_srcdest,$path,$a_srccall,$tx_ack);
#		print $socket "$data\n";
# we can also send the data through IO::Socket::INET module,
		$socket->send($data);
		$log_time = act_time();
#		print LOG "[$log_time] $data";
		$write2file = sprintf "[$log_time] $data";
		print_file($logdatei,$write2file) if ($verbose >= 0);
}

sub send_keepalive {
	my $k_srccall = $_[0];
	my $k_srcdest = "TCPIP*";
	my $k_destcall = "APRS";

# DeinCall>APRS,TCPIP*:
		$data = sprintf ("%s>%s,%s:\n",$k_srccall,$k_destcall,$k_srcdest);
		$socket->send($data);
		$log_time = act_time();
#		print LOG "[$log_time] $data";
		$write2file = sprintf "[$log_time] keepalive $data";
		print_file($logdatei,$write2file) if ($verbose >= 0);
#### alt
#				if (substr($datastring,0,7) eq "# aprsc") {
# received keepalive
# Format: DeinCall>APRS,TCPIP*:					
#					++$keepalive;
# send one back every hour
#					if ($keepalive > 180) {
#						send_keepalive($aprs_login);
#						$keepalive = 0;
#					} 
#				} else {	
#					parse_aprs($datastring);
#				}	
###
}

sub send_msg {
	my $m_srccall = $_[0];
	my $m_srcdest = $_[1];
	my $m_destcall = $_[2];
	my $arq = $_[3];
# alt: WIDE1-1
	my $path = "TCPIP*";
	my $data = $_[4];
		while (length($m_srccall) < 9) {
			$m_srccall = $m_srccall . " ";
		}
			
# DL9SAU: answer_message = Tcall + ">" + MY_APRS_DEST_IDENTIFYER + "::" + String(msg_from) + ":ack" + String(q+1);
		if ($arq eq "no-ack") {
			$data = sprintf ("%s>%s,%s::%s:%s\n",$m_destcall,$m_srcdest,$path,$m_srccall,$data);
		} else {	
			++$pckt_nr;
			$data = sprintf ("%s>%s,%s::%s:%s{%s\n",$m_destcall,$m_srcdest,$path,$m_srccall,$data,$pckt_nr);
		}	
		$socket->send($data);
		$log_time = act_time();
		$write2file = sprintf "[$log_time] TX: %s", $data;
		print_file($logdatei,$write2file);
		print_file($msgdatei,$write2file);
}

sub print_file {
	my $file = $_[0];
	my $data = $_[1];
	
	open(FILE, ">>$file") or die "Fehler bei Logdatei: $file\n";
	print FILE $data;
	print "[PF] $data" if ($verbose >= 1);
    close FILE;
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
	$string =~ s/\r//g;
	return $string;
}

sub act_time {
	$tm = localtime(time);
	return (sprintf("%02d.%02d.%04d %02d:%02d:%02d",$tm->mday, $tm->mon,$tm->year,$tm->hour, $tm->min, $tm->sec));
}
sub read_config {
	$log_time = act_time();
	my $confdatei = $_[0];
	my $par;

	if (-e $confdatei) {
		open(INPUT, $confdatei) or die "Fehler bei Eingabedatei: $confdatei\n";
		{
			local $/;#	
			$data = <INPUT>;
		}    
		close INPUT;
	} else {
		$write2file = sprintf "[$log_time] $confdatei kann nicht geoeffnet werden: $! ($verbose)\n";
		print_file($logdatei,$write2file) if ($verbose >= 1);
		$data = "";
	}

	if ($data ne "") {
		@array = split (/\n/, $data);
		$aprs_login  = "N0CALL-M";
		$aprs_passwd = "-1";
		$aprs_msg_call = "N0CALL";
	
		foreach $entry (@array) {
			if ((substr($entry,0,1) ne "#") && (substr($entry,0,1) ne "")) {
				printf "[%s] \n",$entry if ($verbose >= 2);
				$par = ($entry =~ /([\w]+).*\=.*\"(.*)\"/s)? $2 : "undef";
				$aprs_login = $par if ($1 eq "aprs_login");
				$aprs_passwd = $par if ($1 eq "aprs_passwd");
				$aprs_msg_call = $par if ($1 eq "aprs_msg_call");
			}
		}
		$write2file = sprintf "[$log_time] current aprs config: aprs_login:%s aprs_passwd:%s aprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 1);
		print_file($logdatei,$write2file) if ($verbose >= 1);
	}
	if (($aprs_login eq "N0CALL-M") ||  ($aprs_login eq "")) {
		if ($fmn_call) {
			$aprs_login = ($fmn_call =~ /([\w]+)[-\w]*/s)? $1 : "undef";
			$write2file = sprintf "[$log_time] take aprs login from dashboard. aprs_login:%s\n", $aprs_login if ($verbose >= 1);
			print_file($logdatei,$write2file) if ($verbose >= 1);
		}
	}
	printf "2 config received:[%s]\n",$aprs_login if ($verbose >= 2);
	if (($aprs_login eq "undef") ||  ($aprs_login eq "")) {
		return(0);
	}	
	
	if (($aprs_msg_call eq "N0CALL") ||  ($aprs_msg_call eq "")) {
		$aprs_msg_call = $aprs_login;
	}
	
	if (($aprs_passwd eq "-1") || ($aprs_passwd eq "")) {
		$aprs_passwd = aprs_passcode($aprs_login);
	}	
	printf "aprs_login:%s\naprs_passwd:%s\naprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 1);
	$write2file = sprintf "[$log_time] USING aprs config: aprs_login:%s aprs_passwd:%s aprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 1);
	print_file($logdatei,$write2file) if ($verbose >= 1);
	return(1);
}

sub aprs_tx {
	$log_time = act_time();
	print "[$log_time] reading aprs tx data...\n" if ($verbose >= 3);
	my $aprsdatei = $_[0];
	my $data = "";
	my $data_len = 0;
	my $aprs_msg = "";

# Überprüfen, ob die Datei existiert
	if (-e $aprsdatei) {
		# stat gibt eine Liste zurück. Das 8. Element (Index 7) ist die Größe.
		my @file_info = stat($aprsdatei);
		my $data_len = $file_info[7]; # Größe in Bytes
		if ($data_len) {
			open(INPUT, $aprsdatei) or die "Fehler bei Eingabedatei: $aprsdatei\n";
			{
			local $/;#	
			$data = <INPUT>;
			}    
			close INPUT;
			print "Datei $aprsdatei erfolgreich geoeffnet, Laenge $data_len ($data)\n" if ($verbose >= 2);
			($destcall,$aprs_msg) = split(/\^/, $data);
			$destcall = uc $destcall;
			print "Dest $destcall, Src: $aprs_login, Text: $aprs_msg\n" if ($verbose >= 2);
			$srcdest = "APNFMN";
			if (($destcall eq "FMNUPD") || ($destcall eq "FMNTUPD")) {
				$write2file = sprintf "[$log_time] aprs_tx destcall: %s (no-ack)\n", $destcall if ($verbose >= 2);
				print_file($logdatei,$write2file) if ($verbose >= 2);
				send_msg($destcall,$srcdest,$aprs_login,"no-ack",$aprs_msg);
			} else {	
				$write2file = sprintf "[$log_time] aprs_tx destcall: %s (ack)\n", $destcall if ($verbose >= 2);
				print_file($logdatei,$write2file) if ($verbose >= 2);
				send_msg($destcall,$srcdest,$aprs_login,$pckt_nr,$aprs_msg);
			}	
			unlink $aprsdatei;
		}	
	} else {
		print "Datei '$aprsdatei' nicht gefunden.\n" if ($verbose >= 3);
	}
}

sub aprs_passcode {
# use feature 'say';

# APRS passcode calculator
# Ursprüngliches Copyright (C) 2012-2025 Christophe DAVID ( ON6ZQ | AC6ZQ )

my $callsign = $_[0];      # Rufzeichen
my $passcode = 29666;         # Startwert (uint16)

$callsign = uc($callsign);    # In Großbuchstaben umwandeln

# Wir gehen durch das Rufzeichen (Schrittweite 2)
for (my $i = 0; $i < length($callsign); $i += 2) {
    
    # Erster Charakter des Paares (entspricht a * 256)
    my $a = ord(substr($callsign, $i, 1));
    $passcode ^= ($a << 8); # Bitweises XOR mit (a * 256)

    # Zweiter Charakter des Paares (falls vorhanden)
    if ($i + 1 < length($callsign)) {
        my $b = ord(substr($callsign, $i + 1, 1));
        $passcode ^= $b;
    }
}

# Bitweises UND mit 32767 (entspricht uint16 Maskierung auf 15-bit)
$passcode &= 32767;

printf("\nThe passcode for %s is %d\n", $callsign, $passcode) if ($verbose >= 2);

$write2file = sprintf "[$log_time] The passcode for %s is %d\n", $callsign, $passcode if ($verbose >= 2);
print_file($logdatei,$write2file) if ($verbose >= 2);
return ($passcode);
}

sub prepare_answer_buffer {
	my $srccall = $_[0];
	my $text2send = $_[1];

			open(ANSWER, ">$aprs_txdatei") or do {
						$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_txdatei): $!\n";
						print_file($logdatei,$write2file);
						die "ERROR in Filehandling: $!\n"; };
						# die "Fehler bei Datei: $aprs_txdatei\n";
			printf ANSWER "%s\^%s",$srccall,$text2send;
			close ANSWER;

}

sub connect_aprs {
	my $rr;
	$log_time = act_time();
# creating object interface of IO::Socket::INET modules which internally creates
# socket, binds and connects to the TCP server running on the specific port.
	$socket = new IO::Socket::INET (
	PeerHost => 'euro.aprs2.net',
	PeerPort => '14580',
	Blocking => $blocking,
	Proto => 'tcp',
 	) or do {
		$write2file = sprintf "[$log_time] ERROR in Socket Creation : $!\n";
		print_file($logdatei,$write2file);
		die "ERROR in Socket Creation : $!\n";
	};

    $write2file = sprintf "[$log_time] TCP Connection Success.(%s)\n",$blocking;
    print_file($logdatei,$write2file) if ($verbose >= 1);

# read the socket data sent by server.
# waiting for: # logresp DL3EL verified 
	$rr = 0;
# we can override the config with par login
	if ($login eq "") {
		if  ($verbose >= 2) {
			$aprsgroups = "g/FMNUPD/APNFMN/FMNTUPD";
		} else {
			$aprsgroups = "g/FMNUPD/APNFMN";
		}	
		$login = sprintf ("user %s pass %s vers dl3el_pos 0.1 filter b/%s %s",$aprs_login,$aprs_passwd,$aprs_msg_call,$aprsgroups);
	}	
	my $login_success = "# logresp " . $aprs_login . " verified";
# Login Loop
	while ($rr < 6) {
		$write2file = sprintf "[$log_time] starting login loop ($rr)\n";
		print_file($logdatei,$write2file) if ($verbose >= 1);
		$data = <$socket>;
		++$rr;
		$log_time = act_time();
		if (defined $data) {
			$data =~ s/\r?\n$//;
			$write2file = sprintf "[$log_time] Received from Server ($rr): $data\n";
			print_file($logdatei,$write2file);
			if (substr($data,0,9) eq "# logresp") {
# waiting for: # logresp DL3EL verified 
				if (substr($data,0,length($login_success)) eq $login_success) {
					last;
				}
			}	
			$log_time = act_time();
			$write2file = sprintf "[$log_time] Login with: $login\n";
			print_file($logdatei,$write2file);
# write on the socket to server.
			print $socket "$login\n";
			if (substr($data,0,6) eq "# aprs") {
				$write2file = sprintf "[$log_time] Waiting for: $login_success\n" if ($verbose >= 0);
				print_file($logdatei,$write2file);
			}	
		}	
		sleep 2;
	}	
	if ($rr == 6) {
		$write2file = sprintf "[$log_time]  ($rr)[$aprs_login,$aprs_passwd] Login unsuccessfull $data\n";
		print_file($logdatei,$write2file);
		print_file($msgdatei,$write2file);
		unlink $aprs_ok_datei;
#		exit -1;
		return 0;
	} else {
		system('touch', $aprs_ok_datei);
		$write2file = sprintf "[$log_time] Login successfull $data\n";
		print_file($logdatei,$write2file);
		print_file($msgdatei,$write2file);
		return 1;
	}
}
