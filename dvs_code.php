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
        $file = isset($svxconfig['File_Description']['FUNCTION']) ? $svxconfig['File_Description']['FUNCTION'] : 'no';     
        $radio = isset($svxconfig['File_Description']['RADIO']) ? $svxconfig['File_Description']['RADIO'] : 'no';     
    }

    $DMRStatusFile = DL3EL . "/dmr_status";
    $dmrstatus = trim(shell_exec('cat ' . $DMRStatusFile));
    $DVSModeFile = DL3EL . "/dvs_mode";
    $dvsmode = trim(shell_exec('cat ' . $DVSModeFile));
    $color = "blue";
    $colorb = "blue";
    if ($dmrstatus == "DMR_FM") {
        $mode ="DMR_FM";
        $color = "blue";
        $colorb = "green";
    }
    if ($dmrstatus == "DVS_only") {
        $mode ="DVS_only";
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

    if (isset($_POST['btn_DVS_only']))
    {
        if (($mode == "FM_only")  && ($file === "dmr_fm")) {
            $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_fm 2>&1";
            exec($command,$screen,$retval);
        }
        $mode ="DVS_only";
        $color = "red";
        $colorb = "blue";
        $kanal = "OV F49";
        $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_only /etc/svxlink/svxlink.conf 2>&1";
        exec($command,$screen,$retval);
        $file = "dmr_only";
        $command = "sudo service svxlink restart 2>&1";
        exec($command,$screen,$retval);
        $dmrtg = $DMRdefTG;
        $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $dmrtg . " 2>&1";
        exec($command,$screen,$retval);
        if (defined('DL3EL')) {
            $dmrstatus = "DVS_only >" . $DMRStatusFile;
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
    if (isset($_POST['btn_DVS_only_DISC']))
    {
        if ($mode == "DMR_FM") {
            $command = "echo '*7#' > /tmp/dtmf_svx";
            exec($command,$screen,$retval);
            $command = "echo '*91262649#' > /tmp/dtmf_svx";
            exec($command,$screen,$retval);
            if ($file !== "dmr_fm") {
                $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_fm /etc/svxlink/svxlink.conf 2>&1";
                exec($command,$screen,$retval);
                $file = "dmr_fm";
                $command = "sudo service svxlink restart 2>&1";
                exec($command,$screen,$retval);
            }
        } else {
            if ($dvsmode == "DMR") {
                $command = "/opt/MMDVM_Bridge/dvswitch.sh tune 4000 2>&1";
                exec($command,$screen,$retval);
            }    
            if ($file === "dmr_only") {
                $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_only 2>&1";
                exec($command,$screen,$retval);
                $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_fm /etc/svxlink/svxlink.conf 2>&1";
                exec($command,$screen,$retval);
                $command = "sudo service svxlink restart 2>&1";
                exec($command,$screen,$retval);
                $file = "dmr_fm";
            }
        }
        $mode ="FM_only";
        $color = "blue";
        $colorb = "blue";
        $kanal = "disconnect";
        if (defined('DL3EL')) {
            $dmrstatus = "FM_only >" . $DMRStatusFile;
            shell_exec("echo $dmrstatus");
            $dvsmode = "OFF >" . $DVSModeFile;
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
        if (($mode == "FM_only") && ($file === "dmr_fm")) {
            $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_fm 2>&1";
            exec($command,$screen,$retval);
        }
        $mode ="DVS_only";
        $color = "red";
        $colorb = "blue";
        $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_only /etc/svxlink/svxlink.conf 2>&1";
        exec($command,$screen,$retval);
        $file = "dmr_only";
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
           $dmrstatus = "DVS_only >" . $DMRStatusFile;
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
    if (isset($_POST['btn_DSTAR_only']))
    {
        if (($mode == "FM_only") && ($file === "dmr_fm")) {
            $command = "sudo cp -p /etc/svxlink/svxlink.conf /etc/svxlink/svxlink.conf.dmr_fm 2>&1";
            exec($command,$screen,$retval);
        }
        $mode ="DVS_only";
        $color = "red";
        $colorb = "blue";
        $command = "sudo cp -p /etc/svxlink/svxlink.conf.dmr_only /etc/svxlink/svxlink.conf 2>&1";
        exec($command,$screen,$retval);
        $file = "dmr_only";
        $command = "sudo service svxlink restart 2>&1";
        exec($command,$screen,$retval);
        $dvsmode ="DSTAR";
        $kanal = " ";
        $command = "/opt/MMDVM_Bridge/dvswitch.sh mode DSTAR 2>&1";
        exec($command,$screen,$retval);

        if (defined('DL3EL')) {
           $dvsmode = "DSTAR >" . $DVSModeFile;
           shell_exec("echo $dvsmode");
           $dvsmode = "DSTAR";
           $dmrstatus = "DVS_only >" . $DMRStatusFile;
           shell_exec("echo $dmrstatus");
        }    
    }

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
        <button name="btn_DVS_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:<?php echo $color;?>; height:30px; font-size:12px;"><b>DMR ein</b></button>
--->
        <?php
                if ($mode == "FM_only") {
                    echo '<button name="btn_DVS_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DMR ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_YSF_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>YSF ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_DSTAR_only" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DSTAR ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_DMR_FM" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color: grey; height:30px; font-size:12px;"><b>DVS Bridge ein</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }
                if ($mode == "DVS_only") {
                    echo '<button name="btn_DVS_only_DISC" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DVS aus</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }
                if ($mode == "DMR_FM") {
                    echo '<button name="btn_DMR_FM" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DMR</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                    echo '<button name="btn_YSF" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>YSF</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    echo '<button name="btn_DSTAR" type="submit" style="border-radius:8px; color:white; border-color:transparent; background-color:' . $color .'; height:30px; font-size:12px;"><b>DSTAR</b></button>';
                    echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
                    echo '<button name="btn_DVS_only_DISC" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color: red; height:30px; font-size:12px;"><b>DVS Bridge aus</b></button>';
                    echo "&nbsp;&nbsp;&nbsp;";
                }    
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
//                if (($mode == "DVS_only")  && ($dvsmode == "YSF")) {
                if ($dvsmode == "YSF") {
                        include_once "include/dvs_functions.php";
                        getYSFHosts();
                    echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]). '">';
                    echo '<br><br><button name="btn_YSF_HES" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:green; height:30px; width:100px; font-size:12px;"><b>YSF Hessen</b></button>';
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
                echo "<br>Config:$file($radio) / mode:$mode / DVS Mode:$dvsmode";
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
