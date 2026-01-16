<?php
include_once "config.php";         
include_once "functions.php"; 
      date_default_timezone_set('Europe/Berlin');
      $jetzt = date("Y-m-d H:i:s");
      $tgdb_File = DL3EL_BASE . "include/tgdb.php";
      $content = file_get_contents('https://FM-Funknetz.de/Download/tgdb_list.txt');
      $contentl = strlen($content);
      $logtext = $jetzt . " " . $tgdb_File . " read, length:" . $contentl . "\n";
      addlog ("L",$logtext);
      file_put_contents($tgdb_File, $content);
      include_once DL3EL_BASE . "include/tg_ext.php";
?>

