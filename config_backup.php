<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}
include_once "include/settings.php";
include_once "include/functions.php";

?>

	<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
	      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
    <html lang="en">
	<head>
	    <meta name="language" content="English" />
	    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
	    <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate" />
	    <meta http-equiv="pragma" content="no-cache" />
	    <link rel="shortcut icon" href="images/favicon.ico" type="image/x-icon" />
	    <meta http-equiv="Expires" content="0" />
	    <!-- <link rel="stylesheet" type="text/css" href="css/font-awesome-4.7.0/css/font-awesome.min.css" />
		<link href="css/featherlight.css" type="text/css" rel="stylesheet" />
		<script src="scripts/featherlight.js" type="text/javascript" charset="utf-8"></script> -->
	</head>

<?php
  print "    <link rel=\"stylesheet\" type=\"text/css\" media=\"screen and (min-width: 830px)\" href=\"css/css.php\" />\n";
?>


<body style = "background-color: #e1e1e1;font: 11pt arial, sans-serif;">
<center>
<fieldset style = "box-shadow:5px 5px 20px #999; background-color:#f1f1f1; width:0px;margin-top:15px;margin-left:0px;margin-right:5px;font-size:13px;border-top-left-radius: 10px; border-top-right-radius: 10px;border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;">
<div class="container"> 
	<div class="header">
		<div class="parent">
			
		<div class="img" style = "padding-left:30px"><img src="images/svxlink.ico" /> </div>
<center>
<span style = "font-size: 32px;letter-spacing:4px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $callsign; ?></span>
<p style = "margin-top:0px;margin-bottom:0px;">
<span style = "font-size: 18px;letter-spacing:4px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $fmnetwork; ?></span>
<span style = "font-size: 12px;letter-spacing:2px;font-family: &quot;sans-serif&quot;, sans-serif;font-weight:500;color:PaleBlue"><?php echo $dbversion; ?></span>

</div>
	</div>
		</div>
		
<?php // include_once "include/top_menu.php"; ?>
<div id="display-links" align=right>
	<p style = "padding-right: 5px; text-align: right; color: #000000;">
	<a style = "color: black;">Display</a> |
	<a href="./index.php" style = "color: #000000;">Dashboard</a> | 
	<a href="./node.php" style = "color: #000000;">Nodes</a> | 
	<a href="./echolink_dl3el.php" style = "color: #0000ff;">Echolink</a> | 
	<a href="./tg.php" style = "color: #000000;">Talk Groups</a> | 
	<a href="./relais.php" style = "color: #0000ff;">FM Relais</a> | 
	<a href="./config_backup.php" style = "color: #0000ff;">Backup/Restore</a> | 

		<center>
		<h2><?php echo ('Backup/Restore');?></h2>
		    <?php if (!empty($_POST)) {
			echo '<table width="100%">'."\n";
			addlog("L","Start Backup\n");

			if ( escapeshellcmd($_POST["action"]) == "download" ) {
			    $backupDir = "/tmp/config_backup";
			    $backupZip = "/tmp/config_backup.zip";
			    $hostNameInfo = exec('cat /etc/hostname');

/*
			    exec("sudo rm -rf $backupZip > /dev/null");
			    exec("sudo rm -rf $backupDir > /dev/null");
			    exec("sudo mkdir $backupDir > /dev/null");
			    exec("sudo cp -p -r /etc/NetworkManager/system-connections/*.nmconnection $backupDir > /dev/null");
*/
				$command = "sudo rm -rf $backupZip > /dev/null";
				exec($command,$screen,$retval);
				$command = $command . " RV:" . $retval;
				addlog("L","$command\n");
				$command = "sudo rm -rf $backupDir > /dev/null";
				exec($command,$screen,$retval);
				$command = $command . " RV:" . $retval;
				addlog("L","$command\n");
				$command = "sudo mkdir $backupDir > /dev/null";
				exec($command,$screen,$retval);
				$command = $command . " RV:" . $retval;
				addlog("L","$command\n");
				$command = "sudo cp -p -r /etc/NetworkManager/system-connections/*.nmconnection $backupDir > /dev/null";
				exec($command,$screen,$retval);
				$command = $command . " RV:" . $retval;
				addlog("L","$command\n");
/*
define("SVXCONFPATH", "/etc/svxlink/");
define("MODULEPATH", "/etc/svxlink/svxlink.d/");
*/
//			    exec("sudo cp -p -r /etc/svxlink/* $backupDir > /dev/null");
				$command = "sudo cp -p -r " . SVXCONFPATH . "* $backupDir > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

//			    exec("sudo cp -p -r /etc/svxlink/svxlink.d/* $backupDir > /dev/null");
				$command = "sudo cp -p -r " . MODULEPATH . "* $backupDir > /dev/null";
//				exec($command,$screen,$retval);
//				addlog("L","$command\n");

			    //exec("sudo cp -r /usr/share/svxlink/* $backupDir > /dev/null");
			    exec("sudo cp -p -r /usr/share/svxlink/events.d/local/* $backupDir > /dev/null");

//			    exec("sudo cp -p -r /var/www/html/include/config.php $backupDir > /dev/null");
				$command = "sudo cp -p " . DL3EL_BASE . "include/config.php $backupDir > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

				$command = "sudo cp -p " . DL3EL . "/*.conf $backupDir > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

			    chdir($backupDir);
			    exec("sudo zip -r $backupZip * > /dev/null");

			    if (file_exists($backupZip)) {
					$utc_time = gmdate('Y-m-d H:i:s');
					$utc_tz =  new DateTimeZone('UTC');
					$local_tz = new DateTimeZone(date_default_timezone_get ());
					$dt = new DateTime($utc_time, $utc_tz);
					$dt->setTimeZone($local_tz);
                		$local_time = $dt->format('Y-M-d');
					header('Content-Type: application/zip');
						if ($hostNameInfo != "pi-star") {
				    		header('Content-Disposition: attachment; filename="'.basename("SVXLINK_Config_".$hostNameInfo."_".$local_time.".zip").'"');
						}
						else {
				    		header('Content-Disposition: attachment; filename="'.basename("SVXLINK_Config_$local_time.zip").'"');
						}
					header('Content-Length: ' . filesize($backupZip));
					ob_clean();
					flush();
					readfile($backupZip);
					exit();
			    }

			};
			if ( escapeshellcmd($_POST["action"]) == "restore" ) {
			    echo "<tr><th colspan=\"2\">Configuration Restore</th></tr>\n";

			    $target_dir = "/tmp/config_restore/";
			    exec("sudo rm -rf $target_dir > /dev/null");
			    exec("mkdir $target_dir > /dev/null");
			    if($_FILES["fileToUpload"]["name"]) {
				$filename = $_FILES["fileToUpload"]["name"];
	  			$source = $_FILES["fileToUpload"]["tmp_name"];
				$type = $_FILES["fileToUpload"]["type"];

				$name = explode(".", $filename);
				$accepted_types = array('application/zip', 'application/x-zip-compressed', 'multipart/x-zip', 'application/x-compressed');
				foreach($accepted_types as $mime_type) {
				    if($mime_type == $type) {
					$okay = true;
					break;
				    }
				}
			    }
			    $continue = false;

			    if (isset($name))
			    {
				$continue = strtolower($name[1]) == 'zip' ? true : false;
			    }

			    if(!$continue) {
				$output .= "The file you are trying to upload is not a .zip file. Please try again.\n";
			    }

			    if (isset($filename))
			    {
				$target_path = $target_dir.$filename;
			    }

			    if(isset($target_path) && move_uploaded_file($source, $target_path)) {
				$zip = new ZipArchive();
				$x = $zip->open($target_path);
				if ($x === true) {
			            $zip->extractTo($target_dir); // change this to the correct site path
			            $zip->close();
			            unlink($target_path);
				}

				// Stop SVXLink
			    	exec('sudo systemctl stop svxlink > /dev/null');

				// Overwrite the configs
				//exec("sudo mv -f /tmp/config_backup/* /tmp/config_restore/ > /dev/null");
				exec("sudo rm -rf /tmp/config_restore/tmp > /dev/null");
/*
define("SVXCONFPATH", "/etc/svxlink/");
define("MODULEPATH", "/etc/svxlink/svxlink.d/");
*/
//		        exec("sudo mv -f /tmp/config_restore/Module* /etc/svxlink/svxlink.d/ > /dev/null");
				$command = "sudo mv -f /tmp/config_restore/Module* " . MODULEPATH . " > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

				//exec("sudo mv -f /tmp/config_restore/ca-hook.py /usr/share/svxlink/ > /dev/null");
				exec("sudo mv -f /tmp/config_restore/events.tcl /usr/share/svxlink/ > /dev/null");
//				exec("sudo mv -f /tmp/config_restore/node_info.json /etc/svxlink/ > /dev/null");
				$command = "sudo mv -f /tmp/config_restore/node_info.json  " . SVXCONFPATH . " > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

//				exec("sudo mv -f /tmp/config_restore/svxlink.conf /etc/svxlink/ > /dev/null");
				$command = "sudo mv -f /tmp/config_restore/svxlink.conf " . SVXCONFPATH . " > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

//				exec("sudo mv -f /tmp/config_restore/config.php /var/www/html/include/ > /dev/null");
				$command = "sudo mv -f /tmp/config_restore/config.php " . DL3EL_BASE . "include/ > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

				$command = "sudo mv -f /tmp/config_restore/Reflector*.conf " . DL3EL . "/ > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

				$command = "sudo mv -f /tmp/config_restore/aprs-is.conf " . DL3EL . "/ > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

				$command = "sudo mv -f /tmp/config_restore/aprs-is-msg.conf " . DL3EL . "/ > /dev/null";
				exec($command,$screen,$retval);
				addlog("L","$command\n");

				// Start the services
			    	exec('sudo systemctl restart svxlink > /dev/null &');

				// Complete
				$output .= "<h3 style='text-align:center'>Configuration Restoration Complete.</h3>\n";
				$output .= "<p style='text-align:center'><br><a href='/'>Go to Dashboard...</a></p>\n";
			    }
			    else {
				$output .= "There was a problem with the upload. Please try again.<br />";
				$output .= "\n".'<button onclick="goBack()">Go Back</button><br />'."\n";
				$output .= '<script>'."\n";
				$output .= 'function goBack() {'."\n";
				$output .= '    window.history.back();'."\n";
				$output .= '}'."\n";
				$output .= '</script>'."\n";
			    }
			    echo "<tr><td>$output</td></tr>\n";
			};

			echo "</table>\n";
		    } else { ?>
			<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post" enctype="multipart/form-data">
			    <table width="100%">
				<tr>
				    <td align="center" valign="top" width="50%"><h3>Download Configuration</h3><br />
					<button style="border: none; background: none; margin: 15px 0px;" name="action" value="download"><img src="images/download.jpg" border="0" alt="Download Config" /></button>
				    </td>
				    <td align="center" valign="top"><h3>Restore Configuration</h3><br />
					<button style="border: none; background: none; margin: 10px 0px;" name="action" value="restore"><img src="images/upload.jpg" border="0" alt="Restore Config" /></button><br />
    					<input type="file" style="margin: 5px 0px;" name="fileToUpload" id="fileToUpload" />
				    </td>
				</tr>
				<tr>
				    <td colspan="4" align="justify" style="padding: 8px;">
					<br>
					<p> This backup creates a zip-File that you can download with your browser. To restore please use this zip-File fur uploading.</p>
					<ul>
					    <li>System Passwords / Dashboard passwords are <strong>not</strong> backed up / restored.</li><br>
						<li>Following files are backuped:</li><br>
						<li>svxlink.conf and node_info.json</li><br>
						<li>all svxlink Modules</li><br>
						<li>Dashboard config.php</li><br>
					</ul>
				    </td>
				</tr>
			    </table>
			</form>
		    <?php } ?>
	</body>
    </html>
    
