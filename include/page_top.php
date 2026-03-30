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
    <meta name="KeyWords" content="SVXLink,G4NAB, SP2ONG, SP0DZ, DL3EL" />
    <meta http-equiv="cache-control" content="max-age=0" />
    <meta http-equiv="cache-control" content="no-cache, no-store, must-revalidate" />
    <meta http-equiv="expires" content="0" />
    <meta http-equiv="pragma" content="no-cache" />
    <meta charset="UTF-8">
<link rel="shortcut icon" href="images/favicon.ico" sizes="16x16 32x32" type="image/png">    

<?php echo ("<title>" . $callsign . " Dashboard " . $dbversion . "</title>" ); ?>

<?php include_once "include/browserdetect.php"; ?>
<?php include_once "include/functions.php"; ?>

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
<?php 
//    if ((!strlen($fmnetwork)) || (!strlen($nodeinfo))) {
    if ((empty($fmnetwork)) || (empty($nodeinfo))) {
        $nodeinfo = "";
        $fmnetwork = getfmnetwork($nodeinfo);
    }     
?>
<center><p style = "margin-top:5px;margin-bottom:0px;">
<span style = "font-size: 32px;letter-spacing:4px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $callsign;?></span>
<p style = "margin-top:0px;margin-bottom:0px;">
<span style = "font-size: 18px;letter-spacing:4px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $fmnetwork; ?></span>
<span style = "font-size: 12px;letter-spacing:2px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"> 
    <a href="https://github.com/DL3EL/SVXLink-Dash-V2" target="github" style="color:#ffffff;">
    <?php echo $dbversion; ?>
    </a>
</span>

</p>
<?php include_once "include/page_top_wx.php"; ?>
</center>
<div class="img" style = "padding-right:30px"><a href="https://wiki.fm-funknetz.de/" target="fm-funknetz"><img src="images/fmn-logo.png" /></a></div>
</div></div>
    <div class="text"style = "padding-right:230px">
</div>
<?php
    if ((defined('DL4EM_TOPMENU')) && (DL4EM_TOPMENU === "yes")) {
        include_once "select_menu.php"; 
        if ($menuType == "dropdown") {
            include_once "top_menu_neu.php"; 
        } else {
            include_once "top_menu.php"; 
        }    
    } else {
        include_once "top_menu.php"; 
    }
?>
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
//if (SHOWPTT=="TRUE") {
if ((defined('SHOWPTT')) && (SHOWPTT=="TRUE")) {
    if ((defined('DL3EL_MIKE_TYPE')) && (DL3EL_MIKE_TYPE === "QX18")) {
        $command = "pgrep -f 'ptt_listener.py'";
        exec($command, $output, $returnCode);
        $procs = count($output);
        if ($procs <2) {
            $cmd = "sudo -u svxlink python " . DL3EL . "/ptt-qx18/ptt_listener.py >/dev/null &";
            if ((defined ('debug')) && (debug > 0)) echo "Starting QX18 ";
            exec($cmd, $output, $retval);
            $logtext =  "QX18 Listener gestartet (" . $cmd . ")\n";
            addsvxlog($logtext);
        }    
        include_once "ptt-qx18.html";
    } else {
        include_once "ptt.html";
    }        
//        include_once "ptt-qx18.html";
//        include_once "ptt.html";
}
/*
// now including the volume buttons
    if ((defined('DL3EL_MIKE_TYPE')) && (DL3EL_MIKE_TYPE === "QX18")) {
        include_once "ptt-qx18.html";
    } else {
        include_once "ptt.html";
    }        
}
*/
if ((file_exists('/etc/systemd/system/svxlink-node.service')) && ($_SESSION['auth'] === "AUTHORISED")) {
    echo '<button class="button link" onclick="playAudioToggle(8001, this)">
    <b><img src="images/speaker.png" alt="" style="vertical-align:middle">&nbsp;RX Monitor</b>
    </button>';
}

    if (file_exists(DL3EL.'/Reflector1.conf')) {
        include_once "ref_change.php";
    }
   
///
?>

<?php
if (MENUBUTTON=="TOP") {
include_once "buttons.php"; 
}
?>
</body> 
