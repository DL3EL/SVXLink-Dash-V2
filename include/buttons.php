
<div class="content">
<?php
/*
if ($_SESSION['auth'] !== "AUTHORISED") {
	    return;
	}
*/
 if(array_key_exists('button1', $_POST)) {
        $exec= "echo '" . KEY1[1] . "' > /tmp/dtmf_svx";
        exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button2', $_POST)) {
        $exec= "echo '" . KEY2[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button3', $_POST)) {
        $exec= "echo '" . KEY3[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button4', $_POST)) {
        $exec= "echo '" . KEY4[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button5', $_POST)) {
        $exec= "echo '" . KEY5[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button6', $_POST)) {
        $exec= "echo '" . KEY6[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button7', $_POST)) {
        $exec= "echo '" . KEY7[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }

 if(array_key_exists('button8', $_POST)) {
        $exec= "echo '" . KEY8[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }

 if(array_key_exists('button9', $_POST)) {
        $exec= "echo '" . KEY9[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button10', $_POST)) {
        $exec= "echo '" . KEY10[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
if(array_key_exists('button11', $_POST)) {
        $exec= "echo '" . KEY11[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button12', $_POST)) {
        $exec= "echo '" . KEY12[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button13', $_POST)) {
        $exec= "echo '" . KEY13[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button14', $_POST)) {
        $exec= "echo '" . KEY14[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button15', $_POST)) {
        $exec= "echo '" . KEY15[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button16', $_POST)) {
        $exec= "echo '" . KEY16[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button17', $_POST)) {
        $exec= "echo '" . KEY17[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button18', $_POST)) {
        $exec= "echo '" . KEY18[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button19', $_POST)) {
        $exec= "echo '" . KEY19[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
 if(array_key_exists('button20', $_POST)) {
        $exec= "echo '" . KEY20[1] . "' > /tmp/dtmf_svx";
            exec($exec,$output);
            echo "<meta http-equiv='refresh' content='0'>";
        }
    if (strlen($exec)) {
        $logtext = "DTMF via Button: "  . $exec;
    } else {
        $logtext = "";
    }    

/*
// if(array_key_exists('button8', $_POST)) {
//        $exec="".KEY8[1]."";
//            exec($exec,$output);
//            echo "<meta http-equiv='refresh' content='0'>";
//        }

//if (SHOWPTT=="TRUE") {

// if(array_key_exists('button9', $_POST)) {
//        $exec="".KEY9[1]."";
//            exec($exec,$output);
//           echo "<meta http-equiv='refresh' content='0'>";
//        }
// if(array_key_exists('button10', $_POST)) {
//        $exec="".KEY10[1]."";
//            exec($exec,$output);
//            echo "<meta http-equiv='refresh' content='0'>";
//        }
// }
//
//
*/
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
 /*   
    if ((isset(KEY1[3])) && (KEY1[3] == "auth") && $auth) {
        echo "auth ok, display Button:" . KEY1[1];
    } else {
        echo "auth not ok (" . KEY1[3] . ", $auth), do not display Button:" . KEY1[1];
    }
*/    
    $button = (defined('KEY1'))? KEY1 : "";
    display_button($button,$auth,"button1");
    $button = (defined('KEY21'))? KEY21 : "";
    display_button($button,$auth,"button2");
    $button = (defined('KEY2'))? KEY2 : ""; display_button($button,$auth,"button2");
    $button = (defined('KEY3'))? KEY3 : ""; display_button($button,$auth,"button3");
    $button = (defined('KEY4'))? KEY4 : ""; display_button($button,$auth,"button4");
    $button = (defined('KEY5'))? KEY5 : ""; display_button($button,$auth,"button5");
    $button = (defined('KEY6'))? KEY6 : ""; display_button($button,$auth,"button6");
    $button = (defined('KEY7'))? KEY7 : ""; display_button($button,$auth,"button7");
    $button = (defined('KEY8'))? KEY8 : ""; display_button($button,$auth,"button8");
    $button = (defined('KEY9'))? KEY9 : ""; display_button($button,$auth,"button9");
    $button = (defined('KEY10'))? KEY10 : ""; display_button($button,$auth,"button10");
    $button = (defined('KEY11'))? KEY11 : ""; display_button($button,$auth,"button11");
    $button = (defined('KEY12'))? KEY12 : ""; display_button($button,$auth,"button12");
    $button = (defined('KEY13'))? KEY13 : ""; display_button($button,$auth,"button13");
    $button = (defined('KEY14'))? KEY14 : ""; display_button($button,$auth,"button14");
    $button = (defined('KEY15'))? KEY15 : ""; display_button($button,$auth,"button15");
    $button = (defined('KEY16'))? KEY16 : ""; display_button($button,$auth,"button16");
    $button = (defined('KEY17'))? KEY17 : ""; display_button($button,$auth,"button17");
    $button = (defined('KEY18'))? KEY18 : ""; display_button($button,$auth,"button18");
    $button = (defined('KEY19'))? KEY19 : ""; display_button($button,$auth,"button19");
    $button = (defined('KEY20'))? KEY20 : ""; display_button($button,$auth,"button20");
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
        echo '<label style = "text-shadow: 1px 1px 1px Lightgrey, 0 0 0.5em LightGrey, 0 0 1em whitesmoke;font-weight:bold;color:#464646;" for="dtmfsvx">DTMF command (must end with #):</label>  ';
        echo '<input type="text" id="dtmfsvx" name="dtmfsvx">';
        echo '<input type="submit" value="Send DTMF code" class="green"><br>';
        echo '</center>';
        echo '</form>';
    }    
  if (isset($_POST["dtmfsvx"])){
    $exec= "echo '" . $_POST['dtmfsvx'] . "' > /tmp/dtmf_svx";
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext = "DTMF via Dashboard: "  . $_POST['dtmfsvx'];
    }
  if (isset($_POST["jmpto"])) {
    $exec= "echo '91" . $_POST['jmpto'] . "#' > /tmp/dtmf_svx";
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext = "DTMF via Dashboard: "  . $_POST['jmpto'];
    }
 if (isset($_POST["jmptoA"])) {
    $exec= "echo '*91" . $_POST['jmptoA'] . "#' > /tmp/dtmf_svx";
    exec($exec,$output);
    echo "<meta http-equiv='refresh' content='0'>";
    $logtext = "DTMF via Dashboard: "  . $_POST['jmptoA'];
    }
if (isset($_POST["jmptoM"])) {
    $exec= "echo '94" . $_POST['jmptoM'] . "#' > /tmp/dtmf_svx";
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
//                if ((defined ('debug')) && (debug)) echo "sub auth ok, display Button:" . $button[1] ."<br>";
                $display = 1;
            } else {
                $display = 0;
//                if ((defined ('debug')) && (debug)) echo "sub auth not ok (" . $button[3] . ", $auth), do not display Button:" . $button[1] ."<br>";
            }
        } else {
//            if ((defined ('debug')) && (debug)) echo "sub auth not necessary(!), display Button: $button[1] <br>";
            $display = 1;
        }    
        if ($display) {
            echo '<input type="submit" name="' . $button_name . '" ';
            echo 'class=' . $button[2] . ' value="' . $button[0] . '"/>';
        }    
    }
}
?>
