

    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>">
    <?php
        $file = SVXCONFPATH.SVXCONFIG;
        if (file_exists(DL3EL.'/Reflector1.conf')) {
            echo '<button name="btn_REF1" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; width:100px; font-size:12px;"><b>REF1</b></button>';
            echo '&nbsp;&nbsp;&nbsp;';
        }    
        if (file_exists(DL3EL.'/Reflector2.conf')) {
            echo '<button name="btn_REF2" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; width:100px; font-size:12px;"><b>REF2</b></button>';
            echo '&nbsp;&nbsp;&nbsp;';
        }    
        if (file_exists(DL3EL.'/Reflector3.conf')) {
            echo '<button name="btn_REF3" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; width:100px; font-size:12px;"><b>REF3</b></button>';
            echo '&nbsp;&nbsp;&nbsp;';
        }    
        if (file_exists(DL3EL.'/Reflector4.conf')) {
            echo '<button name="btn_REF4" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:blue; height:30px; width:100px; font-size:12px;"><b>REF4</b></button>';
            echo '&nbsp;&nbsp;&nbsp;';
        }    
    ?>
    </form> 


<?php

    if (isset($_POST['btn_REF1'])) {
        upd_svx_config($file,"dl3el/Reflector1.conf");
    }
    if (isset($_POST['btn_REF2'])) {
        upd_svx_config($file,"dl3el/Reflector2.conf");
    }
    if (isset($_POST['btn_REF3'])) {
        upd_svx_config($file,"dl3el/Reflector3.conf");
    }
    if (isset($_POST['btn_REF4'])) {
        upd_svx_config($file,"dl3el/Reflector4.conf");
    }

function svx_restart() {
    $command = "sudo systemctl restart svxlink 2>&1";
    exec($command,$screen,$retval);
    echo "restarting svxlink ...";
    sleep(1);
    if ($retval === 0) {
        echo "svxlink sucessfull restartet";
      } else {
        echo "svxlink restart failure, check log";
      }
}

function upd_svx_config($file,$file_new) {
    $backup_filename = $file . "." . date("YmdHis");
    exec('sudo cp -p ' . $file . ' ' . $backup_filename);

    $svxconfig = custom_parse_ini_file($file);
    $svxconfig_chg = custom_parse_ini_file($file_new);

    foreach ($svxconfig_chg as $section_chg => $entries) {
        if ((defined ('debug')) && (debug > 1)) echo "[$section]<br>";
        foreach ($entries as $key => $data) {
            if ((defined ('debug')) && (debug > 2)) echo "$key = " . $svxconfig_chg[$section_chg][$key]['value'] . "(" . $svxconfig_chg[$section_chg][$key]['active'] .")<br>";
        }
    }
    $content = "";
    $nn=0; 
// gültig:
//DNS_DOMAIN = fm-funknetz.de
//#HOSTS = 44.148.237.5:5308
//CALLSIGN = DL3EL
//AUTH_KEY = "Connect-FM-FunkNetz25"
//DEFAULT_TG = 262649
//MONITOR_TGS = 262649+++
    
    if ((defined ('debug')) && (debug > 1)) echo "<br>";
    foreach ($svxconfig as $section => $entries) {
        $content = $content . "[$section]\n";
        foreach ($entries as $key => $data) {
            if ($section === $section_chg) {
                if ((defined ('debug')) && (debug > 1)) echo "CONF0: $section/$key, V:" . $svxconfig[$section][$key]['value'] . " A(" . $svxconfig[$section][$key]['active'] . ")<br>";
                if ($svxconfig[$section][$key]['active']) {
                    if (strlen($svxconfig_chg[$section][$key]['value']) > 0) {
                        if (!$svxconfig_chg[$section][$key]['active']) {
                            $svxconfig[$section][$key]['active'] = FALSE;
                        } else {
                            $svxconfig[$section][$key]['value'] = $svxconfig_chg[$section][$key]['value'];
                        }    
                    }
                } else {    
                    if ($svxconfig_chg[$section][$key]['active']) {
                        $svxconfig[$section][$key]['value'] = $svxconfig_chg[$section][$key]['value'];
                        $svxconfig[$section][$key]['active'] = TRUE;
                    }
                }        
                if ((defined ('debug')) && (debug > 1)) echo "CONF1: $section/$key, V:" . $svxconfig[$section][$key]['value'] . " A(" . $svxconfig[$section][$key]['active'] . ")<br>";
            }
            if (!$svxconfig[$section][$key]['active']) {
                $content = $content . "#";
            }  
            $content = $content . "$key = " . $svxconfig[$section][$key]['value'] . "\n";
        ++$nn;
        }  
        $content = $content . "\n";
    }  

    file_put_contents($file, $content);
    svx_restart();
}


?>
