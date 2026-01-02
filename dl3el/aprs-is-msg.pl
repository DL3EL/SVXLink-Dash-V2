#!/usr/bin/perl
use warnings;
use strict;
use utf8;
use Time::Piece;
#use File::stat;
use IO::Socket::INET;

my $entry;
my @array;
my $login = "";
my $verbose = 0;
my $aprs = "";
my $call = "";
my $srccall = "";
my $srcdest = "";
my $destcall = "";
my $data = "";
my $rr = 0;
my $message_time = "";
my $log_time = "";
my $write2file = "";
my $tm = localtime(time);
my $version = "1.30";
my $dir = "";
my $conf = "";

my $aprs_login  = "";
my $fmn_call  = "";
my $aprs_passwd = "";
my $aprs_msg_call = "";

my $hispaddr = "";
my $dbv = "";
my $keepalive = 0;

my $send_trigger = 0;
my $pckt_nr = 0;
my $s_srccall = "";
my $s_srcdest = "";
my $s_destcall = "";

# flush after every write
$| = 1;

my ($socket,$client_socket);


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
	unlink $aprs_ok_datei;

    printf "LOG: %s Logdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    printf "MSG: %s Logdatei: %s\n",$dir,$msgdatei if ($verbose >= 1);
	STDOUT->autoflush(1);
#    open(LOG, ">/home/svxlink/dl3el/get-monitor.log") or die "Fehler bei Logdatei: $tgdatei\n";
	open(LOG, ">$logdatei") or die "Fehler bei Logdatei: $logdatei\n";
	open(MSG, ">>$msgdatei") or die "Fehler bei LOGGINGdatei: $msgdatei\n";
    printf LOG "LOG: %s LOGGINGdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    printf LOG "MSG: %s Logdatei: %s\n",$dir,$msgdatei if ($verbose >= 1);
	if ((!read_config($conf)) && ($login eq "")) {
		print LOG "DL3EL APRS-IS Message Receiver [v$version]: no valid config found, terminating\n";
		print MSG "DL3EL APRS-IS Message Receiver [v$version]: no valid config found, terminating\n";
		exit 0;
	}
	printf LOG "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n$0 @ARGV\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
	printf MSG "DL3EL APRS-IS Message Receiver [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d ($dbv)\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
    close MSG;
    close LOG;

Start:
my $blocking = 0;
	$log_time = act_time();
	$write2file = sprintf "[$log_time] Blocking Mode: (%s)\n",$blocking;
	print_file($logdatei,$write2file);
# creating object interface of IO::Socket::INET modules which internally creates
# socket, binds and connects to the TCP server running on the specific port.
	$socket = new IO::Socket::INET (
	PeerHost => 'rotate.aprs2.net',
	PeerPort => '14580',
	Blocking => $blocking,
	Proto => 'tcp',
 	) or do {
		$write2file = sprintf "[$log_time] ERROR in Socket Creation : $!\n";
		print_file($logdatei,$write2file);
		die "ERROR in Socket Creation : $!\n";
	};

    $write2file = sprintf "[$log_time] TCP Connection Success.\n";
    print_file($logdatei,$write2file) if ($verbose >= 1);

# read the socket data sent by server.
# waiting for: # logresp DL3EL verified 
	$rr = 0;
	while ($rr < 6) {
		$write2file = sprintf "[$log_time] starting login loop ($rr)\n";
		print_file($logdatei,$write2file) if ($verbose >= 1);
		$data = <$socket>;
# we can also read from socket through recv()  in IO::Socket::INET
# $socket->recv($data,1024);
		++$rr;
		$log_time = act_time();
		if (defined $data) {
			$write2file = sprintf "[$log_time] Received from Server ($rr): $data\n";
			print_file($logdatei,$write2file);
# waiting for: # logresp DL3EL verified 
			my $login_success = "# logresp " . $aprs_login . " verified";
			$write2file = sprintf "[$log_time] Waiting for: $login_success\n";
			print_file($logdatei,$write2file);
			if (substr($data,0,length($login_success)) eq $login_success) {
				last;
			}	
			if (substr($data,0,6) eq "# aprs") {
# we can override the config with par login
				if ($login eq "") {
					$login = sprintf ("user %s pass %s vers dl3el_pos 0.1 filter b/%s",$aprs_login,$aprs_passwd,$aprs_msg_call);
				}	
				$data = $login;
				$log_time = act_time();
				$write2file = sprintf "[$log_time] send2server (login): $data\n";
				print_file($logdatei,$write2file) if ($verbose >= 1);
# write on the socket to server.
				print $socket "$data\n";
			}	
		}	
		sleep 2;
	}	
	if ($rr == 6) {
		$write2file = sprintf "[$log_time] Login unsuccessfull $data ($rr)\n";
		print_file($logdatei,$write2file);
		unlink $aprs_ok_datei;
		exit -1;
	} else {
		system('touch', $aprs_ok_datei);
		$write2file = sprintf "[$log_time] Login successfull $data ($rr)\n";
		print_file($logdatei,$write2file);
	}
	$rr = 0;
# we can also send the data through IO::Socket::INET module,
# $socket->send($data);
	while (1) {
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
				$write2file = sprintf "[$log_time] recv successful ($datastring) Laenge:%s, $rr\n",length($datastring);
				print_file($logdatei,$write2file) if ($verbose >= 2);
				if (substr($datastring,0,7) eq "# aprsc") {
# received keepalive
#DeinCall>APRS,TCPIP*:					
					++$keepalive;
					if ($keepalive > 90) {
						send_keepalive($aprs_login);
						$keepalive = 0;
					} 
				} else {	
					parse_aprs($datastring);
				}	
			}	
	    }
# check if something to send
		aprs_tx($aprs_txdatei); 
	}	
	$socket->close();

sub parse_aprs {
	my $raw_data = trim_cr($_[0]);
	my $ack  = "";
	my $nn = 0;
	$message_time = act_time();
	$write2file = sprintf "[$message_time] working on: [$raw_data]\n" if ($verbose >= 2);
	print_file($logdatei,$write2file) if ($verbose >= 2);
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
#	my $payload = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*::([\w-]+)[ :]+(.*)\{([\d]+)/i)? $4 : "undef";
	my $payload = ($raw_data =~ /([\w-]+)\>([\w-]+)\,.*::([\w-]+)[ :]+(.*)([\{]*)/i)? $4 : "undef";
	$ack = "";
	if (defined $5) {
		$ack = ($raw_data =~ /.*\{([\d]+)/i)? $1 : "undef";
		if ($ack eq "undef") {
			$ack = ($raw_data =~ /.*(\:ack)([\d]+)/i)? $1 : "undef";
			if ($ack eq ":ack") {
				$write2file = sprintf "[$message_time] no ack to be send\n";
			} else {
				 $write2file = sprintf "[$message_time] new condition: %s\n",$ack;
			}
		} else {	
			$write2file = sprintf "[$message_time] Payload need ack: %s\n",$ack;
		}	
	} else {
		$write2file = sprintf "[$message_time] no ack necessary\n";
	}	
	print_file($logdatei,$write2file);
	if ((defined $1) && (defined $2) && (defined $3) && ($ack ne "") && ($ack ne ":ack")) {
# ack first
		$pckt_nr = $ack;
		printf "PA: [$message_time] Call: %s from %s, Message: %s, Number: %d [%s]\n",$s_destcall,$s_srccall,$payload,$pckt_nr,$2 if ($verbose >= 2);
		send_ack($1,$2,$3,$ack);
	}
	if (($payload ne "undef") && ($ack ne ":ack")){
		$pckt_nr = $ack;
		$s_srccall = $1;
		$s_srcdest = $2;
		$s_destcall = $3;

		$write2file = sprintf "[$message_time] Message to %s from %s: %s (%s)\n",$3,$1,$payload,$ack;
		print_file($logdatei,$write2file);
		print_file($msgdatei,$write2file);
		system('touch', $msgdatei . ".neu");
		if ($payload eq "?aprs?") {
			$write2file = sprintf "[$message_time] Antwort für Payload %s vorbereiten\n",$payload;
			print_file($msgdatei,$write2file);
			open(ANSWER, ">$aprs_txdatei") or do {
						$write2file = sprintf "[$log_time] ERROR in Filehandling ($aprs_txdatei): $!\n";
						print_file($logdatei,$write2file);
						die "ERROR in Filehandling: $!\n"; };
						# die "Fehler bei Datei: $aprs_txdatei\n";
			printf ANSWER "%s\^APRS @ FM-Funknetz Dashbord %s",$s_destcall,$dbv;
			close ANSWER;
			$write2file = sprintf "[$message_time] Antwort (APRS @ FM-Funknetz Dashbord von DL3EL, %s) an %s vorbereitet\n",$dbv,$s_srccall;
			print_file($msgdatei,$write2file);
		}
	} else {
		$write2file = sprintf "no action: [$raw_data]\n" if ($verbose >= 2);
		print_file($logdatei,$write2file) if ($verbose >= 2);
	}
}

sub send_ack {
	my $srccall = $_[0];
	my $srcdest = $_[1];
	my $destcall = $_[2];
	my $tx_ack = (defined $_[3])? ":ack" . $_[3] : "";

		while (length($srccall) < 9) {
			$srccall = $srccall . " ";
		}	
# DL9SAU: answer_message = Tcall + ">" + MY_APRS_DEST_IDENTIFYER + "::" + String(msg_from) + ":ack" + String(q+1);
		$data = sprintf ("%s>%s,WIDE1-1::%s%s\n",$destcall,$srcdest,$srccall,$tx_ack);
#		print $socket "$data\n";
# we can also send the data through IO::Socket::INET module,
		$socket->send($data);
		$log_time = act_time();
#		print LOG "[$log_time] $data";
		$write2file = sprintf "[$log_time] $data";
		print_file($logdatei,$write2file) if ($verbose >= 0);
		$send_trigger = 2;
}

sub send_keepalive {
	my $srccall = $_[0];
	my $srcdest = "TCPIP*";
	my $destcall = "APRS";

# DeinCall>APRS,TCPIP*:
		$data = sprintf ("%s>%s,%s:\n",$srccall,$destcall,$srcdest);
		$socket->send($data);
		$log_time = act_time();
#		print LOG "[$log_time] $data";
		$write2file = sprintf "[$log_time] keepalive $data";
		print_file($logdatei,$write2file) if ($verbose >= 0);
		$send_trigger = 2;
}

sub send_msg {
	my $srccall = $_[0];
	my $srcdest = $_[1];
	my $destcall = $_[2];
	my $pckt_nmbr = $_[3];
	my $data = $_[4];
		while (length($srccall) < 9) {
			$srccall = $srccall . " ";
		}	
# DL9SAU: answer_message = Tcall + ">" + MY_APRS_DEST_IDENTIFYER + "::" + String(msg_from) + ":ack" + String(q+1);
		++$pckt_nr;
		$data = sprintf ("%s>%s,WIDE1-1::%s:%s{%s\n",$destcall,$srcdest,$srccall,$data,$pckt_nr);
#		print $socket "$data\n";
# we can also send the data through IO::Socket::INET module,
		$socket->send($data);
		$log_time = act_time();
#		print LOG "[$log_time] $data";
		$write2file = sprintf "[$log_time] $data";
		print_file($logdatei,$write2file) if ($verbose >= 0);
		$send_trigger = 0;
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
	print "[$log_time] reading config...\n" if ($verbose >= 2);
	my $confdatei = $_[0];
	my $par;
	open(INPUT, $confdatei) or die "Fehler bei Eingabedatei: $confdatei\n";
		) or do {
			$write2file = sprintf "[$log_time] $confdatei kann nicht geöffnet werden: $!\n";
			print_file($logdatei,$write2file);
			$data = "";
		};
	{
	    local $/;#	
	    $data = <INPUT>;
    }    
	close INPUT;
	if ($data) {
		print "Datei $confdatei erfolgreich geöffnet\n" if ($verbose >= 2);
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
		print "config received:\n" if ($verbose >= 2);
		printf "aprs_login:%s\naprs_passwd:%s\naprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 2);
		$write2file = sprintf "[$log_time] current aprs config: aprs_login:%s aprs_passwd:%s aprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 0);
		print_file($logdatei,$write2file) if ($verbose >= 0);
	}
	if (($aprs_login eq "N0CALL-M") ||  ($aprs_login eq "")) {
		if ($fmn_call) {
			$aprs_login = ($fmn_call =~ /([\w]+)[-\w]*/s)? $1 : "undef";
			$write2file = sprintf "[$log_time] take aprs login from dashboard. aprs_login:%s\n", $aprs_login if ($verbose >= 0);
			print_file($logdatei,$write2file) if ($verbose >= 0);
		}
	}
	if (($aprs_login eq "undef") ||  ($aprs_login eq "")) {
		return(0);
	}	
	
	if (($aprs_msg_call eq "N0CALL") ||  ($aprs_msg_call eq "")) {
		$aprs_msg_call = $aprs_login;
	}
	
	if (($aprs_passwd eq "-1") || ($aprs_passwd eq "")) {
		$aprs_passwd = aprs_passcode($aprs_login);
	}	
	printf "aprs_login:%s\naprs_passwd:%s\naprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 2);
	$write2file = sprintf "[$log_time] USING aprs config: aprs_login:%s aprs_passwd:%s aprs_msg_call:%s\n", $aprs_login,$aprs_passwd,$aprs_msg_call if ($verbose >= 0);
	print_file($logdatei,$write2file) if ($verbose >= 0);
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
			$send_trigger = 1;
			($s_destcall,$aprs_msg) = split(/\^/, $data);
			$s_destcall = uc $s_destcall;
			print "Dest $s_destcall, Src: $aprs_login, Text: $aprs_msg\n" if ($verbose >= 2);
			$s_srcdest = "APNFMN";
			send_msg($s_destcall,$s_srcdest,$aprs_login,$pckt_nr,$aprs_msg);
			unlink $aprsdatei;
		}	
	} else {
		print "Datei '$aprsdatei' nicht gefunden.\n" if ($verbose >= 3);
		$send_trigger = 0;
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
