#!/usr/bin/perl

use warnings;
use strict;
use utf8;
use Time::Piece;
eval {
# needs: sudo cpan Net::MQTT::Simple
    require Net::MQTT::Simple;
};	
if($@) {
    print "Folgender Fehler ist aufgetreten: $@\n"; 
    exit;
}

# Variables:
my $dir = "";
my $conf = "";
my $entry;
my @array;
my $login = "";
my $verbose = 0;
my $log_time = "";
my $write2file = "";
my $tm = localtime(time);
my $version = "1.01";
# later
my $mqtt_exit_datei = $dir  . "mqtt.exit";

 
    $dir = ($0 =~ /(.*)fmn-mqtt.pl/i)? $1 : "undef";
    $conf = $dir . "mqtt.conf";
    printf "DL3EL FM-Funknetz MQTT Receiver (blocking) [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV $dir $conf\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year if ($verbose >= 0);
    my $logdatei = $dir  . "mqtt.log";
    printf "LOG: %s Logdatei: %s\n",$dir,$logdatei if ($verbose >= 1);
    STDOUT->autoflush(1);
#    open(LOG, ">>$logdatei") or die "Fehler bei Logdatei: $logdatei\n";
#    close LOG;
    $write2file = sprintf "DL3EL FM-Funknetz MQTT Receiver (blocking) [v$version] Start: %02d:%02d:%02d am %02d.%02d.%04d\n$0 @ARGV $dir $conf\n",$tm->hour, $tm->min, $tm->sec, $tm->mday, $tm->mon,$tm->year;
    print_file($logdatei,$write2file) if ($verbose >= 0);
    $log_time = act_time();

    my $total = $#ARGV + 1;
    my $counter = 1;
    foreach my $a(@ARGV) {
	print "Arg # $counter : $a\n" if ($verbose == 7);
	$counter++;
	if (substr($a,0,2) eq "v=") {
	    $verbose = substr($a,2,1);
	    print "Debug On, Level: $verbose\n" if $verbose;
	} 
	if (substr($a,0,2) eq "d=") {
	    $dir = substr($a,2,length($a-2));
	    $ dir = $dir . "/dl3el";
	    print "dir: $dir\n" if $verbose;
	}
    }

my $mqttdatei = $dir  . "mqtt.data";

# init mqtt
# Connect to broker
my $mqtt_client_id = sprintf "%s%s", "DL3EL-Dashboard",time();;
$ENV{MQTT_CLIENT_ID} = $mqtt_client_id;
my $mqtt = Net::MQTT::Simple->new('FM-Funknetz.de');
    STDOUT->autoflush(1);
    print "MQTT started mit Log: $logdatei um $log_time (ClientID:$mqtt_client_id)\n";
    $log_time = act_time();
    $write2file = sprintf "[$log_time] DL3EL MQTT RCVR (ClientID:%s) MQTT started\n",$mqtt_client_id;
    print_file($logdatei,$write2file) if ($verbose >= 0);
 
# Depending if authentication is required, login to the broker
#if($mqtt_username and $mqtt_password) {
#    $mqtt->login($mqtt_username, $mqtt_password);
#}
# aktueller tx Status:
    open(LOG, ">>$logdatei") or die "Fehler bei Logdatei: $log_time\n";
    $mqtt->subscribe("/server/statethr/1/#", \&received);
    print LOG "MQTT Subscribed: server/statethr/1/#\n";
    print "MQTT Subscribed: server/statethr/1/#\n";
    # Incoming message on topic /server/mqtt/parrot/9990/analysis 
    $mqtt->subscribe("server/mqtt/parrot/9990/analysis/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/parrot/9990/analysis/#\n";
    print "MQTT Subscribed: server/mqtt/parrot/9990/analysis/#\n";
    #Incoming message on topic /server/mqtt/parrot/9990/status 
    $mqtt->subscribe("server/mqtt/parrot/9990/status/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/parrot/9990/status/#\n";
    print "MQTT Subscribed: server/mqtt/parrot/9990/status/#\n";
    #Incoming message on topic /server/mqtt/heartbeat 
    $mqtt->subscribe("/server/mqtt/heartbeat/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/heartbeat/#\n";
    print STDOUT "MQTT Subscribed: server/mqtt/heartbeat/#\n";
    $mqtt->subscribe("/server/state/loginz/#", \&received);
    print LOG "MQTT Subscribed: server/state/loginz/#\n";
    print STDOUT "MQTT Subscribed: server/state/loginz/#\n";
    $mqtt->subscribe("/server/mqtt/parrot/9990/analysis/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/parrot/9990/analysis/#\n";
    $mqtt->subscribe("/server/mqtt/parrot/9990/status/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/parrot/9998/status/#\n";
    $mqtt->subscribe("/server/mqtt/parrot/9998/analysis/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/parrot/9998/analysis/#\n";
    $mqtt->subscribe("/server/mqtt/parrot/9998/status/#", \&received);
    print LOG "MQTT Subscribed: server/mqtt/parrot/9998/status/#\n";
    close LOG; 

# [20.02.2026 11:45:16] /server/statethr/1: {"time":"11:45:15", "talk":"stop", "call":"EchoLink", "tg":"2", "server":"1"}^1771584316^
    $write2file = sprintf "[$log_time] DL3EL^FM-Funknetz MQTT Interface ^started^%s^\n",,time() if ($verbose >= 1);
    $write2file = sprintf "[$log_time] %s: %s^%s^\n","DL3EL","MQTT Start",time() if (!$verbose);
    print_file($mqttdatei,$write2file) if ($verbose >= 0);
# This call blocks, and runs the callback, when a message arrives
$mqtt->run();
 
$mqtt->disconnect();
# This is the callback function, when a message arrives for the subscribed topic

sub received 
{
    my ($topic, $message) = @_;
        print "Incoming message on topic $topic is: $message  [$mqttdatei]\n" if ($verbose >= 1);
        print "$topic: $message\n" if (!$verbose);
	$log_time = act_time();
	$write2file = sprintf "[$log_time] Incoming message on topic %s is: %s^%s^\n",$topic,$message,time() if ($verbose >= 1);
	$write2file = sprintf "[$log_time] %s: %s^%s^\n",$topic,$message,time() if (!$verbose);
	print_file($mqttdatei,$write2file) if ($verbose >= 0);

}

sub print_file {
	my $file = $_[0];
	my $data = $_[1];
	
	open(FILE, ">>$file") or die "Fehler bei Logdatei: $file\n";
#	select((select(FILE), $| = 1)[0]); 
#	print "[PF] File $file open\n" if ($verbose >= 1);
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
