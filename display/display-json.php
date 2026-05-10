<?php
header('Content-Type: application/json; charset=utf-8');
header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
error_reporting(0);

define('DISPLAY_API_VERSION', '1.1.0');
// DL3EL
error_reporting(-1);
//define ("debug", "1");
if ((defined ('debug')) && (debug > 0)) echo "Ich bin gerade hier: " . getcwd() . "\n";

if (is_readable("../include/config.php")) {
	include_once "../include/config.php";
	//include_once "../include/functions.php";
	define('SVXLINK_CONF', SVXCONFPATH . SVXCONFIG);
	define('SVXLINK_LOG', SVXLOGPATH . SVXLOGPREFIX);
	$fmnetwork = get_network();
	define('DL3EL_DB', 1);
    $dashboardRoot = DL3EL_BASE;
    $dashboardConfig = $dashboardRoot . 'include/config.php';
    $dashboardBaseDir = $dashboardRoot . 'include/basedir.php';
    $dashboardTgDb = $dashboardRoot . 'include/tgdb.php';
} else {	
	define('SVXLINK_CONF', '/etc/svxlink/svxlink.conf');
	define('SVXLINK_LOG', '/var/log/svxlink');
	define('DL3EL_DB', 0);
    $dashboardRoot = __DIR__;
    $dashboardConfig = $dashboardRoot . '/include/config.php';
    $dashboardBaseDir = $dashboardRoot . '/include/basedir.php';
    $dashboardTgDb = $dashboardRoot . '/include/tgdb.php';
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
// DL3EL
define('DEFAULT_DTMF_FIFO', '/tmp/dtmf_svx');

if ((defined ('debug')) && (debug > 0)) echo "DB: $dashboardRoot $dashboardConfig $dashboardBaseDir\n";

if (is_readable($dashboardConfig) && is_readable($dashboardBaseDir)) {
    @include_once $dashboardConfig;
}

$tgdb_array = array();
if (is_readable($dashboardTgDb)) {
    @include $dashboardTgDb;
    if (!isset($tgdb_array) || !is_array($tgdb_array)) $tgdb_array = array();
}

function const_value($name, $default = '') {
    return defined($name) ? constant($name) : $default;
}

function svx_conf_path() {
    $confPath = const_value('SVXCONFPATH', '');
    $confFile = const_value('SVXCONFIG', '');
    if ($confPath !== '' && $confFile !== '') {
        return rtrim($confPath, '/') . '/' . ltrim($confFile, '/');
    }
    return DEFAULT_SVXLINK_CONF;
}

function svx_log_path() {
    $logPath = const_value('SVXLOGPATH', '');
    $logPrefix = const_value('SVXLOGPREFIX', '');
    if ($logPath !== '' && $logPrefix !== '') {
        return rtrim($logPath, '/') . '/' . ltrim($logPrefix, '/');
    }
    return DEFAULT_SVXLINK_LOG;
}

function dtmf_fifo_path() {
    $pty = const_value('DTMF_CTRL_PTY', '');
    if ($pty !== '' && strtolower($pty) !== 'null') return $pty;
    return DEFAULT_DTMF_FIFO;
}

function read_conf() {
    $file = svx_conf_path();
    if (is_readable($file)) {
        $conf = parse_ini_file($file, true, INI_SCANNER_RAW);
        return is_array($conf) ? $conf : array();
    }
    return array();
}

function conf_value($conf, $section, $key, $default = '') {
    if (!isset($conf[$section]) || !isset($conf[$section][$key])) return $default;
    $value = trim((string)$conf[$section][$key]);
    return trim($value, " \"'");
}

function split_list($value) {
    $value = trim((string)$value);
    if ($value === '') return array();
    $parts = preg_split('/[\s,]+/', $value);
    return array_values(array_filter(array_map('trim', $parts), 'strlen'));
}

function strip_module_prefix($name) {
    $name = trim((string)$name);
    $name = rtrim($name, ':;.');
    return preg_replace('/^Module/i', '', $name);
}

function clean_text($value) {
    $value = html_entity_decode(strip_tags((string)$value), ENT_QUOTES, 'UTF-8');
    return trim(preg_replace('/\s+/', ' ', $value));
}

function tg_name($tg) {
    global $tgdb_array;
    $tg = preg_replace('/[^0-9]/', '', (string)$tg);
    if ($tg === '') return '';
    if ($tg >= 400 && $tg <= 499) return 'AUTO QSY';
    return isset($tgdb_array[$tg]) ? clean_text($tgdb_array[$tg]) : '';
}

function svxlink_log_candidates($path) {
    $path = (string)$path;
    if ($path === '') return array();

    $files = array();

    if (is_file($path) && is_readable($path)) {
        if (is_file($path . '.1') && is_readable($path . '.1')) {
            $files[$path . '.1'] = @filemtime($path . '.1') ?: 0;
        }
        $files[$path] = @filemtime($path) ?: time();
        asort($files, SORT_NUMERIC);
        return array_keys($files);
    }

    if (is_dir($path) && is_readable($path)) {
        foreach (array('svxlink*', '*.log', '*') as $pattern) {
            foreach (glob(rtrim($path, '/') . '/' . $pattern) ?: array() as $candidate) {
                if (is_file($candidate) && is_readable($candidate)) {
                    $files[$candidate] = @filemtime($candidate) ?: 0;
                }
            }
        }
        asort($files, SORT_NUMERIC);
        return array_slice(array_keys($files), -4);
    }

    return array();
}

function tail_lines($file, $count = 1200) {
    $out = array();
    foreach (svxlink_log_candidates($file) as $candidate) {
        $cmd = 'tail -n ' . intval($count) . ' ' . escapeshellarg($candidate) . ' 2>/dev/null';
        $part = array();
        exec($cmd, $part);
        if (count($part)) $out = array_merge($out, $part);
    }
    return $out;
}

function section_in_line($line, $section) {
    $section = trim((string)$section);
    if ($section === '') return true;
    return strpos($line, $section . ':') !== false;
}

function last_line_matching($lines, $regex, $section = '') {
    for ($i = count($lines) - 1; $i >= 0; $i--) {
        if ($section !== '' && !section_in_line($lines[$i], $section)) continue;
        if (preg_match($regex, $lines[$i])) return $lines[$i];
    }
    return '';
}

function last_index_matching($lines, $regex, $section = '') {
    for ($i = count($lines) - 1; $i >= 0; $i--) {
        if ($section !== '' && !section_in_line($lines[$i], $section)) continue;
        if (preg_match($regex, $lines[$i])) return $i;
    }
    return -1;
}

function process_running($name) {
    $out = array();
    exec('pidof ' . escapeshellarg($name) . ' 2>/dev/null', $out);
    return count($out) > 0;
}

function configured_logics($conf) {
    $logics = split_list(conf_value($conf, 'GLOBAL', 'LOGICS', ''));
    if (!count($logics)) {
        foreach (array('RepeaterLogic', 'SimplexLogic', 'ReflectorLogic') as $logic) {
            if (isset($conf[$logic])) $logics[] = $logic;
        }
    }
    return $logics;
}

function base_logic($conf, $logics) {
    foreach ($logics as $logic) {
        if ($logic === 'RepeaterLogic' || $logic === 'SimplexLogic') return $logic;
    }
    if (isset($conf['RepeaterLogic'])) return 'RepeaterLogic';
    if (isset($conf['SimplexLogic'])) return 'SimplexLogic';
    return count($logics) ? $logics[0] : '';
}

function reflector_sections($conf, $logics) {
    $sections = array();
    foreach ($logics as $logic) {
        if (strncmp($logic, 'ReflectorLogic', 14) === 0 && isset($conf[$logic])) $sections[] = $logic;
    }
    foreach ($conf as $section => $items) {
        if (strncmp($section, 'ReflectorLogic', 14) === 0 && !in_array($section, $sections, true)) $sections[] = $section;
    }
    return $sections;
}

function callsign_from_conf($conf, $logics, $reflectors) {
    if (isset($GLOBALS['callsign']) && trim((string)$GLOBALS['callsign']) !== '') return trim((string)$GLOBALS['callsign']);
    foreach (array_merge($reflectors, $logics, array('ReflectorLogic', 'SimplexLogic', 'RepeaterLogic')) as $section) {
        $value = conf_value($conf, $section, 'CALLSIGN', '');
        if ($value !== '') return $value;
    }
    $const = const_value('CALLSIGN', '');
    return strtolower($const) === 'null' ? '' : $const;
}

function network_name_from_conf($conf, $reflector) {
    if ($reflector === '') return '';
    return conf_value($conf, $reflector, 'HOSTS', conf_value($conf, $reflector, 'DNS_DOMAIN', conf_value($conf, $reflector, 'HOST', '')));
}

function host_from_conf($conf, $reflector) {
    if ($reflector === '') return '';
    return conf_value($conf, $reflector, 'HOST', conf_value($conf, $reflector, 'HOSTS', conf_value($conf, $reflector, 'DNS_DOMAIN', '')));
}

function radio_status($lines) {
    $line = last_line_matching($lines, '/Turning the transmitter|squelch is|squelch for|Talker start on|Talker stop on/i');
    if ($line === '') return 'Unknown';
    if (stripos($line, 'Turning the transmitter ON') !== false) return 'TX';
    if (stripos($line, 'Talker start on') !== false) return 'RX';
    if (stripos($line, 'squelch is OPEN') !== false) return 'RX';
    return 'Listening';
}

function reflector_status($lines, $reflector, $fmnetwork) {
    $statusRegex = '/Authentication|Connection established|Heartbeat timeout|No route to host|Connection refused|Connection timed out|Locally ordered disconnect|Deactivating link|Activating link|Disconnected/i';
    $line = last_line_matching($lines, $statusRegex, $reflector);
    if ($line === '') return 'No status';
	if (DL3EL_DB) {
        if (preg_match('/Authentication OK|Connection established|Activating link/i', $line)) return $fmnetwork;
        if (preg_match('/Heartbeat timeout|No route to host|Connection refused|Connection timed out|Locally ordered disconnect|Deactivating link|Disconnected/i', $line)) return 'Not connected';
    } else {
        if (preg_match('/Authentication OK|Connection established|Activating link/i', $line)) return 'Connected';
        if (preg_match('/Heartbeat timeout|No route to host|Connection refused|Connection timed out|Locally ordered disconnect|Deactivating link|Disconnected/i', $line)) return 'Not connected';
    }    
    return 'No status';
}

function active_modules($conf, $lines, $baseLogic) {
    $known = array();
    $sections = $baseLogic !== '' ? array($baseLogic) : array_keys($conf);
    foreach ($sections as $section) {
        foreach (split_list(conf_value($conf, $section, 'MODULES', '')) as $m) {
            $name = strip_module_prefix($m);
            if ($name !== '') $known[$name] = 'Off';
        }
    }
    foreach ($lines as $line) {
        if (preg_match('/Activating module\s+([^\s:]+)/i', $line, $m)) {
            $name = strip_module_prefix($m[1]);
            if ($name !== '') $known[$name] = 'On';
        }
        if (preg_match('/Deactivating module\s+([^\s:]+)/i', $line, $m)) {
            $name = strip_module_prefix($m[1]);
            if ($name !== '') $known[$name] = 'Off';
        }
    }
    $out = array();
    foreach ($known as $name => $state) $out[] = array('name' => $name, 'active' => $state === 'On');
    return $out;
}

function selected_tg($lines, $reflector = '') {
    $line = last_line_matching($lines, '/Selecting TG\s*#?\s*([0-9]+)/i', $reflector);
    if ($line && preg_match('/Selecting TG\s*#?\s*([0-9]+)/i', $line, $m)) return $m[1];
    return '';
}

function temporary_monitor_tg($lines, $reflector = '') {
    $line = last_line_matching($lines, '/temporary monitor|emporary monitor/i', $reflector);
    if ($line === '') return '';
    if (stripos($line, 'Remove') !== false || stripos($line, 'Delete') !== false) return '';
    if (preg_match('/#\s*([0-9]+)/', $line, $m)) return $m[1];
    if (preg_match('/TG\s*([0-9]+)/i', $line, $m)) return $m[1];
    return '';
}

function parse_log_time($time) {
    $time = trim((string)$time);
    if ($time === '') return 0;
    $ts = strtotime($time);
    return $ts === false ? 0 : $ts;
}

function short_local_time($time) {
    $time = trim((string)$time);
    if ($time === '') return '';
    if (preg_match('/(\d{1,2}:\d{2}:\d{2})/', $time, $m)) return $m[1];
    return $time;
}

function parse_talker_line($line) {
    if (!preg_match('/^(.+?):\s*([A-Za-z][A-Za-z0-9_+\-]*):\s*Talker\s+(start|stop)\s+on\s+TG\s*#?\s*([0-9]+)\s*:\s*(.+)$/i', $line, $m)) {
        return null;
    }
    $time = trim($m[1]);
    $reflector = trim($m[2]);
    $event = strtolower(trim($m[3]));
    $tg = trim($m[4]);
    $call = trim($m[5]);
    $call = preg_replace('/\s+/', ' ', $call);
    if ($call === '' || strlen($call) > 32) return null;

    $tx = 'OFF';
    if ($event === 'start') {
        $ts = parse_log_time($time);
        $tx = ($ts === 0 || (time() - $ts) <= 300) ? 'ON' : 'OFF';
    }

    return array(
        'time' => $time,
        'time_local' => short_local_time($time),
        'mode' => 'SVXLink',
        'source_mode' => 'SVXLink',
        'callsign' => $call,
        'station' => $call,
        'target' => 'TG ' . $tg,
        'talkgroup' => $tg,
        'target_name' => tg_name($tg),
        'talkgroup_name' => tg_name($tg),
        'src' => 'SVXRef',
        'source_system' => 'SVXRef',
        'reflector' => $reflector,
        'tx' => $tx
    );
}

function last_heard($lines, $limit = 12) {
    $limit = max(1, min(100, intval($limit)));
    $heard = array();
    $seen = array();
    for ($i = count($lines) - 1; $i >= 0 && count($heard) < $limit; $i--) {
        $item = parse_talker_line($lines[$i]);
        if (!$item) continue;
        $key = $item['callsign'] . '#' . $item['talkgroup'];
        if (isset($seen[$key])) continue;
        $seen[$key] = true;
        $heard[] = $item;
    }
    return $heard;
}

function tg_list_for_reflector($conf, $reflector, $activeTg, $temporaryTg) {
    $default = conf_value($conf, $reflector, 'DEFAULT_TG', '');
    $monitor = split_list(conf_value($conf, $reflector, 'MONITOR_TGS', ''));
    $out = array();

    $add = function($tg, $role, $isDefault, $isMonitored, $isActive, $isTemporary) use (&$out) {
        $tg = preg_replace('/[^0-9]/', '', (string)$tg);
        if ($tg === '') return;
        $out[$tg] = array(
            'id' => $tg,
            'name' => tg_name($tg),
            'role' => $role,
            'default' => $isDefault,
            'monitored' => $isMonitored,
            'active' => $isActive,
            'temporary' => $isTemporary
        );
    };

    $add($default, 'default', true, in_array($default, $monitor, true), $default !== '' && $default === $activeTg, false);
    foreach ($monitor as $tg) $add($tg, 'monitor', $tg === $default, true, $tg === $activeTg, false);
    $add($activeTg, 'active', $activeTg === $default, in_array($activeTg, $monitor, true), true, false);
    $add($temporaryTg, 'temporary_monitor', false, true, false, true);

    return array_values($out);
}

function build_reflectors($conf, $lines, $reflectors) {
    $out = array();
    foreach ($reflectors as $reflector) {
        $defaultTg = conf_value($conf, $reflector, 'DEFAULT_TG', '');
        $activeTg = selected_tg($lines, $reflector);
        if ($activeTg === '') $activeTg = $defaultTg;
        $temporaryTg = temporary_monitor_tg($lines, $reflector);
        $monitorTgs = split_list(conf_value($conf, $reflector, 'MONITOR_TGS', ''));
        $out[] = array(
            'name' => $reflector,
            'status' => reflector_status($lines, $reflector, $fmnetwork),
            'network_name' => network_name_from_conf($conf, $reflector),
            'host' => host_from_conf($conf, $reflector),
            'default_tg' => $defaultTg,
            'active_tg' => $activeTg,
            'temporary_monitor_tg' => $temporaryTg,
            'monitor_tgs' => $monitorTgs,
            'tg' => array(
                'default' => $defaultTg,
                'active' => $activeTg,
                'temporary_monitor' => $temporaryTg,
                'monitor' => $monitorTgs,
                'list' => tg_list_for_reflector($conf, $reflector, $activeTg, $temporaryTg)
            )
        );
    }
    return $out;
}

function send_json($payload, $statusCode = 200) {
    http_response_code($statusCode);
    echo json_encode($payload, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
    exit;
}

function send_dtmf_command($command) {
    $fifo = dtmf_fifo_path();
    $ok = file_put_contents($fifo, $command . PHP_EOL, LOCK_EX);
    if ($ok === false) {
        send_json(array('ok' => false, 'error' => 'Could not write DTMF command to ' . $fifo), 500);
    }
    send_json(array('ok' => true, 'command' => $command, 'dtmf_fifo' => $fifo));
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $action = isset($_POST['action']) ? trim($_POST['action']) : '';
    $value = isset($_POST['value']) ? trim($_POST['value']) : '';

    if ($action === 'dtmf') {
        $cmd = preg_replace('/[^0-9A-Da-d\*#]/', '', $value);
        if ($cmd === '') send_json(array('ok' => false, 'error' => 'Missing DTMF value'), 400);
        send_dtmf_command(strtoupper($cmd));
    }

    if ($action === 'active_tg' || $action === 'tg_active') {
        $tg = preg_replace('/[^0-9]/', '', $value);
        if ($tg === '') send_json(array('ok' => false, 'error' => 'Missing TG value'), 400);
        send_dtmf_command('*91' . $tg . '#');
    }

    if ($action === 'link_tg') {
        $tg = preg_replace('/[^0-9]/', '', $value);
        if ($tg === '') send_json(array('ok' => false, 'error' => 'Missing TG value'), 400);
        send_dtmf_command('91' . $tg . '#');
    }

    if ($action === 'monitor_tg' || $action === 'tg_monitor') {
        $tg = preg_replace('/[^0-9]/', '', $value);
        if ($tg === '') send_json(array('ok' => false, 'error' => 'Missing TG value'), 400);
        send_dtmf_command('94' . $tg . '#');
    }

    send_json(array('ok' => false, 'error' => 'Unknown action'), 400);
}

$conf = read_conf();
$lines = tail_lines(svx_log_path(), 1200);
$logicNames = configured_logics($conf);
$baseLogic = base_logic($conf, $logicNames);
$reflectorSections = reflector_sections($conf, $logicNames);
$primaryReflector = count($reflectorSections) ? $reflectorSections[0] : '';
$reflectors = build_reflectors($conf, $lines, $reflectorSections);
$primary = count($reflectors) ? $reflectors[0] : array('tg' => array('default' => '', 'active' => '', 'temporary_monitor' => '', 'monitor' => array(), 'list' => array()));

$response = array(
    'ok' => true,
    'api' => 'mmdvm-display-svxlink',
    'api_version' => DISPLAY_API_VERSION,
    'backend' => 'SVXLink',
    'dashboard' => array(
        'root' => __DIR__,
        'config_file' => svx_conf_path(),
        'log_file' => svx_log_path(),
        'dtmf_fifo' => dtmf_fifo_path()
    ),
    'callsign' => callsign_from_conf($conf, $logicNames, $reflectorSections),
    'network_name' => $primaryReflector !== '' ? network_name_from_conf($conf, $primaryReflector) : '',
    'system' => array(
        'process' => process_running('svxlink') ? 'Running' : 'Stopped',
        'reboot' => trim(shell_exec('uptime -s 2>/dev/null')),
        'mode' => count($logicNames) ? implode(', ', $logicNames) : 'SVXLink',
        'logic' => $baseLogic
    ),
    'radio_status' => radio_status($lines),
    'reflector_status' => $primaryReflector !== '' ? reflector_status($lines, $primaryReflector, $fmnetwork) : 'No status',
    'reflector_host' => $primaryReflector !== '' ? host_from_conf($conf, $primaryReflector, $fmnetwork) : '',
    'reflectors' => $reflectors,
    'logics' => $logicNames,
    'modules' => active_modules($conf, $lines, $baseLogic),
    'tg' => isset($primary['tg']) ? $primary['tg'] : array('default' => '', 'active' => '', 'temporary_monitor' => '', 'monitor' => array(), 'list' => array()),
    'last_heard' => last_heard($lines, isset($_GET['limit']) ? intval($_GET['limit']) : 12),
    'controls' => array(
        'dtmf' => true,
        'active_tg' => true,
        'link_tg' => true,
        'monitor_tg' => true,
        'active_tg_command' => '*91{tg}#',
        'link_tg_command' => '91{tg}#',
        'monitor_tg_command' => '94{tg}#'
    )
);

send_json($response);
