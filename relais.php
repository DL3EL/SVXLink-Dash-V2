<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/page_top.php";
?>
<?php
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
//    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
//    echo '<td valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:1px;">'."\n";

    echo '</div>'."\n";
    echo '</td>'."\n";

    if (defined('DL3EL')) {
        $ELQueryFile = DL3EL . "/el_query";
        $elquery = shell_exec('cat ' . $ELQueryFile);
        $RelaisFile = DL3EL . "/relais.csv";
        $FMQueryFile = DL3EL . "/fm_query";
        $fmquery = shell_exec('cat ' . $FMQueryFile);
        $FMLQueryFile = DL3EL . "/fml_query";
        $fmlquery = shell_exec('cat ' . $FMLQueryFile);
    } else {
        $RelaisFile = "relais.csv";
    }
    if ((defined ('DL3EL_APRS_MSG')) && (DL3EL_APRS_MSG === "yes")) {
        $aprspos = DL3EL . "/aprs-follow.pos";
        if (file_exists($aprspos)) {
            $filepos = file_get_contents($aprspos);
            $position = explode("^",$filepos);
            if ((defined ('debug')) && (debug > 0)) echo "<br>" . $position[0] . " " . $position[1] . ":" .  $position[2];
            $fmquery = "";
            $fmlquery = $position[2];
            $update_list = 1;
        }
    } else {
        $update_list = 0;
    }     
?>
   <p style = "padding-left: 5px; text-align: left;"> &nbsp;
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" onsubmit="reloadPage()">
        <label for="prefix">Call</label>
        <input type="text" id="prefix" name="prefix" value=<?php echo $fmquery;?>>  
        <label for="locator"> oder Locator</label>
        <input type="text" id="locator" name="locator" value=<?php echo $fmlquery;?>>  
        <input type="checkbox" name="type_el" checked value="1">Echolink&nbsp;&nbsp;
        <input type="checkbox" name="type_fr" checked value="1">FM Funknetz&nbsp;&nbsp;
        <input type="checkbox" name="type_fhs" value="1">FM Funknetz Hotspots&nbsp;&nbsp;
        <button type="submit">Query</button>
        <input type="hidden" name="form_submitted" value="1">
    </form>


   </p> 
    <?php
    if ($update_list === 1) {
        $cmd = "wget -O " . $RelaisFile . " -q \"http://relais.dl3el.de/cgi-bin/relais.pl?sel=gridsq&gs=" . $fmlquery . "&type_el=1&type_fr=1&printas=csv&maxgateways=20&nohtml=yes&quelle=y\"";
        echo "",exec($cmd, $output, $retval);
    }    
    $loc = "";
    $loc_found = 0;
    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['form_submitted'])) {
//    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST['type_el'])) { $query_el = "&type_el=" . $_POST['type_el'];}
        if (isset($_POST['type_fr'])) { $query_fr = "&type_fr=" . $_POST['type_fr'];}
        if (isset($_POST['type_fhs'])) { $query_fhs = "&type_fhs=" . $_POST['type_fhs'];}
        if (isset($_POST['prefix'])) { $query_loc = "sel=ctrcall&ctrcall=" . $_POST['prefix'] ;}
        if (isset($_POST['locator']) && ($_POST['locator'] != "")) { 
            $query_loc = "sel=gridsq&gs=" . $_POST['locator'] ;
            $loc = $_POST['locator'];
            $loc_found = 1;
        }
    
//        echo "EL: " . $query_el . "&nbsp; FMR" . $query_fr . "<br>";
//        $cmd = "wget -O " . $RelaisFile . " -q \"http://relais.dl3el.de/cgi-bin/relais.pl?sel=ctrcall&ctrcall=" . $_POST['prefix'] . $query_el . $query_fr . $query_fhs . "&printas=csv&maxgateways=20&nohtml=yes&quelle=y\"";
        $cmd = "wget -O " . $RelaisFile . " -q \"http://relais.dl3el.de/cgi-bin/relais.pl?" . $query_loc . $query_el . $query_fr . $query_fhs . "&printas=csv&maxgateways=20&nohtml=yes&quelle=y\"";
//        $cmd = "wget -O " . $RelaisFile . " -q \"https://relais.dl3el.de/FM-Relais/DM7DS/relaisgeo_fmn_el.php\"";
//        echo "<br>Aufruf: " . $cmd . "<br>";
        if ((defined ('debug')) && (debug > 0)) echo "Call: $cmd<br>";
        echo "",exec($cmd, $output, $retval);
        $update_list = 1;
        if (defined('DL3EL')) {
            $fmquery = $_POST['prefix'] . " >" . $FMQueryFile;
            shell_exec("echo $fmquery");
            $fmlquery = $_POST['locator'] . " >" . $FMLQueryFile;
            shell_exec("echo $fmlquery");
        }    
    }

    if (($handle = fopen($RelaisFile, "r")) !== FALSE) {
        echo '<form method="post">';
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
//                substr($data[4], 0, 20);
//                echo "<tr><td>" . $bold_b . $data[0] . $bold_e . "</td><td>" . $data[1] . "</td><td>"  . $data[2] . "</td><td>"  . $data[3] . "</td><td>" . $bold_b . $data[4] . $bold_e . "</td><td>"  . $data[9] . "</td><td>"  . $echolink_conn . "</td><td>"  . $data[11] . "</td></tr>";
                echo "<tr><td>" . $bold_b . $data[0] . $bold_e . "</td><td>" . $data[1] . "</td><td>"  . $data[2] . "</td><td>"  . $data[3] . "</td><td>" . $bold_b . substr($data[4], 0, 50) . $bold_e . "</td><td>"  . $data[9] . "</td><td>"  . $echolink_conn . "</td><td>"  . $data[11] . "</td></tr>";
            }
        }
        fclose($handle);
        if ($update_list === 1) {
            $cmd = "wget -O- -q \"http://relais.dl3el.de/cgi-bin/metar.pl?sel=gridsq&gs=" . $loc . "\"";
            echo "",exec($cmd, $output, $retval);
        }    
        echo '</form>';
    } else {
      echo "wrong file: " . $RelaisFile ."<br>";  
    }
    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['form_submitted'])) {
//    if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Refresh the page to show updated values
        echo "<script type='text/javascript'>
//            reloadPage();
        </script>";    
    }    
  ?>
</table>

<?php

if (isset($_POST["jmptoE"])) {
    $exec= "echo '2#" . $_POST['jmptoE'] . "#' > /tmp/dtmf_svx";
    exec($exec,$output);
}

?>
<?php include_once "include/page_bottom.php"; ?>
