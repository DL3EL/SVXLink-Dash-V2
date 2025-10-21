<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

include "include/settings.php";
include_once "include/page_top.php";
?>

<?php
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo '<td width="200px" valign="top" class="hide" style = "height:auto;border:0;background-color:#f1f1f1;">';
    echo '<div class="nav" style = "margin-bottom:1px;margin-top:1px;">'."\n";

    echo '</div>'."\n";
    echo '</td>'."\n";

   if (defined('DL3EL')) {
      $ELQueryFile = DL3EL . "/el_query";
      $elquery = shell_exec('cat ' . $ELQueryFile);
   }   
?>
<!--- 
    <form method="post" action=""> 
-->
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" onsubmit="reloadPage()">
        <label for="prefix">Prefix abfragen (DL für Deutschland)</label>
        <input type="text" id="prefix" name="prefix" value=<?php echo $elquery;?> required>
        <button type="submit">Query</button>
        <input type="hidden" name="form_submitted" value="1">
    </form>
    &nbsp;&nbsp;&nbsp;&nbsp;
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" onsubmit="reloadPage()">
        <button type=submit id=jmptoE name=jmptoE class=active_id value=9999>*Echotest*</button>
    </form>
    <?php
    if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['form_submitted'])) {
        $cmd = "perl " . DL3EL . "/get-echolink.pl v=0 r=0 p=" . $_POST['prefix'];
        echo "",exec($cmd, $output, $retval);
        if (defined('DL3EL')) {
            $elquery = $_POST['prefix'] . " >" . $ELQueryFile;
            shell_exec("echo $elquery");
        }    
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
