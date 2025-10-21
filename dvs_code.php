<?php
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
/*
	if (file_exists('/usr/bin/dvs')) {
       $dmr_support = "1";
       $dmrtg = "off";
    } else {
       $dmr_support = "0";
       $dmrtg = "no DMR";
    }   
*/

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
?>
