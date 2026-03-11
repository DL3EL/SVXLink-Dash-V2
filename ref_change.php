

    <form method="post"  style="display: inline;" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>">
    <?php
        $file = SVXCONFPATH.SVXCONFIG;
        $RefModeFile = DL3EL . "/ref_mode";
        $refmode = file_get_contents($RefModeFile);
        $color = "blue";

        for ($i = 1; $i <= 6; $i++) {
            $ref_conf = DL3EL.'/Reflector' . $i . '.conf';
            if (file_exists($ref_conf)) {
                $konstanten_name = "DL3EL_REF" . $i . "_BUTTON";
                if (defined($konstanten_name)) {
                    $buttontext = constant($konstanten_name);
                    $color = ($refmode === $buttontext)? "green" : "blue";
                } else {
                    $buttontext = "ref" . $i;
                }    
                echo '<button name="btn_REF' . $i . '" type="submit" style = "border-radius:8px; color:white;border-color:transparent; background-color:' . $color . '; height:30px; width:100px; font-size:12px;"><b>' . $buttontext . '</b></button>';
                echo '&nbsp;&nbsp;&nbsp;';
            }
        }

    ?>
    </form> 


<?php

    $RefModeFile = DL3EL . "/ref_mode";
// Wir prüfen alle Reflektoren von 1 bis 10
    for ($i = 1; $i <= 6; $i++) {
        $btn_name = "btn_REF" . $i;
    
        if (isset($_POST[$btn_name])) {
            // Hier kannst du jetzt die Aktion ausführen
            // z.B. ein Shell-Skript aufrufen, das den Reflektor umschaltet
            $ref_conf = DL3EL.'/Reflector' . $i . '.conf';
            upd_svx_config($file,$ref_conf);
            $ref_nummer = $i;
            $konstanten_name = "DL3EL_REF" . $ref_nummer . "_BUTTON";
            if (defined($konstanten_name)) {
                // Hier wird der Wert der Konstante indirekt geholt
                $button_wert = constant($konstanten_name);
                if ((defined ('debug')) && (debug > 0)) echo "Der Wert von $konstanten_name ist: " . $button_wert;
                file_put_contents($RefModeFile, $button_wert);
            }
            break; // Schleife abbrechen, da nur ein Button gleichzeitig gedrückt wird
        }
    }


?>
