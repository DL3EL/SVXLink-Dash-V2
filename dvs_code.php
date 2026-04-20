<?php
    $dmrtg = "off";
//######## DVSwitch Stati
/*
$mode = FM_only | DVS_only | DMR_FM   -> steuert das kopieren con .conf 
	DVS_only = dvs Modes
	FM_only normal FM
	DMR_FM = Bridge Mode : normale FM .conf Datei dmr_fm
    * [DVLink]
        CONNECT_LOGICS = SimplexLogic:7:,UsrpLogic
        DEFAULT_ACTIVE = 0
    DEFAULT_ACTIVE muss irgendwie 1 werden. Muss noch getestet werden

$dvsmode = OFF | DMR | YSF | DSTAR

//                    $DVSStatusFile = DL3EL . "/dvs_status";
//                    $dvsstatus = shell_exec('cat ' . $DVSStatusFile);
*/
    $svxConfigFile = SVXCONFPATH."/".SVXCONFIG;
    if (fopen($svxConfigFile,'r')) {
        $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW); 
//      print_r($svxconfig);  
        $file = isset($svxconfig['File_Description']['FILE']) ? $svxconfig['File_Description']['FILE'] : 'no';     
        $function = isset($svxconfig['File_Description']['FUNCTION']) ? $svxconfig['File_Description']['FUNCTION'] : 'no';     
        $radio = isset($svxconfig['File_Description']['RADIO']) ? $svxconfig['File_Description']['RADIO'] : 'no';     
    }

    $DMRStatusFile = DL3EL . "/dmr_status";
    $dmrstatus = trim(shell_exec('cat ' . $DMRStatusFile));
    $DVSModeFile = DL3EL . "/dvs_mode";
    $dvsmode = trim(shell_exec('cat ' . $DVSModeFile));
    $color = "blue";
    $colorf = "blue";
    $colory = "blue";
    $colord = "blue";
    $colordv = "blue";
    if ($dmrstatus == "DMR_FM") {
        $mode ="DMR_FM";
        $color = "blue";
        $colorf = "green";
    }
    if ($dmrstatus == "DVS_only") {
        $mode ="DVS_only";
        $colordv = "green";
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

    if ($dmrstatus == "FM_only") {
        $mode ="FM_only";
        $color = "blue";
        $dmrtg = "off";
    }

    if (isset($_POST['btn_DVS_ein'])) {
// aktuelle svxlink.conf sollte Kopie von svxlink.conf.dmr_fm sein
            $mode ="DMR_FM";
            $color = "blue";
            $colordv = "green";
            $kanal = "OV F49";
            $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DMR 2>&1";
            exec($command,$screen,$retval);
            $command = "echo '*71#' > /tmp/dtmf_svx";
            exec($command,$screen,$retval);
            $dmrtg = $DMRdefTG;
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            file_put_contents($DMRStatusFile, "DMR_FM");
            file_put_contents($DMRTGFile, $dmrtg);
            file_put_contents($DVSModeFile, "DMR");
            $dvsmode = "DMR";
    }

    if (isset($_POST['btn_DMR_91']))
    {
        if (strncmp($mode, "DMR", 3) === 0) {
            $colordv = "green";
            $kanal = "WW";
            $dmrtg = "91";
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            $DMRTGFile = DL3EL . "/dmr_tg";
            file_put_contents($DMRTGFile, $dmrtg);
            file_put_contents($DVSModeFile, "DMR");
            $dvsmode = "DMR";
        }
    }

    if (isset($_POST['btn_DMR_262649']))
    {
        if (strncmp($mode, "DMR", 3) === 0) {
            $colordv = "green";
            $kanal = "OV F49";
            $dmrtg = "262649";
            $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
            exec($command,$screen,$retval);
            $DMRTGFile = DL3EL . "/dmr_tg";
            file_put_contents($DMRTGFile, $dmrtg);
            file_put_contents($DVSModeFile, "DMR");
            $dvsmode = "DMR";
        }
    }
    if (isset($_POST['btn_FM_aus'])) {
        $command = "echo '*9#' > /tmp/dtmf_svx";
        exec($command,$screen,$retval);
        $mode ="DVS_only";
        $color = "red";
        $colorf = "blue";
        file_put_contents($DMRStatusFile, "DVS_only");
    }

    if (isset($_POST['btn_FM_ein'])) {
        $mode ="DMR_FM";
        $color = "blue";
        $colorf = "red";
        $command = "echo '*910#' > /tmp/dtmf_svx";
        exec($command,$screen,$retval);
        file_put_contents($DMRStatusFile, "DMR_FM");
    }


    if (isset($_POST['btn_DVS_aus'])) {
        $command = "echo '*7#' > /tmp/dtmf_svx";
        exec($command,$screen,$retval);
        $command = "echo '*910#' > /tmp/dtmf_svx";
        exec($command,$screen,$retval);
        $mode ="FM_only";
        $color = "blue";
        $colordv = "blue";
        $kanal = "disconnect";
        file_put_contents($DMRStatusFile, "FM_only");
        file_put_contents($DVSModeFile, "OFF");
        $dvsmode = "OFF";
        if ($dmr_support == "1") {
            $dmrtg = "off";
        } else {
            $dmrtg = "no DMR";
        }    
    }
    

// YSF
    if (isset($_POST['btn_YSF']))
    {
        $colory = "green";
        $kanal = " ";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode YSF 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "YSF");
        $dvsmode = "YSF";
    }

    if (isset($_POST['btn_YSF_HES']))
    {
        $kanal = "Hessen";
        $colory = "green";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune c4fm.dehessen.de:42000 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "YSF");
        $dvsmode = "YSF";
    }

    if (isset($_POST['btn_YSF_26269']))
    {
        $dvsmode ="YSF";
        $colory = "green";
        $kanal = "Hessen";
//        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune dc9vq.ysf-deutschland.de/ysf2:42001 2>&1";
//        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune 85.215.138.68:42376 2>&1";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune ysf-deutschland.de:42376 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "YSF");
        $dvsmode = "YSF";
    }

    if (isset($_POST['btn_YSF_DISC']))
    {
        $color = "green";
        $colory = "blue";
        $kanal = "disconnect";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune disconnect 2>&1";
        exec($command,$screen,$retval);
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DMR 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "DMR");
        $dvsmode = "DMR";
    }

// DSTAR
    if (isset($_POST['btn_DSTAR']))
    {
        $colord = "green";
        $dvsmode ="DSTAR";
        $kanal = " ";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DSTAR 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "DSTAR");
        $dvsmode = "DSTAR";
    }


    if (isset($_POST['btn_DSTAR_HES']))
    {
        $dvsmode ="DSTAR";
        $colord = "green";
        $kanal = "Hessen";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune DCS001KL 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "DSTAR");
        $dvsmode = "DSTAR";
    }
    if (isset($_POST['btn_DSTAR_DISC']))
    {
        $color = "green";
        $colord = "blue";
        $dvsmode ="DSTAR";
        $kanal = "disconnect";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune disconnect 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "DSTAR");
        $dvsmode = "DSTAR";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DMR 2>&1";
        exec($command,$screen,$retval);
        file_put_contents($DVSModeFile, "DMR");
        $dvsmode = "DMR";
//        echo '<pre>D* 1KL'; print_r($screen); echo '</pre>';
    }

?>
    

    <table>
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>">
<!---
        <button name="btn_DMR" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; width:100px; font-size:12px;"><b>DMR ein</b></button>
--->
        <?php 
            if ($mode !== "FM_only") {
                if ((file_exists(DL3EL.'/Reflector4.conf')) && ($dvsmode !== "OFF" )) {
                echo "<br>";
                }    
                echo '<button  style = "border-radius:8px; color:white;border-color:transparent; background-color:orange; height:30px; font-size:12px;"><b>';
                echo $dmrtg . '/';
                echo $mode;
                echo '</b></button>';
            }    
        ?>
        &nbsp;&nbsp;&nbsp;
<!---
        <button name="btn_DVS_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:<?php echo $colordv;?>; height:30px; font-size:12px;"><b>DMR ein</b></button>
--->
        <?php
                if ($dvsmode === "OFF") {
                        $colordv = "blue";
                    echo '<button name="btn_DVS_ein" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $colordv .'; height:30px; font-size:12px;"><b>DVS (DMR) ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                } else {
                    if ($mode == "DVS_only") {
                        echo '<button name="btn_FM_ein" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $colorf .'; height:30px; font-size:12px;"><b>FM ein</b></button>';
                        echo "&nbsp;&nbsp;&nbsp;";
                    }
                    if ($mode == "DMR_FM") {
                        $colorf = "red";
                        echo '<button name="btn_FM_aus" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $colorf .'; height:30px; font-size:12px;"><b>FM aus</b></button>';
                        echo "&nbsp;&nbsp;&nbsp;";
                        $colorf = "blue";
                    }    
                    if ($dvsmode === "YSF") {
                        $colory = "green";
                        echo '<button name="btn_YSF_DISC" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:' . $colory .'; height:30px; font-size:12px;"><b>YSF aus</b></button>';
                    } else {
                        $colory = "blue";
                        echo '<button name="btn_YSF" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:' . $colory .'; height:30px; font-size:12px;"><b>YSF</b></button>';
                    }   
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    if ($dvsmode === "DSTAR") {
                        $colord = "green";
                        echo '<button name="btn_DSTAR_DISC" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:' . $colord .'; height:30px; font-size:12px;"><b>DSTAR</b></button>';
                    } else {
                        $colord = "blue";
                        echo '<button name="btn_DSTAR" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:' . $colord .'; height:30px; font-size:12px;"><b>DSTAR</b></button>';
                    }        
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                        $colordv = "green";
                    echo '<button name="btn_DVS_aus" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $colordv .'; height:30px; font-size:12px;"><b>DVS Bridge aus</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                if ((($mode == "DVS_only") || ($mode == "DMR_FM")) && ($dvsmode == "DMR")) {
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
                if ($dvsmode === "YSF") {
                        include_once "include/dvs_functions.php";
                        getYSFHosts();
                    echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]). '">';
                    echo '<button name="btn_YSF_HES" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF Hessen</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    echo '	<button name="btn_YSF_26269" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF 26269</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    echo '	<button name="btn_YSF_DISC" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF Discon</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                }    
//                if (($mode == "DVS_only")  && ($dvsmode == "DSTAR")) {
                if ($dvsmode == "DSTAR") {
                    echo '	<button name="btn_DSTAR_HES" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>D* Hessen</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    echo '	<button name="btn_DSTAR_DISC" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>DSTAR Discon</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                }
            }
                echo "<br>Configfile:$file($radio) / mode:$mode / DVS Mode:$dvsmode";
                echo '</form>';
    ?>
    </table>    
    <?php

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
