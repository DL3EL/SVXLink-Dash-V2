<?php
if (session_status() === PHP_SESSION_NONE) {
	session_start();
	include_once 'settings.php';
	include_once 'basedir.php';
	include_once 'config.php';
}

$call_display = "";
$call_svxsystem = "";
$call_expertedit = "";
$call_normaledit = "";
$call_refconf = "";
$call_logdat = "";
$openhamclock = "";
$kn_nor_col = "";
$kn_exp_col = "";
$normal = true;
$expert = false;
$menu_now = "classic";
$knowledge = "Normal";
$knowledgeFile = DL3EL . "/knowledge";



$datei = "dl3el/menu";
$menu_now = file_get_contents($datei);
if ($menu_now == "dropdown") {

	// Option kein Open Ham Clock Menüeintrag bei Display def. in config.php
	if (DL3EL_OPENHAMCLOCK == "no") {
		$ohc = false;
	} else {
		$ohc = true;
	}

	// Option kein Expert Edit Menü def. in config.php
	if (DL3EL_EXPERT == "no") {
		file_put_contents($knowledgeFile, $knowledge);
	}

	// Option Relais Dashboard ohne erweiterten Menü (nur Dashboard und DX)
	$public_relais = false;

	// Option Develop zusätzlicher Eintrag (Netzwerk Editor)
	if (DL3EL_VERSION == "develop") {
		$develop = true;
	} else {
		$develop = false;
	}

	// Option Anzeige Reflector Menü wenn Reflectoren wenn in config.php konfiguriert
	if ((!defined("DL3EL_REF1_BUTTON")) && (!defined("DL3EL_REF2_BUTTON")) && (!defined("DL3EL_REF3_BUTTON")) && (!defined("DL3EL_REF4_BUTTON"))) {
		$reflector = false;
	} else {
		$reflector = true;
	}

	// Option Anzeige Externe Links Menü wenn Links in config.php konfiguriert
	if ((!defined("DL3EL_EXTERN1_TAB")) && (!defined("DL3EL_EXTERN2_TAB")) && (!defined("DL3EL_EXTERN3_TAB")) && (!defined("DL3EL_EXTERN3_TAB"))) {
		$ext_link_menu = false;
	} else {
		$ext_link_menu = true;
	}

	// Option DX Cluster Menüeintrag wenn in config.php definiert
	if (DL3EL_DXCLUSTER == "yes") {
		$cluster = true;
	} else {
		$cluster = false;
	}

	// Open Ham Clock Menüeintrag DISPLAY wenn in config.php definiert
	if (DL3EL_OPENHAMCLOCK == "yes") {
		$clock = true;
	} else {
		$clock = false;
	}

	// Option OpenWebRx Menüeintrag DISPLAY wenn in config.php definiert
	if (defined('DL3EL_OPENWEBRX')) {
		$owrx = true;
	} else {
		$owrx = false;
	}

	// Extert & Normal Button Logic
	$knowledge = "Normal"; // Default

	// Button-Auswertung (zuerst behandeln!)
	if (isset($_POST['btn_expert'])) {
		$knowledge = "Expert";
		file_put_contents($knowledgeFile, $knowledge);
	} elseif (isset($_POST['btn_normal'])) {
		$knowledge = "Normal";
		file_put_contents($knowledgeFile, $knowledge);
	}
	// Falls kein Button gedrückt wurde → Datei lesen
	elseif (file_exists($knowledgeFile)) {
		$knowledge = file_get_contents($knowledgeFile);
	}

	// Status setzen
	$expert = ($knowledge === "Expert");
	$normal = !$expert;

	$kn_exp = $expert ? "<b>EXPERT</b>" : "EXPERT";
	$kn_nor = $normal ? "<b>NORMAL</b>" : "NORMAL";
	$kn_exp_col = $expert ? "green" : "";
	$kn_nor_col = $normal ? "green" : "";


	if (empty($_SESSION['auth'])) {
		$_SESSION['auth'] = "UNAUTHORISED";
	}

	echo '<br>';
	///////////////////// Drop Down Menu ///////////////////////////////////////////////

	//////////////////////// Funktions Definitionen ////////////////////////////////////
	//////////////////////// DISPLAY FKT Inline //////////////////////////////////////////
	if (isset($_POST['display'])) {
		$val = $_POST['display'];

		// URLs als Array definieren
		$urls = [
			1 => './index.php', //Dashboard wenn in DISPLAY Menü definiert
			2 => './caller.php?id=include/tg',
			3 => file_exists("/usr/local/bin/mqtt-simple")
				? './caller.php?id=monitor&refresh=15'
				: './caller.php?id=monitor0',
			4 => './caller.php?id=echolink_dl3el0',
			5 => './caller.php?id=relais0',
			6 => './caller.php?id=include/svxdxc&refresh=15',
			7 => './caller.php?id=aprs',
			8 => './caller_extern.php?id=https://websdr.z-05.de/#freq=144800000,mod=empty,secondary_mod=packet,sql=-150',
			9 => './caller_extern.php?id=https://stream.fm-funknetz.de',
			10 => './caller_extern.php?id=https://qsolink.websdrbordeaux.fr/dashboard.html',
			11 => 'https://openhamclock.com/',
//			12 => './caller_extern.php?id=https://dashboard.fm-funknetz.de/chat_jd10/index.php?id=' . $callsign . '',
			12 => './caller_extern.php?id=https://chat.fm-funknetz.de/index.php?call=' . $callsign,
			//13 => './caller.php?id=neue_seite1',
			//14 => './caller_extern.php?id=neue_seite2'  // bis20 !!//
		];

		// Prüfen, ob ein Eintrag existiert
		if (isset($urls[$val])) {
			header("Location: " . $urls[$val]);
			exit;
		}

		// Extern 1-4
		$externMap = [
			21 => 'DL3EL_EXTERN1',
			22 => 'DL3EL_EXTERN2',
			23 => 'DL3EL_EXTERN3',
			24 => 'DL3EL_EXTERN4',
		];

		if (isset($externMap[$val])) {
			$constName = $externMap[$val];
			if (defined($constName)) {
				header('Location: ./caller_extern.php?id=' . constant($constName));
				exit;
			}
		}
	}

	//////////////////////// System FKT //////////////////////////////////////////
	if (isset($_POST['svxsystem'])) {
		$val = $_POST['svxsystem'];

		// Mapping von Werten zu URLs
		$redirects = [
			3 => './config_backup.php',
			4 => './index.php'
		];

		// Classic Menü für Option 4
		if ($val == 4) {
			file_put_contents("dl3el/menu", "classic");
		}

		// Weiterleitung, wenn der Wert existiert
		if (isset($redirects[$val])) {
			header('Location: ' . $redirects[$val]);
			exit;
		}
	}
	//////////////////////// Expert Edit FKT //////////////////////////////////////////
	if (isset($_POST['expertedit'])) {
		$val = $_POST['expertedit'];

		// Mapping: Wert => ['type' => 'file'|'page', 'target' => Pfad oder Seite, 'condition' => optional]
		$svxRadio = DL3EL_RADIO;

		$map = [
			1 => ['type' => 'file', 'target' => SVXCONFPATH . 'svxlink.conf'],
			2 => ['type' => 'file', 'target' => MODULEPATH . 'ModuleEchoLink.conf'],
			3 => ['type' => 'file', 'target' => MODULEPATH . 'ModuleMetarInfo.conf'],
			4 => ['type' => 'file', 'target' => DL3EL . '/aprs-is-msg.conf'],
			5 => ['type' => 'page', 'target' => 'amixer/index', 'condition' => in_array($svxRadio, ["Shari", "RFGuru"]) || str_starts_with($svxRadio, "SC ")],
			//6 => ['type' => 'page', 'target' => './rf.php'],
			7 => ['type' => 'file', 'target' => SVXCONFPATH . 'node_info.json'],
			8 => ['type' => 'file', 'target' => DL3EL_BASE . 'include/config.php'],
		];

		if (isset($map[$val])) {
			$entry = $map[$val];

			if (!isset($entry['condition']) || $entry['condition']) {
				if ($entry['type'] === 'file') {
					header("Location: ./caller.php?file={$entry['target']}");
				} else {
					header("Location: ./caller.php?id={$entry['target']}");
				}
				exit;
			}
		}
		if ($val == 6) {
			header("Location: ./rf.php");
			exit;
		}
		if ($val == 9) {
			header("Location: ./wifi.php");
			exit;
		}

	}

	//////////////////////// Normal Edit FKT //////////////////////////////////////////
	if (isset($_POST['normaledit'])) {

		$routes = [
			1 => './editor.php?id=svxlink',
			2 => './editor.php?id=echolink',
			3 => './editor.php?id=metarinfo',
			4 => './caller.php?id=amixer/index',
			5 => './rf.php',
			6 => './editor.php?id=nodeInfo',
		];

		$val = (int) $_POST['normaledit'];

		if (array_key_exists($val, $routes)) {
			header("Location: " . $routes[$val]);
			exit;
		}
	}

	//////////////////////// Reflector Config  FKT //////////////////////////////////////////
	function refconf($refconf)
	{
		return $refconf;
	}

	if (isset($_POST['refconf'])) {
		$val = (int) $_POST['refconf'];
		if ($val >= 1 && $val <= 4) {
			$file_edit = DL3EL . "/Reflector{$val}.conf";
			header("Location: ./caller.php?file=" . urlencode($file_edit));
			exit;
		}
	}

	//////////////////////// Log Dateien FKT //////////////////////////////////////////
	if (isset($_POST['logdat'])) {
		$val = $_POST['logdat'];

		// Mapping der Werte zu den URLs
		$redirects = [
			1 => './editor.php?id=log',     // SVXLink Live Log
			2 => './caller.php?file=log',   // SVXLink Log
			3 => './caller.php?file=loga',  // APRS Log
		];

		// Prüfen, ob $val im Array existiert
		if (isset($redirects[$val])) {
			header("Location: " . $redirects[$val]);
			exit;
		}
	}

	////////////// Option Dashboard für Relais eingeschränktes Menu ///////////////////////////////////////////////////////////////////////////////////
	if ($public_relais == true) {
		// Dashboard Button
		echo '<span style="font-size:18px;font-weight:bold;">Display:&nbsp;&nbsp;&nbsp;<span>';
		echo '<a href="./index.php" style="margin-right:10px;font-size:18px;color:#CD6600" target="_top">Dashboard></a>| ';
		echo '<a href="./caller.php?id=include/svxdxc&refresh=15" style = "font-size:18px;color: #0000ff;" target="_top">&nbsp;DX</a>';
	} else {

		////////////// Option Dashboard mit Drop Down Menu ////////////////////////////////////////////////////////////////////////////////////////
		// Help & Info Button
		list($version, $rest) = explode(" ", $dbversion);
		$dbversionFile = DL3EL . "/dbversion.upd";
		if (file_exists($dbversionFile)) {
			$content = file_get_contents($dbversionFile);
			if ($content === "update") {
				$version = $version . " Update available";
			}
		}
		echo '<a href="./caller_extern.php?id=http://relais.dl3el.de/FM-Funknetz/hilfe.html&wid=950"
                        style="text-align:left;margin-right:20px;border-radius:8px;color:white;
                        border-color:transparent;background-color:blue;font-size:20px;"
                        id="info">Hilfe für ' . $version . '</a>';

		// Dashboard return Button
		echo '<a href="./index.php" style="margin-right:10px;font-size:18px;color:#CD6600" target="_top">Dashboard</a>';

		/////////////////////// Drop Down Menu Optionen ////////////////////////////////////
		echo '<form method="post" action="" style="display:inline;">';

		// Menü Display Inline /////////////////////////////
		if ($ohc == false) {
			$options = [
				['value' => '', 'label' => 'DISPLAY', 'hidden' => true, 'disabled' => true, 'selected' => true],
				//['value' => '1', 'label' => 'Dashboard'],
				['value' => '2', 'label' => 'Talk Groups'],
				['value' => '3', 'label' => 'Monitor Calls'],
				['value' => '4', 'label' => 'Echolink'],
				['value' => '5', 'label' => 'FM Relais'],
				['value' => '6', 'label' => 'DX Cluster', 'condition' => $cluster === true],
				['value' => '7', 'label' => 'APRS Monitor'],
				['value' => '8', 'label' => 'OWR', 'condition' => $owrx === true],
				['value' => '9', 'label' => 'Voice FMN'],
				['value' => '10', 'label' => 'QSO Cockpit'],
				//['value' => '11', 'label' => 'OHC'],
				['value' => '12', 'label' => 'Web Chat'],
			];
		} else {
			$options = [
				['value' => '', 'label' => 'DISPLAY', 'hidden' => true, 'disabled' => true, 'selected' => true],
				//['value' => '1', 'label' => 'Dashboard'],
				['value' => '2', 'label' => 'Talk Groups'],
				['value' => '3', 'label' => 'Monitor Calls'],
				['value' => '4', 'label' => 'Echolink'],
				['value' => '5', 'label' => 'FM Relais'],
				['value' => '6', 'label' => 'DX Cluster', 'condition' => $cluster === true],
				['value' => '7', 'label' => 'APRS Monitor'],
				['value' => '8', 'label' => 'OWR', 'condition' => $owrx === true],
				['value' => '9', 'label' => 'Voice FMN'],
				['value' => '12', 'label' => 'FMN WebChat'],
				['value' => '10', 'label' => 'QSO Cockpit'],
				['value' => '11', 'label' => 'OHC'],
			];
		}

		echo '<select style="font-size:16px;" name="display" onchange="this.form.submit()">';
		foreach ($options as $option) {
			if (isset($option['condition']) && !$option['condition'])
				continue;

			$attrs = '';
			foreach (['hidden', 'disabled', 'selected'] as $attr) {
				if (!empty($option[$attr]))
					$attrs .= " $attr";
			}
			echo "<option value=\"{$option['value']}\"$attrs>{$option['label']}</option>";
		}
		// Extern 1-4 Inline
		for ($i = 1; $i <= 4; $i++) {

			$tabConst = "DL3EL_EXTERN{$i}_TAB";
			$mainConst = "DL3EL_EXTERN{$i}";
			$nameConst = "DL3EL_EXTERN{$i}_NAME";
			$value = 20 + $i; // value 21-24

			// Überspringen wenn TAB = "new"
			if (defined($tabConst) && constant($tabConst) === "new") {
				continue;
			}

			// Nur anzeigen wenn Extern definiert ist
			if (defined($mainConst)) {

				$linktext = defined($nameConst)
					? constant($nameConst)
					: "Ext {$i}";

				echo "<option value='{$value}'>{$linktext}</option>";
			}
		}
		echo '</select>';

		// Menü Externe Links im neuen TAB ////////////////////////
		//if ((!defined("DL3EL_EXTERN1_TAB")) && (!defined("DL3EL_EXTERN2_TAB")) && (!defined("DL3EL_EXTERN3_TAB")) && (!defined("DL3EL_EXTERN3_TAB"))) {
		//	$ext_link_menu = false;
		//} else { $ext_link_menu = true;}

		if ($ext_link_menu == true) {
			echo '<select style="font-size:16px;" onchange="sopen(this)">';
			echo '<option value="" hidden disabled selected>EXT LINKS</option>';

			for ($i = 1; $i <= 4; $i++) {

				$tabConst = "DL3EL_EXTERN{$i}_TAB";
				$linkConst = "DL3EL_EXTERN{$i}";
				$nameConst = "DL3EL_EXTERN{$i}_NAME";

				if (
					defined($tabConst) &&
					constant($tabConst) === "new" &&
					defined($linkConst)
				) {

					$link = constant($linkConst);

					$linktext = defined($nameConst)
						? constant($nameConst)
						: "Ext {$i}";

					echo "<option value='" . htmlspecialchars($link, ENT_QUOTES, 'UTF-8') . "'>"
						. htmlspecialchars($linktext, ENT_QUOTES, 'UTF-8')
						. "</option>";
				}
			}
			echo '</select>';
			echo '<script>
		function sopen(select) {
		if (select.value !== "") {
			window.open(select.value, "_blank");
			select.selectedIndex = 0;
			}
		}
		</script>';
		}

		// Menü System Eintellungen /////////////////////
		echo '<select style="font-size:16px;" name="svxsystem" onchange="this.form.submit()">';
		echo '<option value="" hidden disabled selected>SYSTEM</option>';
		echo '<option value="4">Classic Menü</option>';
		echo '<option value="3">Backup & Restore</option>';
		echo '</select>';

		// Menü Expert Edit /////////////////////////
		if ($expert == true) {
			echo '<select style="font-size:16px;" name="expertedit" onchange="this.form.submit()">';
			echo '<option value="" hidden disabled selected>EXPERT EDIT</option>';
			echo '<option value="1">SVXLink Conf</option>';
			echo '<option value="2">Echolink Conf</option>';
			echo '<option value="3">Metar Info Conf</option>';
			echo '<option value="4">APRS Conf</option>';
			echo '<option value="5">AMixer</option>';
			echo '<option value="6">Radio</option>';
			echo '<option value="7">Node Info</option>';
			echo '<option value="8">config.php</option>';
			echo '<option value="9">Wifi</option>';
			echo '</select>';
		} else {
		}

		// MENÜ Normal Edit ///////////////////////
		if ($normal == true) {
			echo '<select style="font-size:16px;" name="normaledit" onchange="this.form.submit()">';
			echo '<option value="" hidden disabled selected>NORMAL EDIT</option>';
			echo '<option value="1">SVXLink Conf</option>';
			echo '<option value="2">Echolink Conf</option>';
			echo '<option value="3">Metar Info Conf</option>';
			echo '<option value="4">AMixer</option>';
			echo '<option value="5">Radio</option>';
			echo '<option value="6">Node Info</option>';
			echo '</select>';
		}

		// MENÜ Reflector Config /////////////////////////
		if ($reflector == true) {
			if (defined("DL3EL_REF1_BUTTON")) {
				$ref_name_1 = DL3EL_REF1_BUTTON;
			}
			if (defined("DL3EL_REF2_BUTTON")) {
				$ref_name_2 = DL3EL_REF2_BUTTON;
			}
			if (defined("DL3EL_REF3_BUTTON")) {
				$ref_name_3 = DL3EL_REF3_BUTTON;
			}
			if (defined("DL3EL_REF4_BUTTON")) {
				$ref_name_4 = DL3EL_REF4_BUTTON;
			}

			echo '<select style="font-size:16px;" name="refconf" onchange="this.form.submit()">';
			echo '<option value="" hidden disabled selected>REF CONF</option>';
			if (defined("DL3EL_REF1_BUTTON")) {
				echo "<option value='1'>$ref_name_1</option>";
			}
			if (defined("DL3EL_REF2_BUTTON")) {
				echo "<option value='2'>$ref_name_2</option>";
			}
			if (defined("DL3EL_REF3_BUTTON")) {
				echo "<option value='3'>$ref_name_3</option>";
			}
			if (defined("DL3EL_REF4_BUTTON")) {
				echo "<option value='4'>$ref_name_4</option>";
			}
			echo '</select>';
		}

		// MENÜ Log Dateien /////////////////////////
		echo '<select style="font-size:16px;" name="logdat" onchange="this.form.submit()">';
		echo '<option value="" hidden disabled selected>LOG FILES</option>';
		echo '<option value="1">SVXLink Live Log</option>';
		echo '<option value="2">SVXLink Log</option>';
		echo '<option value="3">APRS Log</option>';
		echo '</select>';

		echo '<noscript><input type="submit" value="send"></noscript>';

		// Button Authorise
		if ((defined('MENUTOPAUTH')) && (MENUTOPAUTH === "no")) {
			$show_auth = 0;
			// do not show Authorise
		} else {
			$show_auth = 1;
		}
		if ((defined('debug')) && (debug > 0))
			echo "<br>ShowAUTH: $show_auth";
		$ip = isset($_SERVER['REMOTE_ADDR']) ? $_SERVER['REMOTE_ADDR'] : '0';
		;
		if ((defined('debug')) && (debug > 0))
			echo "<br>RemoteIP: $ip";
		if (defined('DL3EL_MASTER_IP')) {
			$master_ip_array = explode(",", DL3EL_MASTER_IP);
			$nn = 0;
			foreach ($master_ip_array as $master_ip) {
				if ((defined('debug')) && (debug > 0))
					echo "<br>IP($nn):$master_ip ($ip)";
				if (cidr_match($ip, $master_ip)) {
					$_SESSION['auth'] = "AUTHORISED";
					$show_auth = 1;
					break;
				}
				++$nn;
			}
		}

		// Normal / Expert Button
		if (DL3EL_EXPERT == "yes") {
			echo '<button style="color:' . $kn_exp_col . ';font-size:16px;" name="btn_expert" type="submit" >' . $kn_exp . '</button>';
			echo '<button style="color:' . $kn_nor_col . ';font-size:16px;" name="btn_normal" type="submit" >' . $kn_nor . '</button>';
		}

		// Auth Button
		if ($show_auth) {
			echo '<a href="./authorise.php" style="color:crimson;margin-left:10px;font-size:18px;">Authorise</a>';
		}

		// Button Power
		echo '<a style="color:green;margin-left:10px;font-size:18px;" href="./caller.php?id=power/index" >Power</a>';
		echo '</form>';
		echo '<br>';
		echo '<div id="display-links" align=center>';

		echo '</div>';
	}

}

?>
