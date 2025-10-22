<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

// last 1000 Spots
$url="https://dxc.jo30.de/dxcache/spots";
    $result  = file_get_contents($url);
    $spotter = json_decode($result, true);
    
    $dx_max = 0;
    foreach ($spotter as $dx_array) {
	if ((defined ('debug')) && (debug > 0)) echo "($nn): " . $spotter[$nn]['spotted'] . "<br>";
    ++$dx_max;
    }
    --$dx_max;
    $nn = 30;
//    $dx_max = 999;
    $dx_min = $dx_max - $nn;
    $dx = $dx_max;
    echo '<table style = "margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo "<tr><td><b>DX de</b></td><td><b>Freq</b></td><td><b>DX</b></td><td><b>Comments</b></td><td><b>UTC</b></td></tr>";
/* Array Felder
$spotter[$dx]['spotter']
$spotter[$dx]['message']
$spotter[$dx]['when']
$spotter[$dx]['frequency']
$spotter[$dx]['spotted']
*/

    while ($dx > $dx_min) { 
	if (strlen($spotter[$dx]['spotter'])) {
	    $msg = (array_key_exists('message',$spotter[$dx]))? $spotter[$dx]['message'] : " ";
	    $spt = (array_key_exists('spotter',$spotter[$dx]))? $spotter[$dx]['spotter'] : "n/a";
	    $dxc = (array_key_exists('spotted',$spotter[$dx]))? $spotter[$dx]['spotted'] : "n/a";
	    $tme = (array_key_exists('when',$spotter[$dx]))? $spotter[$dx]['when'] : "n/a";
	    $qrg = (array_key_exists('frequency',$spotter[$dx]))? $spotter[$dx]['frequency'] : "n/a";
	    $ll = 40;
	    $llm = strlen($msg);
	    if (strlen($msg) > $ll) {
		$msg_array = explode(" ",$msg);
		$msgn = "";
		foreach ($msg_array as $msg_word) {
		    $msgn = $msgn . $msg_word . " ";;
		    if (strlen($msgn) > $ll) {
			$msgn = $msgn . "<br>";
			$ll = $ll + 40;
		    }
		}
		$msg = $msgn;
		if ((defined ('debug')) && (debug > 0)) $msg = $msg . "<br>[" . $msgn . "]," . $llm;
	    } 
	           
//	    $time = substr($tme,0,strlen($tme)-8);
	    $time = substr($tme,11,8);
	    echo "<tr><td>" . $spt . "</td><td>" . $qrg . "</td><td>" . $dxc . "</td><td>" . $msg . "</td><td>" .  $time . "</td></tr>";
	 }   
	--$dx;
    }
	echo "</td></Tr>";
    echo '</table>';
    echo '<br><br>Danke an Jörg, DJ7NT, für die Bereitstellung der Daten über <a href="https://jo30.de/" target="jo30.de" style="color:blue;">jo30.de</a>';
?>
</center>
</div><br></div></fieldset>
