<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

    $base_dir_def = '<?php define("DL3EL_BASE", "' . __DIR__ . '/");?>' . "\n";;
    $base_dir_dl3el_def = '<?php define("DL3EL", "' . __DIR__ . '/dl3el");?>' . "\n";;
    $base_dir_file = __DIR__ . "/include/basedir.php";
    if (!$handle = fopen($base_dir_file, 'w')) {
//        die("Cannot open file ($base_dir_file) for writing. Check file permissions.");
//        echo("Cannot open file ($base_dir_file) for writing. Check file permissions. ");
//        echo("find " . __DIR__ . " ! -exec sudo chown svxlink:svxlink {} +");
        exec("find " . __DIR__ . " ! -exec sudo chown svxlink:svxlink {} +");
        if (!$handle = fopen($base_dir_file, 'w')) {
            die("Cannot open file ($base_dir_file) for writing. Check file permissions.");
        }
    }
    fwrite($handle, $base_dir_def);
    fwrite($handle, $base_dir_dl3el_def);
    fclose($handle);

if ((!file_exists('include/config.inc.php')) && (file_exists('include/config.inc.php.example')) ) {
    copy('include/config.inc.php.example', 'include/config.inc.php');
    sleep(3); 
}

if ((!file_exists('include/config.php')) && (file_exists('include/config.php.example')) ) {
    copy('include/config.php.example', 'include/config.php');
    sleep(3); 
}

if ((!file_exists('include/buttons.php')) && (file_exists('include/buttons.php.example')) ) {
    copy('include/buttons.php.example', 'include/buttons.php');
    sleep(3); 
}

include_once "include/settings.php";
include_once "include/functions.php";

if ((defined('DL3EL_NOAUTH')) && (DL3EL_NOAUTH === "yes")) {
// always stay logged on
    $_SESSION['auth'] = "AUTHORISED";
} else {
   if (empty($_SESSION['auth'])) {
      $_SESSION['auth'] = "UNAUTHORISED";
   }
}    

if ((defined('DL3EL_SC_CHANGE')) && (DL3EL_SC_CHANGE === "yes")) {
    $svxConfigFile = SVXCONFPATH."/".SVXCONFIG;
    if (fopen($svxConfigFile,'r')) {
        $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW); 
        if (defined('DL3EL_SC_STRING')) {
            $sc_port_cmp = DL3EL_SC_STRING;
        } else {    
            $sc_port_cmp = "Audio Device";
        }
        $sc_port_linux = 'aplay -l | grep "' . $sc_port_cmp . '"';
        $sc = 'aplay -l | grep "Audio Device"';
        $sc = substr(shell_exec($sc),5,1);
         $sc_port_name = $svxconfig['SimplexLogic']['RX']; 
         $sc_port = substr($svxconfig[$sc_port_name]['AUDIO_DEV'],12,5); 
         if ($sc != $sc_port) {
            echo "<b>Soundcard&nbsp;mismatch:<br>Card:" . $sc . "/" . substr($svxconfig[$sc_port_name]['AUDIO_DEV'],5,8) . "</b>, will be changed";
            $sc_port_raw = $svxconfig[$sc_port_name]['AUDIO_DEV']; 
            echo "Data old: " . $sc_port_raw;
            $sc_port_new = substr($svxconfig[$sc_port_name]['AUDIO_DEV'],0,12) . $sc; 
            echo ", will be changed to Data new: " . $sc_port_new . "<br>";
            $content = file_get_contents($svxConfigFile);
            $backup_filename = $svxConfigFile . "." . date("YmdHis");
            exec('sudo cp -p ' . $svxConfigFile . ' ' . $backup_filename);
            $content = str_replace($sc_port_raw,$sc_port_new,$content); 
            file_put_contents($svxConfigFile, $content);
            echo "done, now restarting svxlink..<br>";
            sleep(1);
            exec('sudo systemctl restart svxlink 2>&1', $screen, $retval);
            if ($retval === 0) {
                echo "SVXLink sucessfull restartet, please reload page";
            } else {
                echo "SVXLink restart failure, check log";
            }
         }   
    }
}
include_once "include/page_top.php";
//<!-- PTT button -->
//<?php 
if (SHOWPTT=="TRUE") {
// now including the volume buttons
    include_once "ptt.html";
}



if ((file_exists('/usr/bin/dvs')) && (defined('DL3EL'))) {
    $dmr_support = "1";
} else {
    $dmr_support = "0";
    $dmrtg = "no DMR";
}    
// for debug, switch off dmr_support
//    $dmr_support = "0";
echo "DVS Support: $dmr_support";

if ($dmr_support) {
    include_once "dvs_code.php";
/*
    $dmrtg = "off";
//    if (defined('DL3EL')) {
######## DVSwitch Stati
    $DMRStatusFile = DL3EL . "/dmr_status";
    $dmrstatus = trim(shell_exec('cat ' . $DMRStatusFile));
    $DVSModeFile = DL3EL . "/dvs_mode";
    $dvsmode = trim(shell_exec('cat ' . $DVSModeFile));
    $color = "blue";
    $colorb = "blue";
//    if (strncmp($dmrstatus, "DMR_FM", 6) === 0) {
    if ($dmrstatus == "DMR_FM") {
        $mode ="DMR_FM";
        $color = "blue";
        $colorb = "green";
    }
//    if (strncmp($dmrstatus, "DMR_only", 8) === 0) {
    if ($dmrstatus == "DMR_only") {
        $mode ="DMR_only";
        $color = "red";
        $colorb = "blue";
    }
    $DMRTGFile = DL3EL . "/dmr_tg";
    $dmrtg = shell_exec('cat ' . $DMRTGFile);

    $DMRdefTG = "9";
    if (defined('DL3EL_DMRTG')) {
        $DMRdefTG = DL3EL_DMRTG;
    }    
    if ($dmrtg == "") {
        $dmrtg = $DMRdefTG;
    }    

//    if (strncmp($dmrstatus, "FM_only", 7) === 0) {
    if ($dmrstatus == "FM_only") {
        $mode ="FM_only";
        $color = "blue";
        $colorb = "blue";
        $dmrtg = "off";
    }

    if (isset($_POST['btn_DMR_FM'])) {
        if ($mode == "FM_only") {
// aktuelle svxlink.conf sollte Kopie von svxlink.conf.dmr_fm sein
            $mode ="DMR_FM";
            $color = "blue";
            $colorb = "green";
            $kanal = "OV F49";
            $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DMR 2>&1";
            exec($command,$screen,$retval);
            $command = "echo '*71#' > /tmp/dtmf_svx";
            exec($command,$screen,$retval);
            $dmrtg = $DMRdefTG;
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            if (defined('DL3EL')) {
                $dmrstatus = "DMR_FM >" . $DMRStatusFile;
                shell_exec("echo $dmrstatus");
                $dmrtgsel = $dmrtg . " >" . $DMRTGFile;
                shell_exec("echo $dmrtgsel");
                $dvsmode = "DMR >" . $DVSModeFile;
                shell_exec("echo $dvsmode");
                $dvsmode = "DMR";
            }    
        }
    }

    if (isset($_POST['btn_DMR_only']))
    {
        if ($mode == "FM_only") {
            $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_fm 2>&1";
            exec($command,$screen,$retval);
        }
        $mode ="DMR_only";
        $color = "red";
        $colorb = "blue";
        $kanal = "OV F49";
        $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_only /etc/svxlink/svxlink.conf 2>&1";
        exec($command,$screen,$retval);
        $command = "sudo service svxlink restart 2>&1";
        exec($command,$screen,$retval);
        $dmrtg = $DMRdefTG;
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
            $dmrstatus = "DMR_only >" . $DMRStatusFile;
            shell_exec("echo $dmrstatus");
            $dmrtgsel = $dmrtg . " >" . $DMRTGFile;
            shell_exec("echo $dmrtgsel");
            $dvsmode = "DMR >" . $DVSModeFile;
            shell_exec("echo $dvsmode");
            $dvsmode = "DMR";
        }    
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DMR 2>&1";
        exec($command,$screen,$retval);
    }

    if (isset($_POST['btn_DMR_91']))
    {
        if (strncmp($mode, "DMR", 3) === 0) {
            $color = "red";
            $kanal = "WW";
            $dmrtg = "91";
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            $DMRTGFile = DL3EL . "/dmr_tg";
            if (defined('DL3EL')) {
                $dmrtgsel = $dmrtg ." >" . $DMRTGFile;
                shell_exec("echo $dmrtgsel");
                $dvsmode = "DMR >" . $DVSModeFile;
                shell_exec("echo $dvsmode");
                $dvsmode = "DMR";
            }    
        }
    }

    if (isset($_POST['btn_DMR_262649']))
    {
        if (strncmp($mode, "DMR", 3) === 0) {
            $color = "red";
            $kanal = "OV F49";
            $dmrtg = "262649";
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            $DMRTGFile = DL3EL . "/dmr_tg";
            if (defined('DL3EL')) {
                $dmrtgsel = $dmrtg ." >" . $DMRTGFile;
                shell_exec("echo $dmrtgsel");
                $dvsmode = "DMR >" . $DVSModeFile;
                shell_exec("echo $dvsmode");
                $dvsmode = "DMR";
            }    
        }
    }
    if (isset($_POST['btn_DMR_only_DISC']))
    {
        if ($mode == "DMR_FM") {
            $command = "echo '*7#' > /tmp/dtmf_svx";
            exec($command,$screen,$retval);
            $command = "echo '*91262649#' > /tmp/dtmf_svx";
            exec($command,$screen,$retval);
        } else {
            if ($dvsmode == "DMR") {
                $command = "/opt/MMDVM_Bridge/dvswitch.sh tune 4000 2>&1";
                exec($command,$screen,$retval);
            }    
            $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_only 2>&1";
            exec($command,$screen,$retval);
            $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_fm /etc/svxlink/svxlink.conf 2>&1";
            exec($command,$screen,$retval);
            $command = "sudo service svxlink restart 2>&1";
            exec($command,$screen,$retval);
        }
        $mode ="FM_only";
        $color = "blue";
        $colorb = "blue";
        $kanal = "disconnect";
        if (defined('DL3EL')) {
            $dmrstatus = "FM_only >" . $DMRStatusFile;
            shell_exec("echo $dmrstatus");
            $dvsmode = "Off >" . $DVSModeFile;
            shell_exec("echo $dvsmode");
            $dvsmode = "OFF";
        }    
        if ($dmr_support == "1") {
            $dmrtg = "off";
        } else {
            $dmrtg = "no DMR";
        }    
    }

// YSF
    if (isset($_POST['btn_YSF_only']))
    {
        if ($mode == "FM_only") {
            $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_fm 2>&1";
            exec($command,$screen,$retval);
        }
        $mode ="DMR_only";
        $color = "red";
        $colorb = "blue";
        $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_only /etc/svxlink/svxlink.conf 2>&1";
        exec($command,$screen,$retval);
        $command = "sudo service svxlink restart 2>&1";
        exec($command,$screen,$retval);
        $dvsmode ="YSF";
        $kanal = " ";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode YSF 2>&1";
        exec($command,$screen,$retval);

        if (defined('DL3EL')) {
           $dvsmode = "YSF >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "YSF";
           $dmrstatus = "DMR_only >" . $DMRStatusFile;
           shell_exec("echo $dmrstatus");
        }    
    }

    if (isset($_POST['btn_YSF']))
    {
        $dvsmode ="YSF";
        $kanal = " ";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode YSF 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "YSF >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "YSF";
        }    
//        echo "YSF: " . $command;
//        echo '<pre>YSF'; print_r($screen); echo '</pre>';
    }


    if (isset($_POST['btn_YSF_DISC']))
    {
        $dvsmode ="YSF";
        $kanal = "disconnect";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune disconnect 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "YSF >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "YSF";
        }    
    }

    if (isset($_POST['btn_YSF_HES']))
    {
        $dvsmode ="YSF";
        $kanal = "Hessen";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune c4fm.dehessen.de:42000 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "YSF >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "YSF";
        }    
    }

    if (isset($_POST['btn_YSF_26269']))
    {
        $dvsmode ="YSF";
        $kanal = "Hessen";
//        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune dc9vq.ysf-deutschland.de/ysf2:42001 2>&1";
//        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune 85.215.138.68:42376 2>&1";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune ysf-deutschland.de:42376 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "YSF >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "YSF";
        }    
    }
// DSTAR
    if (isset($_POST['btn_DSTAR']))
    {
        $dvsmode ="DSTAR";
        $kanal = " ";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DSTAR 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "DSTAR >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "DSTAR";
        }    
    }


    if (isset($_POST['btn_DSTAR_DISC']))
    {
        $dvsmode ="DSTAR";
        $kanal = "disconnect";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune disconnect 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "DSTAR >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "DSTAR";
        }    
//        echo '<pre>D* 1KL'; print_r($screen); echo '</pre>';
    }

    if (isset($_POST['btn_DSTAR_HES']))
    {
        $dvsmode ="DSTAR";
        $kanal = "Hessen";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune DCS001KL 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
           $dvsmode = "DSTAR >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "DSTAR";
        }    
    }
*/
}
    if ((defined('DL3EL_VERSION')) && (strncmp(DL3EL_VERSION, "develop", 7) === 0) && ($dmr_support == "1")) {
    ?>

    <table>
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>">
<!---
        <button name="btn_DMR" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; width:100px; font-size:12px;"><b>DMR ein</b></button>
--->
        <?php 
            echo '<button  style = "border-radius:8px; color:white;border-color:transparent; background-color:orange; height:30px; font-size:12px;"><b>';
            echo $dmrtg . '/';
            echo $mode;
            echo '</b></button>';
        ?>
        &nbsp;&nbsp;&nbsp;
<!---
        <button name="btn_DMR_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:<?php echo $color;?>; height:30px; font-size:12px;"><b>DMR ein</b></button>
--->
        <?php
//            if ($dmr_support == "1") {
                if ($mode == "FM_only") {
                    echo '<button name="btn_DMR_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DMR ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_DMR_FM" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $colorb .'; height:30px; font-size:12px;"><b>DMR Bridge ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_YSF_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>YSF ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }
                if (($mode == "DMR_only")  && ($dvsmode == "DMR")) {
                    echo '<button name="btn_DMR_only_DISC" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DMR aus</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }
                if (($mode == "DMR_only")  && ($dvsmode == "YSF")) {
                    echo '<button name="btn_DMR_only_DISC" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>YSF aus</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }
                if ($mode == "DMR_FM") {
                    echo '<button name="btn_DMR_only_DISC" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DMR Bridge aus</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }    
                if ((($mode == "DMR_only") || ($mode == "DMR_FM")) && ($dvsmode == "DMR")) {
                    echo '<button name="btn_DMR_262649" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; font-size:12px;"><b>OV F49</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_DMR_91" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:blue; height:30px; font-size:12px;"><b>DMR WW</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;';

                    echo '</form>';
                    echo '<form method="post" action="">';

                    echo '<label for="dmrtg">DMR TG</label>';
                    echo '<input type="text" id="dmrtg" name="dmrtg" value="">';
                    echo '<button type="submit">ausw&auml;hlen</button>';
                    echo '<input type="hidden" name="form_submitted" value="1">';
                    echo '</form>';
                }
                if (($mode == "DMR_only")  && ($dvsmode == "YSF")) {
                        include_once "include/dvs_functions.php";
                        getYSFHosts();
                    echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]). '">';
//                if ($mode == "DMR_only") {
                    $DVSStatusFile = DL3EL . "/dvs_status";
                    $dvsstatus = shell_exec('cat ' . $DVSStatusFile);
                    if (strncmp($dvsstatus, "DMR_DSTAR_YSF", 13) === 0) {
//                        echo '<br><button name="btn_YSF" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF</b></button>';
//                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        echo '<br><br><button name="btn_YSF_HES" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF Hessen</b></button>';
                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        echo '	<button name="btn_YSF_26269" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF 26269</b></button>';
                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        echo '	<button name="btn_YSF_DISC" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF Discon</b></button>';
                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        echo '	<button name="btn_DSTAR" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>DSTAR</b></button>';
                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        echo '	<button name="btn_DSTAR_HES" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>D* Hessen</b></button>';
                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        echo '	<button name="btn_DSTAR_DISC" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>DSTAR Discon</b></button>';
                        echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    }
                }    
                echo "<br>DMR Mode: $mode / DVS Mode: $dvsmode";
                echo '</form>';
            }
        ?>
    </table>    
    <?php
//}

    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['form_submitted']) && $dmr_support) {
        if (isset($_POST['dmrtg'])) {
            $dmrtg = $_POST['dmrtg'];
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            if (defined('DL3EL')) {
                $dmrtgsel = $dmrtg . " >" . $DMRTGFile;
                shell_exec("echo $dmrtgsel");
            }    
        }
    }    
    ?>

<?php
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:10px;">'."\n";

    echo '<script type="text/javascript">'."\n";
    echo 'function reloadStatusInfo(){'."\n";
    echo '$("#statusInfo").load("include/status.php",function(){ setTimeout(reloadStatusInfo,3000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadStatusInfo,3000);'."\n";
    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
    echo '<div id="statusInfo" style = "margin-bottom:30px;">'."\n";
    include_once "include/status.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
    echo '</td>'."\n";

    echo '<td valign="top" style = "height:auto;border:none;  background-color:#f1f1f1;">';
    echo '<div class="content">'."\n";
    echo '<script type="text/javascript">'."\n";

/*
    if ((defined('URLSVXRAPI')) && (URLSVXRAPI !="")) {
        echo 'function reloadSVXREF(){'."\n";
        echo '  $("#svxref").load("include/svxref.php",function(){ setTimeout(reloadSVXREF,90000) });'."\n";
        echo '}'."\n";
        echo 'setTimeout(reloadSVXREF,90000);'."\n";
    }
*/
    echo 'function reloadLastHeard(){'."\n";
    echo '  $("#LastHeard").load("include/lh.php",function(){ setTimeout(reloadLastHeard,3000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadLastHeard,3000);'."\n";

    if ($dmr_support == "1") {
        echo 'function reloadLastHeardDMR(){'."\n";
        echo '  $("#LastHeardDMR").load("/DVSwitch/include/lh.php",function(){ setTimeout(reloadLastHeardDMR,3000) });'."\n";
        echo '}'."\n";
        echo 'setTimeout(reloadLastHeardDMR,3000);'."\n";
    }

    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
    echo '<center><div id="LastHeard" style = "margin-bottom:30px;">'."\n";
    include_once "include/lh.php";
    echo '</div></center>'."\n";
    if ($dmr_support == "1") {
        echo '<center><div id="LastHeardDMR" style = "margin-bottom:30px;">'."\n";
        echo '</div></center>'."\n";
    }    
    echo "<br />\n";
/*
    if ((defined('URLSVXRAPI')) && (URLSVXRAPI !="")) {
    echo '<center><div id="svxref" style = "margin-bottom:30px;">'."\n";
    include_once "include/svxref.php";
    echo '</div></center>'."\n";
    }
*/
    echo '</td></tr></table>';
?>

<?php
include_once "include/page_bottom.php";
    ?>

