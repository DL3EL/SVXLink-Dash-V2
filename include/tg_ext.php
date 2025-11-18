
<?php

include_once DL3EL_BASE . "include/tgdb.php"; 
  if ((defined ('debug')) && (debug > 0)) echo "TG: " .$tgdb_array[262649] . "<br>";
  $tgdb_array[262649] = "OV Welle F49 Frankfurt-West";
  if ((defined ('debug')) && (debug > 0)) echo "TG: ($tg)" .$tgdb_array[262649] . "<br>";

  $nn = 0;
  $content = "<?php\n
/* talkgroup / number alias database */\n
/* alle Angabennohne Gewähr / FM-Funknetz.de*/\n
\$tgdb_array = [\n";

  foreach ($tgdb_array as $tg => $tgname) {
    if ((defined ('debug')) && (debug > 0)) echo "($nn) [$tg] [$tgname] <br>";
    $content = $content . "'" . $tg . "' => '" . $tgdb_array[$tg] . "',\n";
    ++$nn;
  }  
  $content = $content . "    '0'			=> ' ;-) ',
    ''			=> 'Hier könnte ihre Werbung stehen<br> oder einfach ihre Start <b>TG</b>  &#128580; &#128580;  '
];

/* Make my Day better, vy73 Jens, DJ1JAY */
?>\n";
file_put_contents(DL3EL_BASE . "include/tgdb.php",$content);
//?>
