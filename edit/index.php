<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "../include/config.php";
?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <link href="../css/css.php" type="text/css" rel="stylesheet" />

</head>
<body style = "background-color: #e1e1e1;font: 11pt arial, sans-serif;">

<center>
<h1 id="conf-editor" style = "color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;"
<?php

// Get filename from query parameter
$file = $_GET['file']; 
$log = 0;

if (((defined('DL3EL_NOAUTH')) && (DL3EL_NOAUTH === "yes")) || ($_SESSION['auth'] === 'AUTHORISED')) {
  // ok, go ahead, set to authorized :-)
  $_SESSION['auth'] = "AUTHORISED";
} else {
    if ($file === "info") {
      echo "ausnahmsweise";
    } else {
      echo "not authorized";
      return;
    }  
}  

if (($file == "log") || ($file == "ref") || ($file == "msg") || ($file == "info")) {
  if (($file == "log")  || ($file == "ref")) {
    if ($file == "log") {
      $file = SVXLOGPATH . SVXLOGPREFIX;
    } else {  
      $file = SVXLOGPATH . "svxreflector";
	    if (!file_exists($file)) {
        $file = SVXLOGPATH . "svxreflector.log";
      }
    }
    $zipfile = $file . ".1.gz";
    $log = 1;
    if (!filesize($file)) {
//      $zipfile = SVXLOGPATH . SVXLOGPREFIX . ".1.gz";
      if (file_exists($zipfile)) {
// to getthis working, you have to add 
// svxlink ALL=NOPASSWD: /usr/bin/gunzip
// in the file www-data.sudoers in the dashboards root directory, copy that file to /etc/sudoers.d/svxlink and restart the apache
        exec('sudo gunzip ' . $zipfile,$output,$retval);
        if ($retval === 0) {
          echo "unzip sucessfull:";
//          $file = SVXLOGPATH . SVXLOGPREFIX . ".1";
          $file = $file . ".1";
          echo $file;
        } else {
          echo "unzip failure";
        }
      } else {
//          $file = SVXLOGPATH . SVXLOGPREFIX . ".1";
          $file = $file . ".1";
      }
    }
    echo ">Log Display: " . $file . " (reverse order)</h1>";
  } else {
    if ($file == "msg") {
      $file = DL3EL . "/aprs-is.msg";
      $log = 1;
      echo ">Message Display: " . $file . " (reverse order)</h1>";
    } else {  
      $file = DL3EL . "/readme.info";
      $log = 2;
      echo ">Info Display: " . $file . " </h1>";
    }
  }  
} else {
    echo ">Expert-Editor " . $file . "</h1>";
}

echo '<script type="text/javascript">
        function reloadPage() {
            window.location.href = window.location.pathname + "?reloaded=true&file=' . $file .'";
        }
    </script>';

include_once "../include/functions.php";
include_once "../include/config.php";

// Read file contents
if ($log) {
//    $command = "tac $file > " . DL3EL . "/logfile.txt";
//    passthru($command);
//    $file = DL3EL . "/logfile.txt";
/////////////////////////// ohne Hilfsdatei logfile.txt // tnx Frank DL4EM  ///////////////////
// Read Log File content
      $content = file_get_contents($file);
// reverse Log File reverse
  if ($log === 1) {
      $zeilen_array = explode("\n", $content);
      $umgekehrte_zeilen = array_reverse($zeilen_array);
      $content = implode("\n", $umgekehrte_zeilen);
  }    
} else {   
  $content = file_get_contents($file);
}  
// Display in textarea & edit
echo '<form method="post">';
echo '<textarea name="content" rows="35" cols="120">' . htmlspecialchars($content) . '</textarea><br>';

if (!$log) {
  echo '<div style = "text-align:center">Make your changes here</div>';  
  if (substr($file, -3) !== "php") {
// Save button
    echo '<input type="submit" name="save" value="Save only">';  
    echo '&nbsp;&nbsp;';
// Save &reload button
    echo '<input type="submit" name="save_reload" value="Save & ReLoad">';  
  } else {
    echo '<input type="submit" name="save" value="Save">';  
    echo '&nbsp;&nbsp;';
    echo "after saving, please reload the dashboard to see the changes. Reloading svxlink is not necessary";
  }    
  echo '&nbsp;&nbsp;';
// cancel button
  echo '<input type="submit" name="cancel" value="Cancel">';  
  echo '</form>';

  // Save / Reload on submit//
  if ((isset($_POST['save'])) || (isset($_POST['save_reload']))) {

    // Backup file
    $backup_filename = $file . "." . date("YmdHis");
    exec('sudo cp -p ' . $file . ' ' . $backup_filename);
    // eliminate \r (when editing in a windows browser
    $content = str_replace("\r\n","\n",$_POST['content']); 
    file_put_contents($file, $content);

    echo "<script type='text/javascript'> reloadPage(); </script>";
    if (isset($_POST['save_reload'])) {
  // Reload on submit//
      if ($file == "/etc/svxlink/svxreflector.conf") {
        $prog = "svxreflector";
        $command = "sudo systemctl restart svxreflector 2>&1";
      } else {
        $prog = "svxlink";
        $command = "sudo systemctl restart svxlink 2>&1";
      }    
      echo "restarting $prog ...";
      sleep(1);
      exec($command,$screen,$retval);
      if ($retval === 0) {
        echo "$prog sucessfull restartet";
      } else {
        echo "$prog restart failure, check log";
      }
    }   
  }
}
?>
</body>
</html>
