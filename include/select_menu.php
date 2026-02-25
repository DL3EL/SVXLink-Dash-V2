<?php
  $menuFile = DL3EL . "/menu";
  if ((defined ('debug')) && (debug > 0)) echo "Ich bin gerade hier: " . getcwd();
      
  if (file_exists($menuFile)) {
    $menuType = trim(file_get_contents($menuFile));
  } else {
    if ((defined('MENU_TYPE')) && ((MENU_TYPE === "dropdown") || (MENU_TYPE === "classic"))) {
      $menuType = MENU_TYPE;
    } else {
      $menuType = "classic";
    }
  }

      if (isset($_POST['btn_dropdown'])) {
        $menuType = "dropdown";
        file_put_contents($menuFile, $menuType);
      }
      if (isset($_POST['btn_classic'])) {
        $menuType = "classic";
        file_put_contents($menuFile, $menuType);
      }
      if ($menuType == "dropdown") {
        $menu_dropdown = "<b>dropdown</b>";
        $menu_classic = "classic";
      } else {
        $menu_dropdown = "dropdown";
        $menu_classic = "<b>classic</b>";
      }
?>
    
