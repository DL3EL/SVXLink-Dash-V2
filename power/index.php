<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <link href="../css/css.php" type="text/css" rel="stylesheet" />
</head>
<body style = "background-color: #e1e1e1;font: 11pt arial, sans-serif;">
<center>
<!---
<fieldset style = "border:#3083b8 2px groove;box-shadow:5px 5px 20px #999; background-color:#f1f1f1; width:555px;margin-top:15px;margin-left:0px;margin-right:5px;font-size:13px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
//<div style = "padding:0px;width:550px;background-image: linear-gradient(to bottom, #e9e9e9 50%, #bcbaba 100%);border-radius: 10px;-moz-border-radius:10px;-webkit-border-radius:10px;border: 1px solid LightGrey;margin-left:0px; margin-right:0px;margin-top:4px;margin-bottom:0px;line-height:1.6;white-space:normal;">
//<center>
-->
<h1 id="power" style = "color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">Power</h1>

<?php


if (((defined('DL3EL_NOAUTH')) && (DL3EL_NOAUTH === "YES")) || ($_SESSION['auth'] === 'AUTHORISED')) {

if (!isset($svx_include)) {
    include_once "../include/settings.php";
}


// load the connlist
$retval = null;
$conns = null;

if (isset($_POST['btnPower']))
    {

        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';
        $logtext = "Device Shutdown from $ip \n";
        addsvxlog($logtext);
        $retval = null;
        $screen = null;
        //$sAconn = $_POST['sAconn'];
        //$password = $_POST['password'];
        //exec('sudo nmcli dev wifi rescan');
        $command = "sudo shutdown -h now 2>&1";
        exec($command,$screen,$retval);
}

//if (isset($_POST['btnLcd']))
//    {
//
//        $retval = null;
//        $screen = null;
//        //$sAconn = $_POST['sAconn'];
//        //$password = $_POST['password'];
//        //exec('sudo nmcli dev wifi rescan');
//        $command = "sudo systemctl restart oled2svx  2>&1";
//        exec($command,$screen,$retval);
//}

if (isset($_POST['btnSvxlink']))
    {

        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';
        $logtext = "SVXLink Restarted from $ip \n";
        addsvxlog($logtext);
        $retval = null;
        $screen = null;
        //$sAconn = $_POST['sAconn'];
        //$password = $_POST['password'];
        //exec('sudo nmcli dev wifi rescan');
        if (defined('SVXNAME')) {
                $svxlink = SVXNAME;
        } else {        
                $svxlink = "svxlink";
        }
        $command = "sudo systemctl restart " . $svxlink . " 2>&1";
        exec($command,$screen,$retval);
}

if (isset($_POST['btnSvxRef']))
    {

        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';
        $logtext = "SVXReflector Restarted from $ip \n";
        addsvxlog($logtext);
        $retval = null;
        $screen = null;
        //$sAconn = $_POST['sAconn'];
        //$password = $_POST['password'];
        //exec('sudo nmcli dev wifi rescan');
        $command = "sudo systemctl restart svxreflector 2>&1";
        exec($command,$screen,$retval);
        $logtext = "svxreflector restart (manual)\n";
        addsvxlog($logtext);
}

if (isset($_POST['btnSvxlinkoff']))
    {

        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';
        $logtext = "SVXLink shutdown from $ip \n";
        addsvxlog($logtext);
        $retval = null;
        $screen = null;
        //$sAconn = $_POST['sAconn'];
        //$password = $_POST['password'];
        //exec('sudo nmcli dev wifi rescan');
        if (defined('SVXNAME')) {
                $svxlink = SVXNAME;
        } else {        
                $svxlink = "svxlink";
        }
        $command = "sudo systemctl stop " . $svxlink . " 2>&1";
        exec($command,$screen,$retval);
}

if (isset($_POST['btnRestartAPRS'])) {
// if this file exists, aprs task will terminate, status.php will start it again
        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
        $logtext = "APRS Restarted from $ip \n";
        addsvxlog($logtext);
        $retval = null;
        $aprs_exit = DL3EL . "/aprs.exit";
        touch($aprs_exit);
        $logtext =  "APRS Dienst gestoppt, Restart automatisch durch Dashboard\n";
        addsvxlog($logtext);
    }  

if (isset($_POST['btnRestartMQTT'])) {
        $command = "sudo killall fmn-mqtt.pl 2>&1";
        exec($command,$screen,$retval);
        $logtext =  "MQTT Dienst gestoppt, Restart automatisch durch Dashboard\n";
        addsvxlog($logtext);
    }  

if (isset($_POST['btnRestartDSTAR'])) {
        $command = DL3EL . "/st_dstar_only.sh 2>&1";
        exec($command,$screen,$retval);
        $logtext =  "D-Star Hotspot neu gestartet ($command)\n";
        addsvxlog($logtext);
    }  


if (isset($_POST['btnRestart']))
    {

        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
        $logtext = "Device Restarted from $ip \n";
        addsvxlog($logtext);
        $retval = null;
        $screen = null;
        //$sAconn = $_POST['sAconn'];
        //$password = $_POST['password'];
        //exec('sudo nmcli dev wifi rescan');
        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
        $logtext = "system shutdown, manual, by $ip\n";
        addsvxlog($logtext);
        $command = "sudo shutdown -r now 2>&1";
        exec($command,$screen,$retval);
}

if (isset($_POST['btnrstshari']))
    {
// wichttig: damit das funktioniert, muss mit visudo folgendes eingetragen werden
// svxlink        ALL=(ALL) NOPASSWD: /usr/sbin/alsactl

        $retval = null;
        $screen = null;
        $command = "/home/svxlink/dl3el/shari-arestore.sh  2>&1";
        exec($command,$screen,$retval);
}

if (isset($_POST['btnDashUpdate']))
    {
        update_db();

}

if (isset($_POST['btnrstc710']))
    {
        $retval = null;
        $screen = null;
        $command = "/home/svxlink/dl3el/c710-arestore.sh  2>&1";
        exec($command,$screen,$retval);
}

if (isset($_POST['btnCleanUp']))
    {

        $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';
        $logtext = "System Cleanup enforced from $ip \n";
        addsvxlog($logtext);
        $cron_File = DL3EL . "/crontab.log";
	    $cron = start_cron($cron_File,$callsign,$fmnetwork,"F");
	    touch($cron_File);
}

} else {
  echo '<h1 id="power" style = "color:#00aee8;font: 18pt arial, sans-serif;font-weight:bold; text-shadow: 0.25px 0.25px gray;">You are not authorised to make changes here.</h1>';
 
}
    $dbversionFile = DL3EL . "/dbversion.upd";
    $update_available = 0;
    if (file_exists($dbversionFile)) {
        $content = file_get_contents($dbversionFile);
        if ($content === "update") {
            $update_available = 1;
        }
    }

?>

<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>"> 
	
	<button name="btnSvxlinkoff" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Stop SVXlink Service</button>
	<br>
	<button name="btnSvxlink" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart SVXlink Service</button>
	<br>
	<!--button name="btnLcd" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart Lcd Service</button>
	<BR-->
	<button name="btnRestartAPRS" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart APRS Task</button>
        <br>
	<button name="btnRestartMQTT" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart MQTT Task</button>
        <br>
	<button name="btnRestart" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart Device</button>
        <br>
	<button name="btnPower" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Power OFF</button>
<?php
   if (file_exists('/home/pi/DMR-Hotspot/start/st_dstar_only.sh')) {
        echo '<button name="btnRestartDSTAR" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart D-Star Hotspot</button>';
        echo '<br>';
   }
   if ((file_exists('/var/log/svxreflector')) || (file_exists('/var/log/svxreflector.log'))) {
        echo '<button name="btnSvxRef" type="submit" class="red" style = "height:30px; width:400px; font-size:12px;">Restart SVXReflector Service</button>';
        echo '<br>';
   }
/*
   if (defined('DL3EL_RADIO') && (strncmp(DL3EL_VERSION, "develop", 7) === 0)) {
      $svxRadio = DL3EL_RADIO;
      if ($svxRadio == "Shari") {
        echo '<br><br><br>';
        echo '<button name="btnrstshari" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Reset Sound Shari</button>';
      }    
      if ($svxRadio == "C710") {
        echo '<br><br><br>';
        echo '<button name="btnrstc710" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Reset Sound C710</button>';
      }    
   }   
*/ 
    $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
    $logtext="Power menu started from $ip\n";
    addsvxlog($logtext);

    if (((defined('DL3EL_BASE')) && (file_exists(DL3EL_BASE.'git_pull.sh'))) && ((defined('DL3EL_GIT_UPDATE')) && ((DL3EL_GIT_UPDATE === "yes") || (DL3EL_GIT_UPDATE === "nocheck")))) {
        //$cmd = "wget -T 10  -O versioncheck https://github.com/DL3EL/SVXLink-Dash-V2/raw/refs/heads/main/dl3el/dbversion";
        //echo "",exec($cmd, $output, $retval);
        //$content = trim(shell_exec('cat versioncheck'));
        $content = file_get_contents('https://github.com/DL3EL/SVXLink-Dash-V2/raw/refs/heads/main/dl3el/dbversion') . ".g";

        if ((defined ('debug')) && (debug > 0)) {
            echo "<br><b>Debug ist eingeschaltet, kein Update möglich<br>Folgende Fehlermeldungen ignorieren und debug ausschalten<br></b>";
            $content = ".g";
        }    
        if ($content === ".g")  {
            $git_file  = "https://github.com/DL3EL/SVXLink-Dash-V2/raw/refs/heads/main/dl3el/dbversion";
            echo "<br><br><br>Github Versionprüfung konnte nicht ausgeführt werden. Die Datei <br><a href='$git_file' target='git'>$git_file</a> <br>konnte nicht gelesen werden<br";
        } else {       
            list($gitversion, $rest) = explode(" ", $content);
            $gitversionf = $gitversion;
            file_put_contents("gitversion",$gitversionf);
        }
        $dbversionFile = DL3EL . "/dbversion";
        $dbversion = file_get_contents($dbversionFile);
        list($version, $rest) = explode(" ", $dbversion);
        if ($content === ".g")  {
// version read not possible
            $gitversion = $content;
            echo "<br><br><br>Bitte auf den Link klicken und auf die Fehlermeldung im neuen Tab achten. Es könnte z.B. an PiHole o.ä. liegen.<br";
            if ((defined('DL3EL_GIT_ENFORCE')) && (DL3EL_GIT_ENFORCE === "yes")) {
                echo '<br><br><button name="btnDashUpdate" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Dashboard Update (GitHub)</button>';
            }
        } else {
            echo '<br><br><br>';
            if ($gitversion !== $version) {
                echo "<br>Github Version:$gitversion installierte Version:$version, bitte Update ausführen<br>";
                echo '<button name="btnDashUpdate" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Dashboard Update (GitHub) auf Version ' . $gitversion . '</button>';
            } else {
                if (DL3EL_GIT_UPDATE === "nocheck") {
                    echo "<br>Github Version:$gitversion installierte Version:$version,<br>Versionscheck abgeschaltet, Update ist möglich<br>";
                    if ((defined('DL3EL_VERSION')) && (DL3EL_VERSION === "develop")) {
                        echo '<button name="btnDashUpdate" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Dashboard Update (GitHub) auf Version ' . $gitversion . '</button>';
                    } else {
                        echo "ACHTUNG: nur für erfahrene Benutzer, es kann sein, dass noch nicht freigegebene Updates heruntergeladen werden, die das System beschädigen<br>";
                        echo '<button name="btnDashUpdate" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Dashboard Update (GitHub) auf Version ' . $gitversion . '</button>';
                    }        
                } else {
                    if ($update_available) {
                        echo '<button name="btnDashUpdate" type="submit" class="green" style = "height:30px; width:400px; font-size:12px;">Dashboard Update (GitHub) auf Version ' . $gitversion . '</button>';
                    } else {
                        echo "<br>Github Version:$gitversion entspricht der installierten Version:$version, kein Update notwendig<br>";
                        echo "<br><br>";
                    }    
                }
            }  
            handle_cleanup();
            echo "<br>Der Cleanup Lauf läuft automatisch alle 24 Stunden.<br>Es ist normalerweise nicht notwendig, einen Cleanup Lauf manuell anzustarten.<br>";
            echo "Sollte es im System Unregelmäffigkeiten geben und ein adhoc Cleanup Lauf wird empfohlen, kann dieser hier gestartet werden.<br>";
            echo '<button name="btnCleanUp" type="submit" class="bleu" style = "height:30px; width:400px; font-size:12px;">jetzt CleanUp starten</button>';
        }
  }

?>   
</form>

<p style = "margin: 0 auto;"></p>
<p style = "margin-bottom:-2px;"></p>

</fieldset></center>
</body>
</html>
