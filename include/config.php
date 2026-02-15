<?php
    include_once __DIR__."/basedir.php"; 
// do not report any php errors in apache log
error_reporting(-1);
define ("debug", "0");
// Button keys define for dashboard: description button, DTMF command or command, color of button
//
// syntax: 'KEY number,'Description','DTMF code','color button'.
//
define("KEY1", array(' F49 ','*91262649#','green'));
define("KEY2", array(' 26269 ','*9126269#','green'));
define("KEY3", array(' 777 ','*91777#','green'));
define("KEY4", array(' 26447 ','*9126447#','green'));
//define("KEY4", array(' 7300 ','*9#*817300#','green'));
define("KEY5", array(' AutoQSY ','*92#','red'));
define("KEY6", array(' AutoQSY F49 ','*92262649#','orange'));
define("KEY7", array(' Refl1 letzte TG ','*91#','purple'));
define("KEY8", array(' Refl1 TG0 ','*910#','red'));
define("KEY9", array(' Status ','*9*#','green'));
define("KEY10", array(' Parrot','1#','green'));
define("KEY11", array(' Metar EDDF ','5#','blue','auth'));
define("KEY12", array(' Echolink','*2#','orange'));
define("KEY13", array(' EL DB0HTV', '*2#845239#','purple'));
define("KEY14", array(' EL trennen ','##','red'));
define("KEY15", array(' F49 49 ','*8149#','purple'));
define("KEY16", array(' Refl2 F49 trennen ','*8#','red'));
define("KEY17", array(' 60431 ','*9160431#','green'));
define("DTMF", "auth");
// things that normally should not be changed
// Path and file name of confguration
define("SVXCONFPATH", "/etc/svxlink/");
define("SVXCONFIG", "svxlink.conf");
//
// Path and file name of log
define("SVXLOGPATH", "/var/log/");
define("SVXLOGPREFIX", "svxlink");
//
define("MODULEPATH", "/etc/svxlink/svxlink.d/");
define("ECHOLINKCONFIG", "ModuleEchoLink.conf");
define("METARINFO", "ModuleMetarInfo.conf");
define("TclVoiceMail", "no");
// Put url address to your svxreflector which offer information of status
//define("URLSVXRAPI", "http://192.168.1.33:9999/status");
define("URLSVXRAPI", "https://status.thueringen.link/"); 
//
// Orange Pi Zero LTS version requires CPU_TEMP_OFFSET value 30 
// to display CPU TEMPERATURE correctly
define("CPU_TEMP_OFFSET","0");
//
// Define where is located menu wit buttons TOP or BOTTOM
define("MENUBUTTON", "BOTTOM");
//
// Define if the complete Top Menu is shown without authorization or only the Links where information are provided
define("MENUTOP", "FULL");
//define("MENUTOP", "INFO");
// Define if the authorization Button is shown in the Top Menu without authorization or not (gives a bit more security)
//define("MENUTOPAUTH", "yes");
define("MENUTOPAUTH", "no");
//
// Set SHOWPTT to TRUE if you want use microphone connected
// to sound card and use buttons on dashboard PTT ON & PTT OFF
// Set SHOWPTT to FALSE to disable display PTT buttons
// In most cases you can switch to FALSE
//define("SHOWPTT","FALSE");
define("SHOWPTT","TRUE");
//
//define("DL3EL_BASE", "/var/www/html//");
//define("DL3EL", "/var/www/html/dl3el");
//define("DL3EL_EXPERT", "no");
define("DL3EL_EXPERT", "yes");
//define("DL3EL_NOAUTH", "no");
define("DL3EL_NOAUTH", "yes");
// ssh most likely will ot work on Android devices as clients
define("DL3EL_SSH", "no");
//define("DL3EL_SSH", "yes");
// must be a number, must not be a string
//define("DL3EL_SPEC_TG", 49);
//define("DL3EL_RADIO", "no Radio");
//define("DL3EL_RADIO", "145.600MHz");
define("DL3EL_RADIO", "SC (no Radio, 145.750MHz)");
//define("DL3EL_RADIO", "Shari");
//define("DL3EL_RADIO", "C710");
//define("DL3EL_RADIO", "RFGuru");
//define("DL3EL_DMRTG", "262649");
//define("DL3EL_VERSION", "public");
define("DL3EL_VERSION", "develop");
// show splitcsreen with live_db when running on a desktop, it will never be shown on a mobile device
//define("DL3EL_LIVEDB", "top");
//define("DL3EL_LIVEDB", "yes");
//define("DL3EL_LIVEDB", "no");
//define("DL3EL_LIVEDB", "right");
// good colours are green, silver and blue. There is no colour check :-)
//define ("SVXMQTT_COLOR_active", "#CECECE");
define ("SVXMQTT_COLOR_active", "green");
define ("SVXMQTT_COLOR_passive", "blue");
define("DL3EL_DXCLUSTER", "yes");
define("DL3EL_DXCLUSTER_CONT", "EU");
//define("DL3EL_EXTERNWEB_WIDTH", "1300");
//define("DL3EL_OPENWEBRX", "https://websdr.z-05.de/#freq=144800000,mod=empty,secondary_mod=packet,sql=-150");
define("DL3EL_OPENWEBRX", "http://192.168.241.67:8073/#freq=144800000,mod=empty,secondary_mod=packet,sql=-60");
//define("DL3EL_OPENHAMCLOCK", "no");
define("DL3EL_OPENHAMCLOCK_TAB", "new");
//define("DL3EL_OPENHAMCLOCK", "http://192.168.241.99:3030/");
define("DL3EL_OPENHAMCLOCK", "https://openhamclock.com/");
define("DL3EL_FMFUNKNETZ", "yes");
define("DL3EL_SVXCUBE", "http://192.168.241.58");
define("DL3EL_EXTERN1", "http://44.149.162.74/");
define("DL3EL_EXTERN1_NAME", "Z05 Ref");
define("DL3EL_EXTERN2", "http://192.68.17.19/");
define("DL3EL_EXTERN2_NAME", "F49 Ref");
define("DL3EL_EXTERN3", "https://webtrx.fm-funknetz.de/");
define("DL3EL_EXTERN3_NAME", "WebTRX");
define("DL3EL_EXTERN3_TAB", "new");
//define("DL3EL_SHOW_ONLY_ONE_IP", "yes");
//define("DL3EL_SHOW_ONLY_ONE_IP", "no");
// if you are unable to configure the system with *only* one soundcard for svxlink, the index to the card could be different after a reboot
// setting DL3EL_SC_CHANGEto "yes" enables the dashboard to select the correct soundcard and restart svxlink on the first run after reboot.
define("DL3EL_SC_CHANGE", "no");
// this string has to match the name of the card in the linux system
define("DL3EL_SC_Card_ID", "Device");
define("DL3EL_SC_STRING", "Audio Device");
// allow update "git pull" via this dashboard (can be found in the power menu, if enabled
// therefore for the command "git" had to be added in the file "www-data.sudoers", to activate this command, the script "upd_sudoer" has to be executed (as root)
// no updates un this development machine
//define("DL3EL_GIT_UPDATE", "nocheck");
define("DL3EL_DARK_MODE", "yes");
// currently not in config.php.example
// seconds to refresh the monitor call list. Use with care, min value is 5. No harm, if that does not exist.
define("DL3EL_MONREFRESH", "5");
//
define("DL3EL_APRS_MSG", "yes");
//
define("DL3EL_SHOW_NEXT_RUN", "no");
define("DL3EL_CRON_TIMER", 86400);
//define("DL3EL_CRON_TIMER", 10);
define("DL3EL_REF1_BUTTON", "FMN DL");
define("DL3EL_REF2_BUTTON", "HAMNET FMN");
define("DL3EL_REF3_BUTTON", "FMN 262649");
define("DL3EL_REF4_BUTTON", "HAMNET Z05");
// show detail to call from FM-Funknetz instead of qrz.com
define("DL3EL_LH_DETAIL", "FMN");
//
define("DL3EL_MASTER_IP", "44.149.160.87/32, 44.149.162.105/32");
//
include_once DL3EL_BASE ."include/config.inc.php";
?>

