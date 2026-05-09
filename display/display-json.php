<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
error_reporting(-1);

define('DISPLAY_API_VERSION', '1.0.1');
define('DTMF_FIFO', '/tmp/dtmf_svx');

if ((defined ('debug')) && (debug > 0)) echo "Ich bin gerade hier: " . getcwd() . "\n";
//define ("debug", "0");

if (is_readable("../include/config.php")) {
	include_once "../include/config.php";
	//include_once "../include/functions.php";
	define('SVXLINK_CONF', SVXCONFPATH . SVXCONFIG);
	define('SVXLINK_LOG', SVXLOGPATH . SVXLOGPREFIX);
	$fmnetwork = get_network();
	define('DL3EL_DB', 1);
} else {	
	define('SVXLINK_CONF', '/etc/svxlink/svxlink.conf');
	define('SVXLINK_LOG', '/var/log/svxlink');
	define('DL3EL_DB', 0);
}
if ((defined ('debug')) && (debug > 0)) echo "SVXLINK_CONF: " . SVXLINK_CONF . "\n";
if ((defined ('debug')) && (debug > 0)) echo "SVXLINK_LOG: " . SVXLINK_LOG . "\n";

function get_network() {
    $svxConfigFile = SVXCONFPATH . SVXCONFIG;
    $svxconfig = parse_ini_file($svxConfigFile,true,INI_SCANNER_RAW);
    $section = "ReflectorLogic";
    $fmnetwork_all = isset($svxconfig[$section]['HOSTS']) ? $svxconfig[$section]['HOSTS'] : $fmnetwork =$svxconfig[$section]['DNS_DOMAIN'];
	$fmnetwork_arry = explode(",", $fmnetwork_all);
	$fmnetwork = $fmnetwork_arry[0];
    return $fmnetwork;
}

function read_conf() {
    if (is_readable(SVXLINK_CONF)) {
        $conf = parse_ini_file(SVXLINK_CONF, true, INI_SCANNER_RAW);
        return is_array($conf) ? $conf : array();
    }
    return array();
}

function conf_value($conf, $section, $key, $default = '') {
    return isset($conf[$section]) && isset($conf[$section][$key]) ? trim((string)$conf[$section][$key]) : $default;
}

function split_list($value) {
    $value = trim((string)$value);
    if ($value === '') return array();
    $parts = preg_split('/[\s,]+/', $value);
    return array_values(array_filter(array_map('trim', $parts), 'strlen'));
}

function svxlink_log_candidates($path) {
    $path = (string)$path;
    if ($path === '') return array();
    if (is_file($path) && is_readable($path)) return array($path);

    $files = array();
    if (is_dir($path) && is_readable($path)) {
        foreach (array('*.log', 'svxlink*', '*') as $pattern) {
            foreach (glob(rtrim($path, '/') . '/' . $pattern) ?: array() as $candidate) {
                if (is_file($candidate) && is_readable($candidate)) $files[$candidate] = @filemtime($candidate) ?: 0;
            }
        }
    }
    arsort($files, SORT_NUMERIC);
    return array_slice(array_keys($files), 0, 3);
}

function tail_lines($file, $count = 800) {
    $out = array();
    foreach (svxlink_log_candidates($file) as $candidate) {
        $cmd = 'tail -n ' . intval($count) . ' ' . escapeshellarg($candidate) . ' 2>/dev/null';
        $part = array();
        exec($cmd, $part);
        if (count($part)) $out = array_merge($out, $part);
    }
    return $out;
}

function last_line_matching($lines, $regex) {
    for ($i = count($lines) - 1; $i >= 0; $i--) {
        if (preg_match($regex, $lines[$i])) return $lines[$i];
    }
    return '';
}

function last_index_matching($lines, $regex) {
    for ($i = count($lines) - 1; $i >= 0; $i--) {
        if (preg_match($regex, $lines[$i])) return $i;
    }
    return -1;
}

function reflector_configured($conf) {
    $logics = split_list(conf_value($conf, 'GLOBAL', 'LOGICS', ''));
    $hasLogic = in_array('ReflectorLogic', $logics, true) || isset($conf['ReflectorLogic']);
	if (DL3EL_DB) {
		$host = $fmnetwork;
	} else {
		$host = conf_value($conf, 'ReflectorLogic', 'HOST', conf_value($conf, 'ReflectorLogic', 'HOSTS', ''));
	}    
	if ((defined ('debug')) && (debug > 0)) echo "Net: $fmnetwork\n";
    return $hasLogic && trim((string)$host) !== '';
}

function radio_status($lines) {
    $line = last_line_matching($lines, '/Turning the transmitter|squelch is|Talker start on|Talker stop on/i');
    if ($line === '') return 'Unknown';
    if (stripos($line, 'Turning the transmitter ON') !== false) return 'TX';
    if (stripos($line, 'Talker start on') !== false) return 'RX';
    if (stripos($line, 'squelch is OPEN') !== false) return 'RX';
    return 'Listening';
}

function reflector_status($lines, $conf,$fmnetwork) {
    $connectedRegex = '/Authentication OK|Connection established|Activating link|connected to|reflector.*connected|SVXReflector.*connected/i';
    $disconnectedRegex = '/Heartbeat timeout|No route to host|Connection refused|Connection timed out|Locally ordered disconnect|Deactivating link|disconnect(?:ed)?|not connected/i';
    $activityRegex = '/Talker (?:start|stop) on TG|Selecting TG #?\s*[0-9]+/i';

    $connectedIndex = last_index_matching($lines, $connectedRegex);
    $disconnectedIndex = last_index_matching($lines, $disconnectedRegex);
    $activityIndex = last_index_matching($lines, $activityRegex);

	if (DL3EL_DB) {
		if ($activityIndex >= 0 && $activityIndex > $disconnectedIndex) return $fmnetwork;
		if ($connectedIndex >= 0 && $connectedIndex > $disconnectedIndex) return $fmnetwork;;
		if ($disconnectedIndex >= 0 && $disconnectedIndex >= $connectedIndex) return $fmnetwork;
	} else {
		if ($activityIndex >= 0 && $activityIndex > $disconnectedIndex) return 'Connected';
		if ($connectedIndex >= 0 && $connectedIndex > $disconnectedIndex) return 'Connected';
		if ($disconnectedIndex >= 0 && $disconnectedIndex >= $connectedIndex) return 'Not connected';
	}	

    if (reflector_configured($conf) && process_running('svxlink')) return 'Connected';

    return 'No status';
}

function process_running($name) {
    $out = array();
    exec('pidof ' . escapeshellarg($name) . ' 2>/dev/null', $out);
    return count($out) > 0;
}

function active_modules($conf, $lines) {
    $known = array();
    foreach ($conf as $section => $items) {
        if (isset($items['MODULES'])) {
            foreach (split_list($items['MODULES']) as $m) $known[$m] = 'Off';
        }
    }
    foreach ($lines as $line) {
        if (preg_match('/Activating module\s+([^\s:]+)/i', $line, $m)) $known[rtrim($m[1], ':')] = 'On';
        if (preg_match('/Deactivating module\s+([^\s:]+)/i', $line, $m)) $known[rtrim($m[1], ':')] = 'Off';
    }
    $out = array();
    foreach ($known as $name => $state) $out[] = array('name' => $name, 'active' => $state === 'On');
    return $out;
}

function selected_tg($lines) {
    $line = last_line_matching($lines, '/Selecting TG #?\s*([0-9]+)/i');
    if ($line && preg_match('/Selecting TG #?\s*([0-9]+)/i', $line, $m)) return $m[1];
    return '';
}

function last_heard($lines, $limit = 12) {
//
	$use_names = 0;
	if (DL3EL_DB) {
		$DMRIDFile = DL3EL . "/DMRIds.dat";
		if ((defined ('debug')) && (debug > 0)) echo  "ID: $DMRIDFile\n";
		$dmrIDline = file_get_contents($DMRIDFile);
		if (strlen($dmrIDline) > 1000000) {
		$use_names = 1;
		include_once "../include/tgdb.php";    
		}  
	}	
    if ((defined ('debug')) && (debug > 0)) echo "use names: $use_names\n";
//
    $heard = array();
    $seen = array();
    for ($i = count($lines) - 1; $i >= 0 && count($heard) < $limit; $i--) {
        $line = $lines[$i];
        if (!preg_match('/^(.{19}).*: Talker (start|stop) on TG #?\s*([0-9]+):\s*(.+)$/i', $line, $m)) continue;
        $time = trim($m[1]);
        $event = strtolower($m[2]);
        $tg = trim($m[3]);
        $call = trim($m[4]);
        if ($call === '' || strlen($call) > 24) continue;
//
		$station = $call;
		if ($use_names) {
			if ((defined ('debug')) && (debug > 0)) echo "($i) search Name: $call ";
			//Aufbereitung LastCall zur Suche in DMRIds.dat (entfernt Bindestrich)
			$string = $call;
			$position = strpos($string, "-");		// Stelle des "-" zurück
			if ($position !== false) {			// Prüft, ob das Zeichen gefunden wurde
				$call = substr($string, 0, $position);	//schneidet ab "-" ab
			}
			// Suche Name zum Call in DMRIds.da
			if ((defined ('debug')) && (debug > 0)) echo "search Name: $call ";
			$arr2 = $call;
			if (isset($call_array[$call])) {
				if ((defined ('debug')) && (debug > 0)) echo "$call already found: $call_array[$call] \n";
				$name = $call_array[$call];
			} else {	 
				if ((defined ('debug')) && (debug > 0)) echo "search Name: $call ";
				if ((substr($call,0,3) === "DB0") || (substr($call,0,3) === "DM0") || (substr($call,0,3) === "DO0")) {
					if ((defined ('debug')) && (debug > 0)) echo "Call is Repeater, no names \n";
					$name = "";
				} else {	
					$pos = strpos($dmrIDline, $arr2." ");
					if ($pos != false) {
						$name = substr($dmrIDline, ($pos + strlen($arr2." ")));
						$name = ltrim($name, " ");
						$x = strpos($name, "\n");
						$y = strpos($name, " ");
						$name = rtrim($name, " ");
						if ($x < $y) {
							$name = substr($name, 0, $x);
						} else {
							$name = substr($name, 0, $y);
						}
						if ((defined ('debug')) && (debug > 0)) echo "Name: $name \n";
						$call_array[$call] = $name;
					}	
				}	
			}	
		} else {
			$name = "";
		}
	
//
// TG
		if (DL3EL_DB) {
			$tgnumber = $tg;
			$tgname = ltrim(isset($tgdb_array[$tgnumber])? $tgdb_array[$tgnumber] : '');
			if ( $tgnumber>=400 and $tgnumber<= 499){ $tgname ="AUTO QSY";}
		} else {
			$tgname = "";
		}	

//
        $key = $call . '#' . $tg;
        if (isset($seen[$key])) continue;
        $seen[$key] = true;
        $heard[] = array(
            'time' => $time,
            'time_local' => $time,
            'mode' => 'SVXLink',
            'source_mode' => 'SVXLink',
            'callsign' => $call,
            'station' => $station,
            'name' => $name,
            'target' => 'TG ' . $tg,
            'talkgroup' => $tg,
            'target_name' => '',
            'talkgroup_name' => $tgname,
            'src' => 'SVXRef',
            'source_system' => 'SVXRef',
            'tx' => $event === 'start' ? 'ON' : 'OFF'
        );
    }
    return $heard;
}

function tg_list($conf) {
    $default = conf_value($conf, 'ReflectorLogic', 'DEFAULT_TG', '');
    $monitor = split_list(conf_value($conf, 'ReflectorLogic', 'MONITOR_TGS', ''));
    $out = array();
    if ($default !== '') $out[] = array('id' => $default, 'name' => 'Default TG', 'default' => true, 'monitored' => in_array($default, $monitor));
    foreach ($monitor as $tg) {
        if ($tg !== $default) $out[] = array('id' => $tg, 'name' => 'Monitor TG', 'default' => false, 'monitored' => true);
    }
    return $out;
}

function send_dtmf_command($command) {
    $ok = file_put_contents(DTMF_FIFO, $command . PHP_EOL, LOCK_EX);
    if ($ok === false) {
        http_response_code(500);
        echo json_encode(array('ok' => false, 'error' => 'Could not write DTMF command to ' . DTMF_FIFO));
        exit;
    }
    echo json_encode(array('ok' => true, 'command' => $command));
    exit;
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $action = isset($_POST['action']) ? trim($_POST['action']) : '';
    $value = isset($_POST['value']) ? trim($_POST['value']) : '';
    if ($action === 'dtmf') {
        $cmd = preg_replace('/[^0-9A-Da-d\*#]/', '', $value);
        if ($cmd === '') { http_response_code(400); echo json_encode(array('ok'=>false,'error'=>'Missing DTMF value')); exit; }
        send_dtmf_command(strtoupper($cmd));
    }
    if ($action === 'active_tg') {
        $tg = preg_replace('/[^0-9]/', '', $value);
        if ($tg === '') { http_response_code(400); echo json_encode(array('ok'=>false,'error'=>'Missing TG value')); exit; }
        send_dtmf_command('91' . $tg . '#');
    }
    if ($action === 'monitor_tg') {
        $tg = preg_replace('/[^0-9]/', '', $value);
        if ($tg === '') { http_response_code(400); echo json_encode(array('ok'=>false,'error'=>'Missing TG value')); exit; }
        send_dtmf_command('94' . $tg . '#');
    }
    http_response_code(400);
    echo json_encode(array('ok' => false, 'error' => 'Unknown action'));
    exit;
}

$conf = read_conf();
$lines = tail_lines(SVXLINK_LOG, 12000);
$logicNames = split_list(conf_value($conf, 'GLOBAL', 'LOGICS', ''));
$modules = active_modules($conf, $lines);
$defaultTg = conf_value($conf, 'ReflectorLogic', 'DEFAULT_TG', '');
$activeTg = selected_tg($lines);
if ($activeTg === '') $activeTg = $defaultTg;
$monitorTgs = split_list(conf_value($conf, 'ReflectorLogic', 'MONITOR_TGS', ''));
//
if (DL3EL_DB) {
    if ((defined ('debug')) && (debug > 0)) echo "Net: $fmnetwork\n";
    $callsign = conf_value($conf, 'SimplexLogic', 'CALLSIGN', conf_value($conf, 'RepeaterLogic', 'CALLSIGN', ''));
} else {
	$fmnetwork = conf_value($conf, 'ReflectorLogic', 'HOST', conf_value($conf, 'ReflectorLogic', 'HOSTS', ''));
    $callsign = defined('CALLSIGN') ? CALLSIGN : conf_value($conf, 'SimplexLogic', 'CALLSIGN', conf_value($conf, 'RepeaterLogic', 'CALLSIGN', ''));
}    
//
$response = array(
    'ok' => true,
    'api' => 'mmdvm-display-svxlink',
    'api_version' => DISPLAY_API_VERSION,
    'backend' => 'SVXLink',
//    'callsign' => defined('CALLSIGN') ? CALLSIGN : conf_value($conf, 'SimplexLogic', 'CALLSIGN', conf_value($conf, 'RepeaterLogic', 'CALLSIGN', '')),
    'callsign' => $callsign,
//    'network_name' => conf_value($conf, 'ReflectorLogic', 'HOSTS', ''),
    'network_name' => $fmnetwork,
    'system' => array(
        'process' => process_running('svxlink') ? 'Running' : 'Stopped',
        'reboot' => trim(shell_exec('uptime -s 2>/dev/null')),
        'mode' => count($logicNames) ? implode(', ', $logicNames) : 'SVXLink'
    ),
    'radio_status' => radio_status($lines),
    'reflector_status' => reflector_status($lines, $conf,$fmnetwork),
//    'reflector_host' => conf_value($conf, 'ReflectorLogic', 'HOST', conf_value($conf, 'ReflectorLogic', 'HOSTS', '')),
    'reflector_host' => $fmnetwork,
    'logics' => $logicNames,
    'modules' => $modules,
    'tg' => array(
        'default' => $defaultTg,
        'active' => $activeTg,
        'monitor' => $monitorTgs,
        'list' => tg_list($conf)
    ),
    'last_heard' => last_heard($lines, isset($_GET['limit']) ? intval($_GET['limit']) : 12),
    'controls' => array(
        'dtmf' => true,
        'active_tg' => true,
        'monitor_tg' => true
    )
);

echo json_encode($response, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
