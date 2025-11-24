<html>
<?php
include_once ('include/config.php');
?>

<head>
<link rel="shortcut icon" href="images/favicon.ico" sizes="16x16 32x32" type="image/png">
<?php echo ("<title>" . $callsign . " Dashboard " . $dbversion . "</title>" ); ?>
<body style="background-color:#e1e1e1;">
</head>

<style>
.container {
  display: flex;
  width: 1920px;
  height: 1200px;
}
.left-half {
  width: 950px;
}
.right-half {
  width: 700px;
}
</style>

<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

	$mobile = 1;
  if ((defined('DL3EL_LIVEDB_AUTO')) && (DL3EL_LIVEDB_AUTO === "yes")) {
    include_once "include/browserdetect.php";
//	$mobile = 1 = mobile;
  }
  if ((defined ('debug')) && (debug > 0)) echo "<br>Browser: $mobile (0=Desktop, 1=MOBIL)<br>";
  if (!$mobile) {
    echo '<body>';
    echo '<div class="container">';

    echo '<div class="left-half">';
    echo '<object style="outline:none; height:1200px; width:950px;" data=index1.php></object>';
    echo '</div>';

    echo '<div class="right-half">';
    echo '<object style="outline:none; height:1200px; width:700px" data=svx2mqtt/index_duo.php></object>';
    echo '</div>';

    echo '</div>';

    echo '</body>';

  } else {
    if ((defined ('debug')) && (debug > 0)) echo "<br>Browser: $mobile<br>";
    header("Location: http:index1.php");
    exit;
  }

?>
</html>
