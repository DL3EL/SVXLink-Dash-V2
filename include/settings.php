<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
$progname = basename($_SERVER['SCRIPT_FILENAME'],".php");
include_once "config.php";
include_once "functions.php";
include_once "tools.php";
$dbversionFile = DL3EL . "/dbversion";
$dbversion = shell_exec('cat ' . $dbversionFile);
$uknetwork = "uk.wide.svxlink.uk";
$uknetwork_state = "ukwide.svxlink.net";
$uknetwork_lh = "https://svxlink.qsos.uk/status";
$nanetwork = "north.america.svxlink.net";
$nanetwork_state = "north.america.svxlink.net";
$nanetwork_lh = "https://north.america.svxlink.net/status.php?json";
?>
