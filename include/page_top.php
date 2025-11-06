<!DOCTYPE html >
<html>
<head>
    <meta name="robots" content="index" />
    <meta name="robots" content="follow" />
    <meta name="language" content="English" />
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta name="generator" content="SVXLink" />
    <meta name="Author" content="G4NAB, SP2ONG, SP0DZ" />
    <meta name="Description" content="Dashboard for SVXLink by G4NAB, SP2ONG, SP0DZ, DL3EL" />
    <meta name="KeyWords" content="SVXLink,G4NAB, SP2ONG, SP0DZ" />
    <meta http-equiv="cache-control" content="max-age=0" />
    <meta http-equiv="cache-control" content="no-cache, no-store, must-revalidate" />
    <meta http-equiv="expires" content="0" />
    <meta http-equiv="pragma" content="no-cache" />
<link rel="shortcut icon" href="images/favicon.ico" sizes="16x16 32x32" type="image/png">    

<?php echo ("<title>" . $callsign . " Dashboard " . $dbversion . "</title>" ); ?>

<?php include_once "include/browserdetect.php"; ?>
    <script type="text/javascript" src="scripts/jquery.min.js"></script>
    <script type="text/javascript" src="scripts/functions.js"></script>
    <script type="text/javascript" src="scripts/pcm-player.min.js"></script>
<!---- ## PTT --->
<!----
    <script type="text/javascript" src="../scripts/ptt.js"></script>
--->
    <script type="text/javascript">
      $.ajaxSetup({ cache: false });
    </script>
    <link href="css/featherlight.css" type="text/css" rel="stylesheet" />
    <script src="scripts/featherlight.js" type="text/javascript" charset="utf-8"></script>
<!----
<link rel="stylesheet" href="">
 <script type="text/javascript">
        function reloadPage() {
            window.location.href = window.location.pathname + "?reloaded=true";
        }
    </script>
--->
<link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">
</head>
<body style = "background-color: #e1e1e1;font: 11pt arial, sans-serif;">
<center>
<fieldset style = "box-shadow:5px 5px 20px #999; background-color:#f1f1f1; width:0px;margin-top:15px;margin-left:0px;margin-right:5px;font-size:13px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
<div class="container"> 
<div class="header">
<div class="parent">
    <div class="img" style = "padding-left:30px"><img src="images/svxlink.ico" /></div>

<!---
    <div class="text"style = "padding-right:230px">
--->    
<center><p style = "margin-top:5px;margin-bottom:0px;">
<span style = "font-size: 32px;letter-spacing:4px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $callsign;?></span>
<p style = "margin-top:0px;margin-bottom:0px;">
<span style = "font-size: 18px;letter-spacing:4px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $fmnetwork; ?></span>
<span style = "font-size: 12px;letter-spacing:2px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"> 
    <a href="https://github.com/DL3EL/SVXLink-Dash-V2" target="github" style="color:#ffffff;">
    <?php echo $dbversion;
        if ((defined('DL3EL_VERSION')) && (DL3EL_VERSION === "develop")) {
            $cmd = "wget -q -O " . DL3EL . "/dbwget.log \"http://relais.dl3el.de/cgi-bin/db-stat.pl\"";
            if ((defined ('debug')) && (debug > 4)) echo "Stat: $cmd<br>";
            exec($cmd);
            $dbstatFile = DL3EL . "/dbwget.log";
            $dbstat = file_get_contents($dbstatFile);
            echo " (" . $dbstat . ")";
        }
     ?>
    </a>
</span>

</p></center>
</div></div>
    <div class="text"style = "padding-right:230px">
</div>
<?php include_once "top_menu.php"; ?>
<div class="content"><center>
<div style = "margin-top:0px;">
</div></center>
</div>
<?php
// klingt seltsam
//if (isProcessRunning('node')) {
//echo '&nbsp;&nbsp;<button class="button link" onclick="playAudioToggle(8080, this)"><b>&nbsp;&nbsp;&nbsp;<img src=images/speaker.png alt="" style = "vertical-align:middle">&nbsp;&nbsp;RX Monitor&nbsp;&nbsp;&nbsp;</b></button><br><br>';
//}
?>
<!-- PTT button -->
<?php 
if (SHOWPTT=="TRUE") {
// now including the volume buttons
    include_once "ptt.html";
}
?>

<?php
if (MENUBUTTON=="TOP") {
include_once "buttons.php"; 
}
?>
