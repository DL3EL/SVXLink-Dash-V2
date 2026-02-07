<?php
if (MENUBUTTON=="BOTTOM") {
include_once "include/buttons.php"; }
if (isset($_POST['btn_KILL_Player'])) {
    $command = "sudo killall -KILL node 2>&1";
    exec($command,$screen,$retval);
}
if (file_exists('/etc/systemd/system/svxlink-node.service')) {
    echo '<form method="post" action="' . htmlspecialchars($_SERVER["PHP_SELF"]) . '">';
    echo '<button name="btn_KILL_Player" type="submit" style = "border-radius:8px; color:white; border-color:transparent; background-color:red; height:30px; width:100px; font-size:12px;"><b>Restart Player (if necessary)</b></button>';
    echo '</form>';
} 


    echo '<div class="content2">'."\n";
echo ' <center>';
    echo '<script type="text/javascript">'."\n";
    echo 'function reloadSysInfo(){'."\n";
    echo '  $("#sysInfo").load("include/system.php",function(){ setTimeout(reloadSysInfo,15000) });'."\n";
    echo '}'."\n";
    echo 'setTimeout(reloadSysInfo,15000);'."\n";
    echo '$(window).trigger(\'resize\');'."\n";
    echo '</script>'."\n";
    echo '<div id="sysInfo">'."\n";
    include_once "include/system.php";
    echo '</div>'."\n";
    echo '</div>'."\n";
?>
<!--- Please do not remove copyright info -->
<center><span title="Dashboard" style = "font: 7pt arial, sans-serif;">SvxLink Dashboard Ver 2.5 ©  G4NAB, SP2ONG, SP0DZ, DL3EL <?php $cdate=date("Y"); if ($cdate > "2021") {$cdate="2021-".date("Y");} echo $cdate ;
 ?>
</div>
</fieldset>
<br>
</body>
</html>
