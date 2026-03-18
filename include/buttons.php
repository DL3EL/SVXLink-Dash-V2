
<div class="content">
<center>

<?php
/*
if ($_SESSION['auth'] !== "AUTHORISED") {
	    return;
	}
*/
 $exec="";
 $dtmf_pty = get_pty();
 
for ($i = 1; $i <= 20; $i++) {
    $buttonName = 'button' . $i;
    $constantName = 'KEY' . $i;

    if (array_key_exists($buttonName, $_POST)) {
        // Wir nutzen constant(), um den Wert der KEY1, KEY2 etc. dynamisch abzugreifen
        $keyValue = constant($constantName)[1];
        
        $exec = "echo '" . $keyValue . "' > " . $dtmf_pty;
        exec($exec, $output);
        
        echo "<meta http-equiv='refresh' content='0'>";
        
        // Da nur ein Button gleichzeitig gedrückt wird, können wir hier abbrechen
        break; 
    }
}
/* 
 if(array_key_exists('button1', $_POST)) {
        $exec= "echo '" . KEY1[1] . "' > /tmp/dtmf_svx";
        exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
*/        
    if (strlen($exec)) {
        $logtext = "DTMF via Button: "  . $exec;
    } else {
        $logtext = "";
    }    

?> 



   
<fieldset style = "box-shadow:5px 5px 20px #999;background-color:#e8e8e8e8; width:855px;margin-top:5px;margin-bottom:14px;margin-left:6px;margin-right:0px;font-size:12px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
<div style = "padding:0px;width:100%;background-image: linear-gradient(to bottom, #e9e9e9 50%, #bcbaba 100%);border-radius: 10px;-moz-border-radius:10px;-webkit-border-radius:10px;border: 1px solid LightGrey;margin-left:0px; margin-right:0px;margin-top:4px;margin-bottom:0px;white-space:normal;">
<p style = "margin-bottom:0px;"></p>
<form method="post">
    <center>
<?php 
if ($_SESSION['auth'] !== "AUTHORISED") {
	    $auth = 0;
	} else {
        $auth = 1;
    }
//    $button = (defined('KEY20'))? KEY20 : ""; display_button($button,$auth,"button20");

    for ($i = 1; $i <= 20; $i++) {
        $keyName = "KEY" . $i;
        $button = (defined($keyName)) ? constant($keyName) : "";
        display_button($button, $auth, "button" . $i);
    }
//

//
?>        

 </center>
    </form>
<p style = "margin: 0 auto;"></p>

<?php
    if ((defined('DTMF')) && (DTMF === "auth")) {
        if ($auth) {
            $display = 1;
        } else {
            $display = 0;
        }
    } else {
        $display = 1;
    }
    if ($display) {
        echo '<form action="" method="POST" style = "margin-top:4px;">';
        echo '<center>';
        if (defined('DL3EL_DARK_MODE') && (DL3EL_DARK_MODE === "yes")) {
            echo '<label style = "font-size:18px; font-weight:bold; color:#464646;" for="dtmfsvx">DTMF command (must end with #):</label>  ';
        } else {
            echo '<label style = "text-shadow: 1px 1px 1px Lightgrey, 0 0 0.5em LightGrey, 0 0 1em whitesmoke;font-weight:bold;color:#464646;" for="dtmfsvx">DTMF command (must end with #):</label>  ';
        }        

        echo '<input type="text" id="dtmfsvx" name="dtmfsvx">';
        echo '<input type="submit" value="Send DTMF code" class="green"><br>';
        echo '</center>';
        echo '</form>';
    }    
  if (isset($_POST["dtmfsvx"])){
    $exec= "echo '" . $_POST['dtmfsvx'] . "' > " . $dtmf_pty;
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext = "DTMF via Dashboard: "  . $_POST['dtmfsvx'];
    }
  if (isset($_POST["jmpto"])) {
    $exec= "echo '91" . $_POST['jmpto'] . "#' > " . $dtmf_pty;
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext = "DTMF via Dashboard: "  . $_POST['jmpto'];
    }
 if (isset($_POST["jmptoA"])) {
    $exec= "echo '*91" . $_POST['jmptoA'] . "#' > " . $dtmf_pty;
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext = "DTMF via Dashboard: "  . $_POST['jmptoA'];
    }
if (isset($_POST["jmptoM"])) {
    $exec= "echo '94" . $_POST['jmptoM'] . "#' > " . $dtmf_pty;
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext =  "DTMF via Dashboard: "  . $_POST['jmptoM'];
    }
$logtext = "[" . $logtext . "]\n";
if ($logtext !== "[]\n") {
    $ip = isset($_SERVER['REMOTE_ADDR'])? $_SERVER['REMOTE_ADDR'] : '0';;
    $logtext = $ip . ": " . $logtext;
    addsvxlog($logtext);
}    

?>
<p style = "margin-bottom:-2px;"></p>
</div>
</fieldset>
<?php
function display_button($button,$auth,$button_name) {
    if (isset($button[1])) {
        if (isset($button[3])) {
            $key_auth = $button[3];
            if (($key_auth == "auth") && $auth) {
                if ((defined ('debug')) && (debug > 1)) echo "sub auth ok, display Button:" . $button[1] ."<br>";
                $display = 1;
            } else {
                $display = 0;
                if ((defined ('debug')) && (debug > 1)) echo "sub auth not ok (" . $button[3] . ", $auth), do not display Button:" . $button[1] ."<br>";
            }
        } else {
            if ((defined ('debug')) && (debug > 1)) echo "sub auth not necessary(!), display Button: $button[1] <br>";
            $display = 1;
        }    
        if ($display) {
            echo '<input type="submit" name="' . $button_name . '" ';
            echo 'class=' . $button[2] . ' value="' . $button[0] . '"/>';
        }    
    }
}
?>
