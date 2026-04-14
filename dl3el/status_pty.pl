#!/usr/bin/perl
use strict;
use warnings;

my $input_pipe = '/tmp/state_smp';
my $output_file = '/tmp/svxlink_last_state.txt';

# Öffne die Pipe zum Lesen (benötigt sudo/Rechte für /tmp/state_smp)
open(my $fh, "<", $input_pipe) or die "Konnte $input_pipe nicht öffnen: $!";

while (my $line = <$fh>) {
    chomp($line);
    
    my $status = "";
    
    # Extrahiere Typ und Status
    if ($line =~ /Rx:sql_state.*"sql_open":(true|false)/) {
        $status = "RX: " . ($1 eq 'true' ? "OPEN" : "CLOSED");
    }
    elsif ($line =~ /MultiTx:state.*"transmit":(true|false)/) {
        $status = "TX: " . ($1 eq 'true' ? "ON" : "OFF");
    }

    # Nur schreiben, wenn eine relevante Statusänderung erkannt wurde
    if ($status) {
        open(my $out, ">", $output_file) or warn "Schreibfehler: $!";
        print $out "$status\n";
        close($out);
        
        # Optional: Ausgabe in der Konsole zum Debuggen
        print "Aktueller Status: $status\n";
    }
}
