<?php
    $last_pos = 0;
    if (defined('DL3EL')) {
        $RelaisFile = DL3EL . "/relais.csv";
        $FMQueryFile = DL3EL . "/fm_query";
        $fmquery = file_get_contents($FMQueryFile);
        $FMLQueryFile = DL3EL . "/fml_query";
        $fmlquery = file_get_contents($FMLQueryFile);
        $last_pos = 1;
        $relais_act = DL3EL . "/relais.act";
    } else {
        return;
    }
    if ((defined ('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
        $aprspos = DL3EL . "/aprs-follow.pos";
        if (file_exists($aprspos)) {
            $delta = time() - filemtime($aprspos);
// 43.200 = 12h
            if ($delta < 43200) {
//            if ($delta < 30) {
                $filepos = file_get_contents($aprspos);
                $position = explode("^",$filepos);
                if ((defined ('debug')) && (debug > 0)) echo "<br>" . $position[0] . " " . $position[1] . ":" .  $position[2] . ":" .  $position[3];
                if (isset($position[3])) {
                    $fmquery = $position[3];
                    $fmlquery = "";
                    $last_pos = 2;
                    $update_list = 1;
                } else {   
                    $last_pos = 1;
                }    
                $update_list = 0;
            } else {
                $last_pos = 1;
                $update_list = 0;
            }    
        }
    } else {
        $update_list = 0;
    }     

    if ($last_pos === 2) {
//        $cmd = "wget -O " . $RelaisFile . " -q \"http://relais.dl3el.de/cgi-bin/relais.pl?sel=gridsq&gs=" . $fmlquery . "&type_el=1&type_fr=1&printas=csv&maxgateways=20&nohtml=yes&quelle=y\"";
        $cmd = "wget -O " . $RelaisFile . " -q \"http://relais.dl3el.de/cgi-bin/relais.pl?sel=ctrcall&ctrcall=" . $position[3] . "&type_el=1&type_fr=1&printas=csv&maxgateways=35&nohtml=yes&quelle=y\"";
        echo "",exec($cmd, $output, $retval);
        $update_list = 1;
        touch($relais_act);
    }    

    $loc = "";
    $loc_found = 0;
    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['form_submitted'])) {
        if (isset($_POST['type_el'])) { $query_el = "&type_el=" . $_POST['type_el'];}
        if (isset($_POST['type_fr'])) { $query_fr = "&type_fr=" . $_POST['type_fr'];}
        if (isset($_POST['type_fhs'])) { $query_fhs = "&type_fhs=" . $_POST['type_fhs'];}
        if (isset($_POST['prefix'])) { $query_loc = "sel=ctrcall&ctrcall=" . $_POST['prefix'] ;}
        if (isset($_POST['locator']) && ($_POST['locator'] != "")) { 
            $query_loc = "sel=gridsq&gs=" . $_POST['locator'] ;
            $loc = $_POST['locator'];
            $loc_found = 1;
        }
   
        $cmd = "wget -O " . $RelaisFile . " -q \"http://relais.dl3el.de/cgi-bin/relais.pl?" . $query_loc . $query_el . $query_fr . $query_fhs . "&printas=csv&maxgateways=35&nohtml=yes&quelle=y\"";
        if ((defined ('debug')) && (debug > 0)) echo "Call: $cmd<br>";
        echo "",exec($cmd, $output, $retval);
        $update_list = 1;
        touch($relais_act);

        if (defined('DL3EL')) {
            $fmquery = $_POST['prefix'];
            file_put_contents($FMQueryFile, $fmquery);
            $fmlquery = $_POST['locator'];
            file_put_contents($FMLQueryFile, $fmlquery);
        }    
    }
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    if (($handle = fopen($RelaisFile, "r")) !== FALSE) {
        while (($data = fgetcsv($handle, 1000, ";", "\"", "\\")) !== FALSE) {
//        echo "0: " . $data[0] . "/ 1:" . $data[1] . "/ 2:"  . $data[2] . "/ 3:"  . $data[3] . "/ 4:"  . $data[4] . "/ 5:" . $data[5] . "/ 6:" . $data[6] . "/ 7:"  . $data[7] . "/ 8:"  . $data[8] . "/ 9:"  . $data[9] . "/ 10:" . $data[10] . "/ 11:"  . $data[11] . "/ 12:"  . $data[12] . "<br>";
            if (!$loc_found && ($data[3] !== "Locator")) {
                $loc = $data[3];
                $loc_found = 1;
            }    
            if ((strncmp($data[0], "Daten m", 7) !== 0) && (strncmp($data[0], "Call", 4) !== 0)) {
                if (strncmp($data[12], "FR", 2) === 0) {
                    $bold_b = "<b>";
                    $bold_e = "</b>";
                } else {
                    $bold_b = "";
                    $bold_e = "";
                }
                if (strncmp($data[12], "E", 1) === 0) {
//          printf "<button type=submit id=jmptoE name=jmptoE class=active_id value=%s>%s:%s</button>",$CurrentLoginsTab{$call}{'ELNODE'},$CurrentLoginsTab{$call}{'ELCALL'},$CurrentLoginsTab{$call}{'ELNODE'};
                    $echolink_conn = "<button type=submit id=jmptoE name=jmptoE class=active_id value=" . $data[13] . ">EL#" . $data[13] . "</button>";
                } elseif (strncmp($data[12], "F", 1) === 0) {
                    $echolink_conn = "<button type=submit id=jmptoE name=jmptoA class=active_id value=" . $data[14] . ">TG " . $data[14] . "</button>";
                } else {
                    $echolink_conn = "";
                }    
                echo "<tr><td>" . $bold_b . $data[0] . $bold_e . "</td><td>" . $data[1] . "</td><td>"  . $data[2] . "</td><td>"  . $data[3] . "</td><td>" . $bold_b . substr($data[4], 0, 50) . $bold_e . "</td><td>"  . $data[9] . "</td><td>"  . $echolink_conn . "</td><td>"  . $data[11] . "</td></tr>";
            }
        }
        fclose($handle);
        $relais_act = DL3EL . "/relais.act";
        if (file_exists($relais_act)) {
            $cmd = "wget -O- -q \"http://relais.dl3el.de/cgi-bin/metar.pl?sel=gridsq&gs=" . $loc . "\"";
            echo "",exec($cmd, $output, $retval);
            unlink($relais_act);
        }   
        echo '</form>';
    } else {
      echo "wrong file: " . $RelaisFile ."<br>";  
    }
?>
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]) . "?id=relais0"; ?>">
        <label for="prefix">Call</label>
        <input type="text" style = "width:100px" id="prefix" name="prefix" value=<?php echo $fmquery;?>>  
        <label for="locator"> oder Locator</label>
        <input type="text" id="locator" name="locator" value=<?php echo $fmlquery;?>>  
        <input type="checkbox" name="type_el" checked value="1">Echolink&nbsp;&nbsp;
        <input type="checkbox" name="type_fr" checked value="1">FM Funknetz&nbsp;&nbsp;
        <input type="checkbox" name="type_fhs" value="1">FM Funknetz Hotspots&nbsp;&nbsp;
        <button type="submit">Query</button>
        <input type="hidden" name="form_submitted" value="1">
    </form>
</table>

<?php

if (isset($_POST["jmptoE"])) {
    $exec= "echo '2#" . $_POST['jmptoE'] . "#' > /tmp/dtmf_svx";
    exec($exec,$output);
}

?>
