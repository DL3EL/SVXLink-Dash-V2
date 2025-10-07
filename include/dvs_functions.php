<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "config.php";         
include_once "tools.php";        
//include_once __DIR__.'/functions.php';
//require_once __DIR__.'/include/config.php';
//function open_for_edit($filename) {
//        if (fopen($filename,'r')){
//                $file_conf = parse_ini_file($filename, true, INI_SCANNER_RAW);
//        }
//        $parts = explode(",", $file_conf['A']['B']);
//}

function getYSFHosts() {
	// Open Logfile and copy loglines into LogLines-Array()
	$ysfLines = array();
	$ysfLines1 = array();
//	if (file_exists(SVXLOGPATH.SVXLOGPREFIX)) {
	if (file_exists("/var/lib/mmdvm/YSFHosts.txt")) {
		$ysfFile = "/var/lib/mmdvm/YSFHosts.txt";

      // Read the entire log file into an array of lines
      //$lines = file($ysfFile);

      // Calculate the number of lines in the log file
      //$numLines = count($lines);
      //echo "YSFHosts.txt: $numLines<br>";

//			$ysfLines = explode("\n", `cat $ysfFile |egrep -a -h ";DE-" `);
//			$ysfLines = explode("\n", $lines);
			$ysfLines = explode("\n", `cat $ysfFile `);
      $de = 0;
      foreach ($ysfLines as $ysfLine) {
        if (strlen($ysfLine)) {
          $lineParts = explode(";", $ysfLine);
          $ysfLine = $lineParts[1] . ";". $lineParts[0] . ";". $lineParts[2] . ";". $lineParts[3] . ";". $lineParts[4] . ";". $lineParts[5] . ";" . $de . ";" . substr($lineParts[1],0,2) . ";" ;
//          echo $ysfLine . "<br>";
          $ysfLines1[$de] = $ysfLine;
          ++$de;
        }
      }
//      echo "YSFHosts.txt: $de<br>";
      asort($ysfLines1);
      $de = 0;

//      echo '<table>';
      $cntry = 0;
      $cntry_list = array();
      $cntry_list[$cntry] = "DE";
      foreach ($ysfLines1 as $ysfLine) {
        $lineParts = explode(";", $ysfLine);
        $ysfLine = "<td>" . $lineParts[0] . "</td><td>". $lineParts[1] . "</td><td>". $lineParts[2] . "</td><td>" . $lineParts[3] . "</td><td>" . $lineParts[4] . "</td><td>" . $lineParts[5] . "</td><td>" . $lineParts[6] . "</td><td>" . $lineParts[7] . "</td><td>" . $cntry . "</td>";
        if ($cntry_list[$cntry] !== $lineParts[7]) {
          ++$cntry;
          $cntry_list[$cntry] = $lineParts[7];
        }  
        ++$de;
//        echo $ysfLine . "<td>($cntry_list[$cntry]/$cntry)</td><tr>";
      }
//      echo '</td></tr></table>';
//      echo "YSFHosts.txt DE: $de, Countries: $cntry<br>";

// Country List
      $cn = 0;
      echo '<form method="post" action="">';
      echo '<select id="ysfcountry" name="ysfcountry" required>';
      foreach ($cntry_list as $ysfCountry) {
        $ysfLine = "<option value=" . $cn . ">" . $ysfCountry . "</option><br>\n";
        echo $ysfLine;
        ++$cn;
      }
      echo "</select>";
      echo '<button type="submit">Select YSF Country</button>';
      echo '<input type="hidden" name="country_form_submitted" value="1">';
      echo "</form>&nbsp;&nbsp;";

      if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['country_form_submitted'])) {
        if (isset($_POST['ysfcountry'])) {
          $ysfptr = $_POST['ysfcountry'];
          $ysfcntry = $cntry_list[$ysfptr];
// Room List
          $de = 0;
          echo '<form method="post" action="">';
//      echo '<label for="ysfroom">Select new YSF room</label>&nbsp;&nbsp;';
          echo '<select id="ysfroom" name="ysfroom" required>';
          foreach ($ysfLines1 as $ysfLine) {
            $lineParts = explode(";", $ysfLine);
            if ($ysfcntry == $lineParts[7]) {
              $ysfLine = "<option value=" . $lineParts[6] . ">" . $lineParts[0] . "(" . $lineParts[2] . "/" . $lineParts[1]. ")[$de]" . $lineParts[7] . "</option><br>\n";
              echo $ysfLine;
            }  
            ++$de;
          }
          echo "</select>";
          echo '<button type="submit">Connect to YSF Room</button>';
          echo '<input type="hidden" name="ysf_form_submitted" value="1">';
          echo "</form>";
        }
      }

      if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['ysf_form_submitted'])) {
        if (isset($_POST['ysfroom'])) {
          $ysfptr = $_POST['ysfroom'];
          $ysfroom = $ysfLines1[$ysfptr];
//          echo "new Room (index = $ysfptr): $ysfroom / max rooms: $de  <br>";
          $lineParts = explode(";", $ysfroom);
          $ysfLine = $lineParts[3] . ":". $lineParts[4];
//          echo "$ysfLine <br>";
          $command = "/opt/MMDVM_Bridge/dvswitch.sh tune " . $ysfLine . " 2>&1";
//          echo "$command <br>";
          exec($command,$screen,$retval);
        }
      }
	}
	return $logLines;
}
    ?>
