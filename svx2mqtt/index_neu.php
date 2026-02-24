<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
      if (isset($svx_include)) {
      } else {
        include_once "../include/settings.php";
        include_once "../include/tgdb.php";    
      }

    if ($_SESSION['auth'] !== 'AUTHORISED') {
        echo '<button id="n/a" style = "margin-right:90px; width:150px; color:white; background-color:red; border-color:transparent; border-radius:8px; height:40px; font-size:16px;"><b>nicht angemeldet</b></button>';
        return;
    }

    $mqtt_script = shell_exec("pgrep fmn-mqtt.pl");
    if (!strlen($mqtt_script)) {
      $debug = "";
      if ((defined ('debug')) && (debug > 0)) $debug = "v=" . debug . " ";
      $cmd = DL3EL_BASE . "svx2mqtt/fmn-mqtt.pl " . $debug . " >/dev/null &";
      echo "Starting MQTT " . $cmd . "<br>";
      exec($cmd, $output, $retval);
      echo "$output $retval<br>";
      $logtext =  "MQTT Dienst gestartet " . $cmd . "\n";
      addsvxlog($logtext);
    }
if (defined ('SVXMQTT_COLOR_active')) {
    $svxmqtt_color_active = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_active . ";color:white;font-weight:bold;font-size:14px;";
  } else {
    $svxmqtt_color_active = "green";
  }

  if (defined ('SVXMQTT_COLOR_passive')) {
    $svxmqtt_color_passive = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_passive . ";color:white;font-weight:bold;font-size:14px;";
  } else {
    $svxmqtt_color_passive = "blue";
  }

  if (defined ('SVXMQTT_COLOR_friends')) {
    $svxmqtt_color_friends = "style=cursor:pointer;border:none;border-radius:8px;width:85px;background-color:" . SVXMQTT_COLOR_friends . ";color:white;font-weight:bold;font-size:14px;";
  } else {
    $svxmqtt_color_friends = "green";
  }

// file with mqtt data
    $file = DL3EL_BASE . "svx2mqtt/mqtt.data";
    $content = file_get_contents($file);
    $zeilen_array = explode("\n", $content);
    $anzahl_zeilen = count($zeilen_array); 
    $umgekehrte_zeilen = array_reverse($zeilen_array);
    $content = implode("\n", $umgekehrte_zeilen);
    $mqtt_start_ts = 0;
    if ((defined ('debug')) && (debug > 0)) echo "<tr><td><b>MQTT RX</b></td></tr>";
    $nn = 0;
    if ((defined ('debug')) && (debug > 0)) echo "$nn  $anzahl_zeilen ($file)<br>";
    if ($anzahl_zeilen > 3000) {
      $nn = $anzahl_zeilen - 3000;
    } else {
      $nn = 0;
    }  

    if (defined ('DL3EL_FRIENDS')) {
      $friend_call_array = explode(",", DL3EL_FRIENDS);
    } else {
      $friend_call_array[0] = 0;
    }    

    if ((defined ('debug')) && (debug > 0)) echo "<tr><td><b>MQTT RX</b></td></tr>";
    if ($anzahl_zeilen > 3000) {
      $nn = $anzahl_zeilen - 3000;
    } else {
      $nn = 0;
    }  
//valid records
//[19.02.2026 12:39:24] /server/mqtt/heartbeat: 324973 seconds^1771628950^
//[19.02.2026 12:39:22] /server/statethr/1: {"time":"12:39:21", "talk":"stop", "call":"DB0BLO", "tg":"13055", "server":"1"}^1771628950^
//[21.02.2026 00:09:10] /server/state/loginz: 507^1771628950^
//[20.02.2026 17:38:28] DL3EL: MQTT Start^1771605508^
//[21.02.2026 08:08:15] /server/mqtt/parrot/9990/analysis: {"tg": 9990, "callsign": "DO3FHS-APP", "timestamp": 1771535167.418658, "summary": {"avg_rms_db": -28.4, "max_peak_db": 0.0, "min_rms_db": -49.3, "max_rms_db": -2.1, "dynamics_db": 47.2, "clip_percent": 12.6, "silence_percent": 2.9, "total_frames": 579, "active_frames": 562, "duration_s": 11.6}, "recommendation": {"rating": 3, "rating_text": "\ud83d\udfe0 Grenzwertig", "level_status": "borderline", "message": "Clipping etwas hoch (13%).", "action": "Optional: TX-Pegel leicht reduzieren", "avg_rms_db": -28.4, "max_peak_db": 0.0, "clip_percent": 12.6, "meter_position": 72.0}}^1771657695^
//[21.02.2026 08:08:15] /server/mqtt/parrot/9990/status: {"tg": 9990, "callsign": "", "status": "online", "timestamp": 1771558480.0964217}^1771657695^

    While ($nn < $anzahl_zeilen) {
      $line = $zeilen_array[$nn];
      if (strlen($line)) {
//echo "$line <br>";
        if (str_contains($line,"server/statethr/1")) {
//get timestamp
          $timestamp_start = strpos($line, '^');
          if ($timestamp_start !== false) {
            $timestamp = substr($line, $timestamp_start);
            $timestamp = trim($timestamp,"\^");
          }  
          // 1. Den JSON-Teil sauber ausschneiden (ab der Position nach dem Doppelpunkt)
          // Wir suchen die erste "{" Klammer
          $raw_line = $line;
          $json_start = strpos($raw_line, '{');
          if ($json_start !== false) {
            $json_end = strpos($raw_line, '}')+1;
            $json_string = substr($raw_line, $json_start,$json_end-$json_start);
            $json_string = trim($json_string);
          
            // 2. Den String in ein PHP-Array umwandeln
            $line_e = json_decode($json_string, true);
            if ($line_e !== null && json_last_error() === JSON_ERROR_NONE) {
              $time = $line_e['time'];
              $talk_state = $line_e['talk'];
              $call = $line_e['call'];
              $tg = $line_e['tg'];
              if ((defined ('debug')) && (debug > 0)) echo "<tr><td>$time / $call / $tg / $talk_state / $timestamp</td></tr>";
              $array[$call]["call"] = $call;
              $array[$call]["talk_state"] = $talk_state;
              if ($talk_state === "start") {
                $array[$call]["time_1"] = $time;
                $array[$call]["time_2"] = time()- $timestamp . "s";
                $array[$call]["dauer"] = time() - $timestamp;
                $array[$call]["dauera"] = time() - $timestamp;
                $array[$call]["dauerp"] = 0;
                $array[$call]["talker"] = "1";
                $array[$call]["ts"] = $timestamp;
                $array[$call]["ts-start"] = $timestamp;
                $array[$call]["ts-stop"] = 0;
              } else {
                if (isset($array[$call]["time_1"])) {
                  $array[$call]["time_2"] = $time;
                  $array[$call]["talker"] = "2";
                  $array[$call]["dauer"] = 0;
                  $array[$call]["dauera"] = 0;
                  $ts_start = $array[$call]["ts-start"] ?? 0; 
                  $time_1_start = $array[$call]["time_1"] ?? 0;
// komplettes qso, $call ändern um doppelte zu verhindern
                  $call = $call . "^" . $ts_start;
                  $array[$call]["time_1"] = $time_1_start;
                  $array[$call]["time_2"] = $time;
//                  $array[$call]["time_2"] = date("H:i:s",$ts_start);
                  $array[$call]["talker"] = "0";
                  $tgnumber = $tg;
                  $name=trim(isset($tgdb_array[$tgnumber])? $tgdb_array[$tgnumber] : '---');;
                  $array[$call]["tgn"] = $name;
                  $array[$call]["ts"] = $timestamp;
                  $array[$call]["ts-start"] = $ts_start;
                  $array[$call]["ts-stop"] = $timestamp;
                  $array[$call]["dauer"] = 0;
                  $array[$call]["dauerp"] = $array[$call]["ts-stop"] - $array[$call]["ts-start"];
                } else {
                    $array[$call]["talker"] = "3";
                    $array[$call]["time_1"] = 0;
                    $array[$call]["time_2"] = 0;
                    $array[$call]["ts"] = 0;
                    $array[$call]["dauer"] = 0;
                    $array[$call]["dauera"] = 0;
                    $array[$call]["dauerp"] = 0;
//                    unset($array[$call]);
                }    
              }  
              $tgnumber = $tg;
              $name=trim(isset($tgdb_array[$tgnumber])? $tgdb_array[$tgnumber] : '---');;
              $array[$call]["tg"] = $tg;
//              $array[$call]["tg"] = sprintf("<button type=submit id=jumptoA name=jmptoA class=active_id value=\"%s\">%s</button>", $tg,$tg);
              if ($array[$call]["talker"]) {
                $svxmqtt_color = $svxmqtt_color_active;
              } else {
                $svxmqtt_color = $svxmqtt_color_passive;
              }  
              $array[$call]["tg"] = sprintf("&nbsp;<button style=cursor:pointer;border-radius:8px;width:85px;%s;font-weight:bold;font-size:14px; type=submit id=jumptoA name=jmptoA class=active_id value=\"%s\">%s</button>",$svxmqtt_color, $tg,$tg);
              $array[$call]["tgn"] = $name;
              $array[$call]["ts"] = $timestamp;
            } else {
              echo "<tr><td>JSON $json_string JSON-Fehler: " . json_last_error_msg() . " im String: $json_string</td></tr>";
            }
          } else{
            echo "<tr><td>Keine JSON-Daten gefunden ($line).</td></tr>";
          }  
        } else {
            if (str_contains($line,"/server/mqtt/heartbeat")) {
              if ((defined ('debug')) && (debug > 0)) echo "<tr><td>$line</td></tr>";
            }
            if (str_contains($line,"/server/state/loginz: ")) {
              if ((defined ('debug')) && (debug > 1)) echo "<tr><td>$line</td></tr>";
              $loginz_start = strpos($line, ': ')+2;
              $loginz_end = strpos($line, '^');
              $loginz = substr($line, $loginz_start,$loginz_end-$loginz_start);
            }  

            if (str_contains($line,"DL3EL: MQTT Start")) {
              if ((defined ('debug')) && (debug > 1)) echo "<tr><td>$line</td></tr>";
              $mqtt_start = strpos($line, 't^')+2;
              $mqtt_start_ts = substr($line, $mqtt_start);
              $mqtt_start_ts = trim($mqtt_start_ts,"\^");
            }  

        }   
      }  
      ++$nn;
    }

    if ((defined ('debug')) && (debug > 0)) echo "<tr><td><b>MQTT Array</b></td></tr>";
    $nn = 0;
    foreach ($array as $call => $row) {
      foreach ($row as $key => $value) {
        if ((defined ('debug')) && (debug > 0)) echo "Value at position key=$key and Call=$call: $value<br>";
        if ($key === "time_1") $time_1 = $value;
        if ($key === "time_2") $time_2 = $value;
        if ($key === "talk_state") $talk_state = $value;
        if ($key === "tg") $tg = $value;
        if ($key === "tgn") $tgn = $value;
        if ($key === "ts") $ts = $value;
        if ($key === "talker") $act = $value;
        if ($key === "dauer") $dauer = $value;
        if ($key === "dauera") $dauera = $value;
        if ($key === "dauerp") $dauerp = $value;
        if ($key === "ts-start") $ts_start = $value;
      }

      $timestamp_start = strpos($call, '^');
      if ($timestamp_start !== false) {
        if ((defined ('debug')) && (debug > 0)) {
          // do not delete timestamp for debug
        } else {  
          $call = substr($call, 0, $timestamp_start);
        }  
      }  

      $color_friend = "";
      foreach ($friend_call_array as $fcall) {
// look for friends
        trim($fcall);
// add " " to the call for exact match
        $fcall = $fcall . " ";
        $ccall = substr($call,0,strlen($fcall)-1) . " ";
        if ($ccall === $fcall) {
          $color_friend =  $svxmqtt_color_friends;
          if ((defined ('debug')) && (debug > 0)) echo "Friends check: $ccall === $fcall <br>";
          break;
        }
      }
      if ($time_1) {
        if ((defined ('debug')) && (debug > 0)) {
          $debug = $tg . "/" . $ts_start . "/" . $mqtt_start_ts;
          $line = $act . "<tr style='height:24px;'><td>" . $call . "&nbsp;</td><td>" .  $debug . "&nbsp;</td><td>"  .  $tgn . "&nbsp;</td><td>" .  $ts_start . "&nbsp;</td><td>" .  $time_2 . "&nbsp;[DA:" . $dauera . ", DP:" . $dauerp . "] </td></tr>";
        } else {
//          $line = $act . "<tr style='height:24px;'><td>" . $call . "&nbsp;</td><td>" .  $tg . "&nbsp;</td><td>"  .  $tgn . "&nbsp;</td><td>" .  $time_1 . "&nbsp;</td><td>" .  $time_2 . "&nbsp;</td></tr>";
          $line = $act . "<tr style='height:24px;" . $color_friend . "'><td>" . $call . "&nbsp;</td><td>" .  $tg . "&nbsp;</td><td>"  .  $tgn . "&nbsp;</td><td>" .  $time_1 . "&nbsp;</td><td>" .  $time_2 . "&nbsp;</td></tr>";
        }    
        if (($act) && ($ts_start < $mqtt_start_ts)) {
            if ((defined ('debug')) && (debug > 0)) echo "unterdrückt (stop fehlt): $line</td></tr>";
        } else {    
          $array_time[$ts_start] = $line;
        }  
        if ((defined ('debug')) && (debug > 0)) echo "<tr><td>$line</td></tr>";
        ++$nn;
      }    
    }
    krsort($array_time);
	
echo '<span style = "font-size:25px"> </span>';
    echo '<span style = "font-weight:bold; font-size:15px;"><a href="https://dashboard.fm-funknetz.de/" target="fmn">FM-Funknetz Live Activity</a></span>';
    if ((defined ('debug')) && (debug > 0)) echo "Ich bin gerade hier: " . getcwd();

    echo '<form method="post">';
    echo '<table style = "width: 500px; text-align: left; margin-top: 5px; margin-bottom:0px;border:0; border-collapse:collapse; cellspacing:0; cellpadding:0; background-color:#f1f1f1;"><tr style = "border:none;background-color:#f1f1f1;">';
    echo '<thead>';
    echo '<tr style="height:24px;">';
    echo '<th style="width:22%;">&nbsp;&nbsp;Call</th>';
    echo '<th style="width:12%">TG</th>';
    echo '<th style="width:40%">TG Name&nbsp;&nbsp;(Logins:' . $loginz . ')</th>';
    echo '<th style="width:12%">von</th>';
    echo '<th style="width:13%">bis/Dauer&nbsp;</th>';
    echo '</tr>';
    echo '</thead>';

    foreach ($array_time as $line) {
// show talkers
      if (substr($line,0,1) === "1") { 
        echo substr($line,1);
      }  
    }
    echo "<tr> <td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td></tr>";
    echo "<tr> <td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;zuletzt aktiv</td><td>&nbsp;</td><td>&nbsp;</td></tr>";
    $nn = 0;
    foreach ($array_time as $line) {
      if (substr($line,0,1) === "0") { 
        echo substr($line,1);
        ++$nn;
      }  
      if ($nn > 30) break;
  }
?>
</table></form>
