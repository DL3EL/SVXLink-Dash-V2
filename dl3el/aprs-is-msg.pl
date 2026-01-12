#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use Time::Piece;
#use File::stat;
use IO::Socket::INET;
use IO::Select;
######
# Todo
# Filter prüfen, ob pos Packete empfangen werden können
# falls, ja "moving Client einbauen", d.h. Koordinaten hier folgen einem Call
# ausserdem flexible Filter einbauen und die entsprechenden Pakete parsen
# Aufwand der MQTT Integration (OpenWebRX) prüfen 
# Beacon mit aktueller TG senden ok, 07.01.2026
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
my $version = "1.65";
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

my $exit_script = 0;

	$dir = ($0 =~ /(.*)aprs-is-msg.pl/i)? $1 : "undef";
	$conf = $dir . "aprs-is-msg.conf";
	printf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV $dir $conf\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year if ($verbose >= 1);
 	my $logdatei = $dir  . "aprs-is.log";

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

 	my $msgdatei = $dir  . "aprs-is.msg";
 	my $aprs_rxdatei = $dir  . "aprs-is.txt";
 	my $aprs_txdatei = $dir  . "aprs-tx.msg";
 	my $aprs_bcdatei = $dir  . "tg_status";
	my $aprs_ok_datei = $dir  . "aprs-login.ok";
	my $aprs_exit_datei = $dir  . "aprs.exit";
	my $aprs_follow_pos = $dir  . "aprs-follow.pos";
	my $dbversionFile = $dir  . "dbversion.upd";
	unlink $aprs_ok_datei;

    printf "LOG: %s Logdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
	STDOUT->autoflush(1);
	open(LOG, ">>$logdatei") or die "Fehler bei Logdatei: $logdatei\n";
    printf LOG "LOG: %s LOGGINGdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    close LOG;
#	printf LOG "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n$0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	$write2file = sprintf "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\nCalled via: $0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	print_file($logdatei,$write2file) if ($verbose >= 0);
	if ((!read_config($conf)) && ($login eq "")) {
		$write2file = sprintf "DL3EL APRS-IS Message Receiver [v$version]: no valid config found, terminating\n";
		print_file($logdatei,$write2file) if ($verbose >= 0);
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
		    $last_beacon = 0;
		}
	    my $datastring = '';
	    $hispaddr = recv($socket, $datastring, 1024, 0); # blocking recv
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
# remove CR obsolete
#            $datastring =~ s/\r?\n$//;
# it could be, that there are more frames in one datastring, separated bei 0x0A (\r), so we have to deal with it
			if (length($datastring)) {
				$log_time = act_time();
				++$rr;
				my $ll = length($datastring);
				$write2file = sprintf("[$log_time]recv successful ($datastring) [$ll] %s\n",$rr) if ($verbose >= 2);
				print_file($logdatei,$write2file) if ($verbose >= 2);
				my @matches = $datastring =~ m/\r/g; # Findet alle Vorkommen von \r
				my $count = scalar @matches;    # Zählt die Anzahl der gefundenen Elemente
				$count = 0;
				while ($datastring =~ /(.*)\r/g) {
					$count++; # Inkrementiert Zähler für jedes gefundene \r
# for debugging this, we have to start the script via cli 
					print "(0x0a) gefunden, $count Teil: $1\n" if ($verbose >= 3);
					if (substr($1,0,7) ne "# aprsc") {
# do not parse server msg
						parse_aprs($1);
						$ll = length($1);
						$write2file = sprintf("[$log_time]2 ($count) parsed ($1) [$ll] %s\n",$rr) if ($verbose >= 3);
						print_file($logdatei,$write2file) if ($verbose >= 3);
					} 
				}
# do not parse server msg
#				if (substr($datastring,0,7) ne "# aprsc") {
#					print_file($logdatei,$write2file) if ($verbose >= 0);
#					parse_aprs($datastring);
#				} else {
#					print_file($logdatei,$write2file) if ($verbose >= 2);
#				}		
			}	
	    }
# check if something to send
		aprs_tx($aprs_txdatei); 
		if (($aprs_lat ne "5001.00N") && ($aprs_lon ne "00800.00E")) {
#	if coordinates are given, send a beacon, when TG has changed
# resets keepalive timer, if beacon was sent
			beacon_tx($aprs_bcdatei);
		}


		if (($old_aprs_lat ne $aprs_lat) || ($old_aprs_lon ne $aprs_lon)) {
			send_keepalive($aprs_login);
			$last_beacon = time();
			$old_aprs_lat = $aprs_lat; 
			$old_aprs_lon = $aprs_lon; 
		}

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
#####
# $verbose = 1;
#####
	my $raw_data = trim_cr($_[0]);
	my $ack  = "";
	my $nn = 0;
	my $datatype = "undef";
	my $position = "";
	my $srccall = "undef";
	my $srcdest = "undef";
	my $destcall = "undef";
	my $payload = "";

	$message_time = act_time();
	$write2file = sprintf "[$message_time] working on: [$raw_data]\n" if ($verbose >= 1);
	print_file($logdatei,$write2file) if ($verbose >= 1);
# msg
# DL3EL-8>APDR16,TCPIP*,qAC,T2ERFURT::DL3EL    :google.com{36
# other
# WA1GOV-10>APX219,TCPIP*,qAC,T2PERTH::DL3EL    :ISS over JO40HD AOS 02Jan26 21:28 UTC AZ 199 MaxEL/AZ 15/139
# DJ6RG-2>T9UPV0,qAS,DB0ND-10:`~)VpHov/`"5B}439.300MHz Wolfram on Tour_1 (something invalid) -> compressed position
# DH4FE>APDR16,TCPIP*,qAC,T2SWEDEN:=5012.46N/00842.25E>438.875MHz/A=000596      F-51          438. (something invalid)
# DL3EL-8>APDR16,TCPIP*,qAC,T2POLNW::DL3EL    :?wx?{135
# DL2FCQ>APNFMN,TCPIP*,qAC,T2SPAIN::DL3EL    :Text{2
#
# Code Alternative.
# offener Punkt: ? kann nicht ausgewertet werden. Entgegen aller Beschreibeungen klappt die Regex damit nicht.
# offener Punkt: T für Telemetrie löst auch bei einem T als erstem Zeichen in der Nachricht aus
#	if ($raw_data =~ /([\w-]+)>([\w-]+),.*:([:;!\/=@#$%*<>T]{1})/i) {
	if ($raw_data =~ /([\w-]+)>([\w-]+),.*:([:;!\/=@#$%*<>]{1})/i) {
		$srccall = $1;
		$srcdest = $2;
		$datatype = $3;
	}	

	if ($datatype eq "undef") {
# Todo: compressed beacon		
		$write2file = sprintf "[$message_time] unknown data [%s]\n",$raw_data if ($verbose >= 0);
		print_file($logdatei,$write2file) if ($verbose >= 1);
		return 0;
	}	
	if ($datatype eq ":") {
# datatype is message
		$payload = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*::([\w-]+)[ :]+(.*)/i)? $4 : "undef";

		if ($payload ne "undef") {
			$srccall = $1;
			$srcdest = $2;
			$destcall = $3;
# Type Message found			
#			$payload = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*::([\w-]+)[ :]+(.*)([\{]*)/i)? $4 : "undef";
			$destcall = $3;
			if ($destcall eq $aprs_msg_call) {
				$write2file = sprintf "[$message_time] Message found from $srccall to $destcall [%s] \n",$payload if ($verbose >= 1);
				print_file($logdatei,$write2file) if ($verbose >= 1);
			} else {
				$write2file = sprintf "[$message_time] other message [%s]\n",$raw_data if ($verbose >= 1);
				print_file($logdatei,$write2file) if ($verbose >= 1);
				return 0;
			}		
		} else {
			$write2file = sprintf "[$message_time] wrong payload for datatype [%s] [%s]\n",$datatype,$raw_data if ($verbose >= 1);
			print_file($logdatei,$write2file) if ($verbose >= 1);
			return 0;
		}	
	} else {
		$write2file = sprintf "[$message_time] Datatyp is [%s] [%s]\n",$datatype,$raw_data if ($verbose >= 1);
		print_file($logdatei,$write2file) if ($verbose >= 1);
		if (($srcdest ne "APNFMN") && ($destcall ne $aprs_msg_call)) {
		# not for us, but put into Log		
			process_other($raw_data,$srccall);
		} else {
			$write2file = sprintf "[$message_time] Data ignored [%s], Datatyp is [%s]\n",$raw_data,$datatype if ($verbose >= 0);
			print_file($logdatei,$write2file) if ($verbose >= 0);
		}	
		return 0;
	}	
# check and process ack if necessary
	$write2file = sprintf "[$message_time] calling check for ack R[%s], P[%s]\n",$raw_data,$payload if ($verbose >= 2);
	print_file($logdatei,$write2file) if ($verbose >= 2);
	$ack = process_ack($raw_data,$payload,$srccall,$srcdest,$destcall);

# Process data
	if ($ack ne ":ack") {
		$pckt_nr = $ack;
# Payload could end with {ack#, therefore a substring should be done
		if (((substr($srccall,0,5) eq "DL3EL") && (substr($payload,0,8) eq "?update?")) || ($destcall eq "FMNUPD")) {
# process ?update?
			process_update($payload);
		}	
		if (($payload eq "?wx?") || (substr($payload,0,2) eq "?w")) {
# process ?wx?
			process_wx($srccall);
		}	
		if (($payload eq "?rp?") || (substr($payload,0,2) eq "?r")) {
# process ?rp?
			process_rp($payload,$srccall);
		}	
		if (substr($payload,0,3) eq "?ap") { 
# process ?aprs?
			process_aprs($payload,$srccall);
		} else {
			$write2file = sprintf "[$message_time] Message to %s from %s: [%s] (%s) D:[%s]\n",$destcall,$srccall,$payload,$ack,$srcdest;
			print_file($logdatei,$write2file);
			$write2file = sprintf "[$message_time] FM-Funknetz: Message to %s from %s: [%s]\n",$destcall,$srccall,$payload;
			$payload = ($payload =~ /(.*)\{/i)? $1 : "undef";
			$write2file = sprintf "%s^MSG^%s^^%s^\n",$srccall,$payload,aprs_time();
			print_file($msgdatei,$write2file);
			system('touch', $msgdatei . ".neu");
		}
	} else {
		$write2file = sprintf "no action: [$raw_data]\n" if ($verbose >= 1);
		print_file($logdatei,$write2file);
	}
}

sub process_ack {
	my $raw_data = $_[0];
	my $payload = $_[1];
	my $srccall = $_[2];
	my $srcdest = $_[3];
	my $destcall = $_[4];
	
	my $ack = "";
	$write2file = sprintf "[$message_time] check for ack R[%s], P[%s]\n",$raw_data,$payload if ($verbose >= 2);
	print_file($logdatei,$write2file) if ($verbose >= 2);
	$ack = ($raw_data =~ /.*\{([\d]+)/i)? $1 : "undef";
	if ($ack eq "undef") {
		$write2file = sprintf "[$message_time] no ack to be send [$payload]\n" if ($verbose >= 2);
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

	print_file($logdatei,$write2file) if ($verbose >= 2);
	if (($ack ne "") && ($ack ne ":ack")) {
# ack first
		$write2file = sprintf "[$message_time] Message to %s from %s: %s (%s), will be ack'd\n",$destcall,$srccall,$payload,$ack if ($verbose >= 2);
		print_file($logdatei,$write2file) if ($verbose >= 2);
		send_ack($srccall,$srcdest,$destcall,$ack);
		if ($ack =~ /^-?\d+(\.\d+)?$/) {
			$pckt_nr = $ack;
		}
	}
	return $ack;
}	
sub process_update {
	my $payload = $_[0];
			$write2file = sprintf "DL3EL^Info^neues Update steht bereit!^%s^\n",aprs_time();
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
sub process_wx {
	my $srccall = $_[0];
			my $metar = "curl -s \"http://relais.dl3el.de/cgi-bin/adds.pl?ctrcall=$srccall\" 2>&1";
			$write2file = sprintf "[$message_time] Metar: %s\n",$metar;
			print_file($logdatei,$write2file) if ($verbose >= 2);
			$metar = `$metar`;
			if ($metar eq "") {
				$metar = "kein Flughafen im Umkreis von 50km";
			}	
			if (length($metar) > 67) {
				my $metar1 = substr($metar,0,67);
				my $metar2 = substr($metar,67,67);
				$write2file = sprintf "[$message_time] Metar: [%s]\n",$metar1;
				print_file($logdatei,$write2file);
				prepare_answer_buffer($srccall,$metar1);
				aprs_tx($aprs_txdatei); 
				$metar = $metar2;
			} 
			$write2file = sprintf "[$message_time] Metar: [%s]\n",$metar;
			print_file($logdatei,$write2file);
			prepare_answer_buffer($srccall,$metar);
}

sub process_rp {
	my $payload = $_[0];
	my $srccall = $_[1];

			my $position = 0;
			my $old_position = 0;
			my $aprs_pr = ($payload eq "?rp?")? "y" : "s";
			my $relais = "curl -s \"http://relais.dl3el.de/cgi-bin/relais.pl?sel=ctrcall&ctrcall=$srccall&maxgateways=7&printas=csv&type_el=1&type_fr=1&kmmls=km&aprs_pr=$aprs_pr\" 2>&1";
			$write2file = sprintf "[$message_time] Repeater: %s\n",$relais;
			print_file($logdatei,$write2file) if ($verbose >= 1);
			$relais = `$relais`;
			my $relais_org = $relais;
			my $relais_1 = "";
			my $relais_2 = "";
			if ($aprs_pr eq "y") {
				$write2file = sprintf "[$message_time] Relais ORG: [%s] $old_position, $position\n",$relais_org;
				print_file($logdatei,$write2file) if ($verbose >= 2);
				if ($relais eq "") {
					$relais = "keine Relais mit Echolink oder FM-Funknetz erreichbar";
				} else {
# max. 67 Byte Payload
					$relais_1 = extract_relais($relais);
					$relais = substr($relais_org,length($relais_1)+1,length($relais_org));
					$relais_2 = extract_relais($relais);
					$relais_1 =~ s/\^/ /g;
					prepare_answer_buffer($srccall,$relais_1);
					aprs_tx($aprs_txdatei); 
					$relais_2 =~ s/\^/ /g;
					prepare_answer_buffer($srccall,$relais_2);
				}
			} else {
				@array = split (/\^/, $relais);
				my $rr = 0;
				foreach $entry (@array) {
					if ($rr < 3) {
						$relais_1 = $relais_1 . sprintf("%-20s", $entry);
					} else {
						$relais_2 = $relais_2 . sprintf("%-20s", $entry);
					}
					++$rr;
					last if ($rr == 6);
				}	
			prepare_answer_buffer($srccall,$relais_2);
			aprs_tx($aprs_txdatei); 
			$write2file = sprintf "[$message_time] Relais1 ORG: [%s] [$relais_2]\n",$relais_1;
			print_file($logdatei,$write2file) if ($verbose >= 1);
			prepare_answer_buffer($srccall,$relais_1);
			}	
		}	

sub process_aprs {
	my $payload = $_[0];
	my $srccall = $_[1];

			$write2file = sprintf "[$message_time] Antwort fuer Payload %s vorbereiten\n",$payload if ($verbose >= 1);
			print_file($logdatei,$write2file) if ($verbose >= 1);
			open(ANSWER, ">$aprs_txdatei") or do {
						$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_txdatei): $!\n";
						print_file($logdatei,$write2file);
						die "ERROR in Filehandling: $!\n"; };
						# die "Fehler bei Datei: $aprs_txdatei\n";
			printf ANSWER "%s\^APRS @ FM-Funknetz Dashbord %s",$srccall,$dbv;
			close ANSWER;
			$write2file = sprintf "[$message_time] Antwort (APRS @ FM-Funknetz Dashbord von DL3EL, %s) an %s vorbereitet\n",$dbv,$srccall if ($verbose >= 1);
			print_file($logdatei,$write2file) if ($verbose >= 1);
}	

sub extract_relais {
	my $relais = $_[0];
	my $position = 0;
	my $old_position = 0;

	while ($position < 67) {
		$old_position = $position;
		$position = index($relais, "^", $position+1);
		last if ($position eq "-1");
	}
	return substr($relais,0,$old_position);
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
		$write2file = sprintf "[$log_time]ack: $data";
		print_file($logdatei,$write2file) if ($verbose >= 0);
}

sub process_other {
	my $raw_data = $_[0];
	my $srccall = $_[1];
	my $datatype = "";
	my $position = "";
	my $rx_aprs_lat = "";
	my $rx_aprs_lon = "";
	my $aprs_txt = "";
	my $aprs_time = "";
	my $dist = "";
# danach noch manuell filtern
# APRS Data Type Identifiers
# ! Position without timestamp
# / Position with timestamp
# = Position without timestamp (with APRS messaging)
# @ Position with timestamp (with APRS messaging)
# ? Query
# # Peet Bros U-II Weather Station
# $ Raw GPS data or Ultimeter 2000
# % Agrelo DFJr / MicroFinder
# * Peet Bros U-II Weather Station
# < Station Capabilities
# > Status
# T Telemetry data
# 
# : Message
# Symbol Table Identifier Selected Table or Overlay Symbol 
#/ Primary Symbol Table (mostly stations) 
#\ Alternate Symbol Table (mostly Objects) 
#0-9 Numeric overlay. Symbol from Alternate Symbol Table (uncompressed lat/long data format) 
#a-j Numeric overlay. Symbol from Alternate Symbol Table (compressed lat/long data format only). i.e. a-j maps to 0-9 
#A-Z Alpha overlay. Symbol from Alternate Symbol Table

# 2026-01-07 15:32:17 CET: DL3EL-5>APDR16,TCPIP*,qAC,T2TURKIYE:=5009.20N/00839.39E$/A=000735 https://aprsdroid.org/
# [08.01.2026 12:19:31] DF1VA-12>APRSMC,TCPIP*,qAR,DG3FAW-12:!5011.27N/00838.10E#Gateway JO40HE/B=061/A=000420
# DN9EER-10>APDW17,TCPIP*,qAC,T2LAUSITZ:!4951.90NR00819.02E&PHG0500OpenWebRX APRS gateway
# DN9EER-10>APDW17,TCPIP*,qAC,T2LAUSITZ:!4951.90NR00819.02E&PHG0500OpenWebRX APRS gateway
# DH1FAN-9>APDR10,DB0VA*,WIDE2-1,qAR,DB0EJ:!5002.80N/00811.63E(173/000/A=000465Maddin - TG262
# DH1FAN-9>APDR10,DB0VA*,WIDE2-1,qAR,DB0EJ:!5002.80N/00811.63E(173/000/A=000465Maddin - TG262
# DH4FE>APDR16,TCPIP*,qAC,T2SWEDEN:=5012.46N/00842.25E>438.875MHz/A=000624      F-51          438.DL8HW-12>APRSGW,TCPIP*,qAR,DL8HW-12:!5016.61N/00847.06E-Wetterau#Hans/B=005/A=000476
# DH4FE>APDR16,TCPIP*,qAC,T2SWEDEN:=5012.46N/00842.25E>438.875MHz/A=000624      F-51          438.DL8HW-12>APRSGW,TCPIP*,qAR,DL8HW-12:!5016.61N/00847.06E-Wetterau#Hans/B=005/A=000476
# DB0VA>APNU19,WIDE1-1,WIDE2-2,qAO,DF6PA-2:)DB0VA FM1!5006.57N/00808.00Er439.325MHz -760 z54.vfdb.org  Wiesbaden (compressed pos?)
# DB0FDA>APRS,WIDE3-3,qAR,DB0EJ:!4952.05N/00838.28E#Relais Hochschule Darmstadt 438.587 MHz DOK F42
# DB0FDA>APRS,WIDE3-3,qAR,DB0EJ:!4952.05N/00838.28E#Relais Hochschule Darmstadt 438.587 MHz DOK F42
# DG3FBL-13>APRSGW,TCPIP*,qAR,DG3FBL-13:!4958.13N/00834.21E#MeshCom mobil#Jochen on Tour/B=098/A=000259
# DG3FBL-13>APRSGW,TCPIP*,qAR,DG3FBL-13:!4958.13N/00834.21E#MeshCom mobil#Jochen on Tour/B=098/A=000259
#		$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:([\!\/\=\@]+)([\d\d\d\d\.\d\d[N|S][\/|\\|0-9|a-j|A-Z]\d\d\d\d\d\.\d\d[W|E])/i)? $4 : "undef";
#		$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:([!|=]+)([\d\d\d\d\.\d\d[N|S][\/|\\|0-9|a-j|A-Z]\d\d\d\d\d\.\d\d[W|E])/i)? $4 : "undef";
#		$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:!([\d*\.\d*[N|S]\/\d*\.\d*[W|E])/i)? $3 : "undef";
#		$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:!([\d*\.\d*][N|S]\/\d*\.\d*[W|E])/i)? $3 : "undef";
#		$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:([!|=]+)([\d\d\d\d\.\d\d]+)/i)? $4 : "undef";
# ok		$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:([!|=]+)([\d*.\d*]+[N|S]+[\/|\\|0-9|a-j|A-Z]{1}[\d*.\d*]+)/i)? $4 : "undef";
	$position = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*:([!\/=@]+)([\d*.\d*]+[N|S]+)[\/|\\|0-9|a-j|A-Z]{1}([\d*.\d*]+[E|W]+).(.*)/i)? $4 : "undef";

	if ($position eq "undef") {
# other than pos reports
		$write2file = sprintf "[$message_time]. %s\n",$raw_data if ($verbose >= 1);
		print_file($logdatei,$write2file) if ($verbose >= 1);
	} else {
		$rx_aprs_lat = $4;
		$rx_aprs_lon = $5;
		$aprs_txt = $6;
		
		if ($srccall eq $aprs_follow) {
			$aprs_lat = $4;
			$aprs_lon = $5;
			$write2file = sprintf "0^%s^^Position of station to follow %s found: %s %s^^\n",$srccall,$srccall,$aprs_lat,$aprs_lon,aprs_time() if ($verbose >= 0);
			print_file($aprs_rxdatei,$write2file) if ($verbose >= 0);
# Position of DL3EL-15 found: 5009.20N/00839.42
# aprs_lat = "5009.20N"; 
# aprs_lon = "00839.42E";
			$aprs_lat_dec = aprs_to_decimal($aprs_lat);
			$aprs_lon_dec = aprs_to_decimal($aprs_lon);			
			my $locator = convert2loc($aprs_lat_dec,$aprs_lon_dec);
			$position = $aprs_lat . "^" . $aprs_lon . "^" . $locator;

			open(DBPOS, ">$aprs_follow_pos") or do {
					$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_follow_pos): $!\n";
					print_file($logdatei,$write2file);
					die "ERROR in Filehandling: $!\n"; };
					# die "Fehler bei Datei: $aprs_txdatei\n";
			printf DBPOS $position;
			close DBPOS;
		} else {
			my $lat_dec = aprs_to_decimal($rx_aprs_lat);
			my $lon_dec = aprs_to_decimal($rx_aprs_lon);			
			my $locator = convert2loc($lat_dec,$lon_dec);
			$dist = calcdist($lat_dec,$lon_dec,$aprs_lat_dec,$aprs_lon_dec,"km");
			$write2file = sprintf "[$message_time]%s (%s): %s, dist:%s from [%s %s]\n",$1,$locator,$6,$dist,$aprs_lat_dec,$aprs_lon_dec if ($verbose >= 0);
			print_file($logdatei,$write2file) if ($verbose >= 0);
			$write2file = sprintf "%s^%s^%s^%s^%s^%s^\n",$dist,$srccall,$locator,$aprs_txt,aprs_time(),$dist;
			print_file($aprs_rxdatei,$write2file) if ($verbose >= 0);
		}
	}	
}

sub aprs_to_decimal {
    my ($coord, $type) = @_;

    # Extraktion der Werte mit Regex
    # Latitude: DDMM.mm (2 Stellen Grad)
    # Longitude: DDDMM.mm (3 Stellen Grad)
    if ($coord =~ /^(\d{2,3})(\d{2}\.\d+)([NSEW])$/) {
        my ($degrees, $minutes, $direction) = ($1, $2, $3);

        # Berechnung: Dezimalgrad = Grad + (Minuten / 60)
        my $decimal = $degrees + ($minutes / 60);

        # Südliche Breite und westliche Länge sind negativ
        if ($direction eq 'S' || $direction eq 'W') {
            $decimal *= -1;
        }

        return sprintf("%.6f", $decimal);
    }
    return undef;
}

sub convert2loc {
my $latcalc;
my $loncalc;
my @loc;

	$latcalc = (defined $_[0])? $_[0] : 0;
	$loncalc = (defined $_[1])? $_[1] : 0;

	$loncalc += 180;
	$loncalc /= 2;

	$loc[0] = int($loncalc/10) + 65;
	$loc[2] = int($loncalc%10) + 65 - 17;
	$loc[4] = int(24*($loncalc-int($loncalc))) + 65;

	$latcalc += 90;

	$loc[1] = int($latcalc/10) + 65;
	$loc[3] = int($latcalc%10) + 65 - 17;
	$loc[5] = int(24*(abs($latcalc-int($latcalc)))) + 65;

	return(sprintf "%s%s%s%s%s%s",chr($loc[0]),chr($loc[1]),chr($loc[2]),chr($loc[3]),chr($loc[4]),chr($loc[5]));
}

sub send_keepalive {
	my $k_srccall = $_[0];
	my $k_srcdest = "TCPIP*";
	my $k_destcall = "APRS";
	my $aprs_init = "";

# mal beobachten ob das bei HTV oder FT gegen einen Verbindungsabbruch hilft
# DeinCall>APRS,TCPIP*:
#		$data = sprintf ("%s>%s,%s:\n",$k_srccall,$k_destcall,$k_srcdest);
#		$data = sprintf ("%s>%s,%s:# DL3EL keepalive\n",$k_srccall,$k_destcall,$k_srcdest);
#		$data = sprintf ("%s>%s:# DL3EL keepalive\n",$k_srccall,$k_destcall);
		if (!$last_beacon) {
			$aprs_init = "Online";
		} else {
			$aprs_init = "";
		}	
		if (($aprs_lat eq "5001.00N") &&	($aprs_lon eq "00800.00E")) {
			$data = sprintf ("%s>%s,TCPIP*:\n",$k_srccall,$k_destcall);
		} else {
			$data = sprintf ("%s>%s,TCPIP*:!$aprs_lat/$aprs_lon$aprs_sym FM-Funknetz APRS-Client (by DL3EL) %s\n",$k_srccall,$k_destcall,$aprs_init);
		}	
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
#		print_file($msgdatei,$write2file);
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

sub aprs_time {
		$tm = localtime(time);
		return (sprintf("%02d:%02d:%02d",,$tm->hour, $tm->min, $tm->sec));
}
sub read_config {
	$log_time = act_time();
	my $confdatei = $_[0];
	my $par;
	my $aprs_ssid;
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
		$aprs_login  = "N0CALL";
		$aprs_passwd = "-1";
		$aprs_msg_call = "N0CALL";
	
		foreach $entry (@array) {
			if ((substr($entry,0,1) ne "#") && (substr($entry,0,1) ne "")) {
				printf "[%s] \n",$entry if ($verbose >= 2);
				$par = ($entry =~ /([\w]+).*\=.*\"(.*)\"/s)? $2 : "undef";
				$aprs_login = $par if ($1 eq "aprs_login");
				$aprs_passwd = $par if ($1 eq "aprs_passwd");
				$aprs_msg_call = $par if ($1 eq "aprs_msg_call");
				$aprs_lat = $par if ($1 eq "aprs_lat");
				$aprs_lon = $par if ($1 eq "aprs_lon");
				$aprs_sym = $par if ($1 eq "aprs_sym");
				$aprs_filter = $par if ($1 eq "aprs_filter");
				$aprs_follow = $par if ($1 eq "aprs_follow");
			}
		}
		$write2file = sprintf "[$log_time] current aprs config: aprs_login:%s aprs_passwd:%s aprs_msg_call:%s \n aprs_filter:%s ", $aprs_login,$aprs_passwd,$aprs_msg_call,$aprs_filter;
		$write2file .= sprintf "aprs_lat: %s aprs_lon:%s aprs_sym:%s aprs_follow:%s\n",$aprs_lat,$aprs_lon,$aprs_sym,$aprs_follow if ($verbose >= 1);
		print_file($logdatei,$write2file) if ($verbose >= 1);
	}
	if ((substr($aprs_login,0,6) eq "N0CALL") ||  ($aprs_login eq "")) {
		if ($fmn_call) {
			$aprs_login = ($fmn_call =~ /([\w]+)[-\w]*/s)? $1 : "undef";
			$write2file = sprintf "[$log_time] take aprs login from dashboard. aprs_login:%s\n", $aprs_login if ($verbose >= 1);
			print_file($logdatei,$write2file) if ($verbose >= 1);
		}
	}
	$write2file = sprintf "[$log_time] Config received  CallCheck: aprs_login:%s\n", $aprs_login if ($verbose >= 1);
	print_file($logdatei,$write2file) if ($verbose >= 1);
	if (($aprs_login eq "undef") ||  ($aprs_login eq "")) {
		return(0);
	}	
	
	if (($aprs_msg_call eq "N0CALL") ||  ($aprs_msg_call eq "")) {
		$aprs_msg_call = $aprs_login;
	}
	$write2file = sprintf "[$log_time] Begin1 CallCheck: aprs_login:%s\n", $aprs_login if ($verbose >= 0);
	print_file($logdatei,$write2file) if ($verbose >= 1);
	$aprs_login = ($aprs_login =~ /([\w]+)([-]*)([-\w]*)/s)? $1 : "undef";
	if ($2 eq "-") {
		$aprs_ssid = $3;

# Test if numeric
		if ($aprs_ssid =~ /^-?\d+(\.\d+)?$/) {
			if ($aprs_ssid > 15) {
				$aprs_login = $aprs_login . "-14";
			} else {
				if ($aprs_ssid != 0) {
					$aprs_login = $aprs_login . "-" . $aprs_ssid;
				}	
			}	
		} else {
			$aprs_login = $aprs_login . "-15";
		}	

	$write2file = sprintf "[$log_time] Begin2 CallCheck: aprs_login:%s aprs_ssid:%s\n", $aprs_login,$aprs_ssid if ($verbose >= 0);
	print_file($logdatei,$write2file) if ($verbose >= 1);
	}		
	
	if (($aprs_passwd eq "-1") || ($aprs_passwd eq "")) {
		$aprs_passwd = aprs_passcode($aprs_login);
	}	
## check if station to follow and if pos avail	
	if ($aprs_follow ne "") {
		if (-e $aprs_follow_pos) {
			open(INPUT, $aprs_follow_pos) or die "Fehler bei Eingabedatei: $aprs_follow_pos\n";
			{
				local $/;#	
				$data = <INPUT>;
			}    
			close INPUT;
			($aprs_lat,$aprs_lon) = split(/\^/, $data);
			$write2file = sprintf "[$log_time] Follower %s found with position %s %s [$data]\n", $aprs_follow,$aprs_lat,$aprs_lon if ($verbose >= 0);
			print_file($logdatei,$write2file) if ($verbose >= 0);
			$aprs_filter .= " b/" . $aprs_follow;
		}	
	} else {
		if ((($aprs_lat ne "5001.00N") && ($aprs_lon ne "00800.00E")) || (($aprs_lat ne "") && ($aprs_lon ne ""))) {
			my $position = $aprs_lat . "^" . $aprs_lon;
			open(DBPOS, ">$aprs_follow_pos") or do {
					$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_follow_pos): $!\n";
					print_file($logdatei,$write2file);
					die "ERROR in Filehandling: $!\n"; };
					# die "Fehler bei Datei: $aprs_txdatei\n";
			printf DBPOS $position;
			close DBPOS;
		}	
	}
	$old_aprs_lat = $aprs_lat; 
	$old_aprs_lon = $aprs_lon; 
	$aprs_lat_dec = aprs_to_decimal($aprs_lat);
	$aprs_lon_dec = aprs_to_decimal($aprs_lon);			


	$write2file = sprintf "[$log_time] USING aprs config: aprs_login:%s aprs_passwd:%s aprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 0);
	$write2file .= sprintf "aprs_filter:%s aprs_lat: %s aprs_lon:%s aprs_sym:%s aprs_follow:%s\n",$aprs_filter,$aprs_lat,$aprs_lon,$aprs_sym,$aprs_follow if ($verbose >= 0);
	print_file($logdatei,$write2file) if ($verbose >= 0);
	$write2file .= sprintf "Radio Info found:%s\n",$radioinfo if ($verbose >= 0);
	print_file($logdatei,$write2file) if ($verbose >= 1);
	$write2file = "";
	return(1);
}

sub aprs_tx {
	$log_time = act_time();
	print "[$log_time] reading aprs tx data...\n" if ($verbose >= 3);
	my $aprsdatei = $_[0];
	my $data = "";
	my $data_len = 0;
	my $aprs_msg = "";
	my $destcall = "";
	my $srcdest = "";

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

sub beacon_tx {
#####
# $verbose = 2;
#####
	$log_time = act_time();
	print "[$log_time] reading beacon tx data...\n" if ($verbose >= 3);
	my $aprsdatei = $_[0];
	my $data = "";
	my $data_len = 0;
	my $aprs_msg = "";
#	$srcdest = "APNFMN";
	my $srcdest = "APRS";
# my $tg_status = "";
# my $old_tgstatus = "";

	$log_time = act_time();
# Überprüfen, ob die Datei existiert
	if (-e $aprsdatei) {
		# stat gibt eine Liste zurück. Das 8. Element (Index 7) ist die Größe.
		my @file_info = stat($aprsdatei);
		my $data_len = $file_info[7]; # Größe in Bytes
		my $data_mod = time() - $file_info[9]; # Größe in Bytes
		$write2file = sprintf "[$log_time] Datei:%s Laege:%s Zeit:%s\n",$aprsdatei,$data_len,$data_mod;
		print_file($logdatei,$write2file) if ($verbose >= 3);
		if (($data_len) && ($data_mod < 5)) {
			open(INPUT, $aprsdatei) or die "Fehler bei Eingabedatei: $aprsdatei\n";
			{
			local $/;#	
			$tg_status = <INPUT>;
			}    
			close INPUT;
			if ($tg_status ne $old_tgstatus) {
				$data = sprintf ("%s>%s,TCPIP*:!$aprs_lat/$aprs_lon$aprs_sym FM-Funknetz %s:TG%s %s\n",$aprs_login,$srcdest,$fmn_call,$tg_status,$radioinfo);
				$socket->send($data);
# reset keepalive timer
				$last_beacon = time();
				$write2file = sprintf "[$log_time] beacon $data";
				print_file($logdatei,$write2file) if ($verbose >= 0);
			} else {
				$write2file = sprintf "[$log_time] no change, no beacon $data";
				print_file($logdatei,$write2file) if ($verbose >= 3);
			}
			$old_tgstatus = $tg_status
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
#		$login = sprintf ("user %s pass %s vers dl3el_pos 0.1 filter b/%s %s",$aprs_login,$aprs_passwd,$aprs_msg_call,$aprsgroups);
		if ($aprs_filter eq "") {
			$aprs_filter = sprintf ("m/50 b/%s",$aprs_msg_call);
			$aprs_filter = sprintf ("b/%s",$aprs_msg_call);
			if ($aprs_follow ne "") {
				$aprs_filter .= sprintf ("/%s",$aprs_follow);
			}	
		}	
		$login = sprintf ("user %s pass %s vers %s filter %s %s",$aprs_login,$aprs_passwd,$app_name,$aprs_filter,$aprsgroups);
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
		$write2file = sprintf "Error^^[$log_time]  ($rr)[$aprs_login,$aprs_passwd] Login unsuccessfull $data^^\n";
		print_file($aprs_rxdatei,$write2file);
		unlink $aprs_ok_datei;
#		exit -1;
		return 0;
	} else {
		system('touch', $aprs_ok_datei);
		$write2file = sprintf "[$log_time] Login successfull $data\n";
		print_file($logdatei,$write2file);
#		print_file($aprs_rxdatei,$write2file);
		return 1;
	}
}

sub calcdist {
# 1.852 = 1 Breitenminute; am Äquator auch eine Längenminute, am Pol ist diese 0, bei  etwa 1 bis 1,5km
	my $lat1 = (defined $_[0])? $_[0] : 0;
	my $lon1 = (defined $_[1])? $_[1] : 0;
	my $lat2 = (defined $_[2])? $_[2] : 0; # $latctr;
	my $lon2 = (defined $_[3])? $_[3] : 0; # $lonctr;
	my $kmmls = (defined $_[4])? $_[4] : "km";
	my $calcdist = 0;
	my $d = 0;
	my $gc_dm = 0;
		
	$lat1 = $lat1/180*3.1415926;
	$lon1 = $lon1/180*3.1415926;
	$lat2 = $lat2/180*3.1415926;
	$lon2 = $lon2/180*3.1415926;

	if (($lat1 == $lat2) && ($lon1 == $lon2)) {
		$calcdist = 0;
		}
	else	{
		$d = acos(sin($lat1) * sin($lat2) + cos($lat1) * cos($lat2) * cos($lon1 - $lon2));
		$gc_dm = ((180/(3.1415926)) * $d * (1.852)) * 60;
		if ($kmmls eq "mls") {
			$calcdist = $gc_dm / 1.652;
			}
		else	{
			$calcdist = $gc_dm;
			}
		}
	$calcdist = sprintf("%5.2f%s",$calcdist,$kmmls);
	return($calcdist);
}

sub acos { atan2( sqrt(1 - $_[0] * $_[0]), $_[0] ) }
