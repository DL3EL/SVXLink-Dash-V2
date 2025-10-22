<?php
if (MENUBUTTON=="BOTTOM") {
include_once "include/buttons.php"; }
?>
<?php

    echo '<div class="content2">'."\n";
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
