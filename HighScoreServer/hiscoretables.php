<head>
<meta http-equiv="PRAGMA" content="NO-CACHE">
<meta http-equiv="EXPIRES" content="-1">

<?php

define("W1", 1);define("L1", 2);
define("W2", 3);define("L2", 4);
define("W3", 5);define("L3", 6);
define("W4", 7);define("L4", 8);

define("W1P", 11);define("L1P", 12);
define("W2P", 13);define("L2P", 14);
define("W3P", 15);define("L3P", 16);
define("W4P", 17);define("L4P", 18);

define("WT", 9);define("LT", 10);


ini_set ( "memory_limit", "40M");

$message="";

$failed=0;

error_reporting(E_ALL);





//**TODO: Need to add extension field to player table in database. If it exists, there is an image.

//If not, there is not an image.





// we first include the upload class, as we will need it here to deal with the uploaded file

//include('class.upload.php');







// everything was fine !

					//*****TODO: Add new columns to player table. What about duplication of regular and hardcore

					//databases for players table?****



					$db_host = 'localhost';

					$db_user = 'ddraig_admin';

					$db_pwd = 'yourpwd';



					$database = 'ddraig_sdhiscores';



					if (!mysql_connect($db_host, $db_user, $db_pwd))

						  die("Can't connect to database");



					if (!mysql_select_db($database))

						  die("Can't select database");







					$result = mysql_query("select playername from players");



					if (!mysql_num_rows($result)) {



						$message="Player name not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[strtolower($row['playername'])]=array($row['playername'],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);


								}



					}



	if($failed==1){



	}

mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {

								//print_r ($row);
								//echo "<br><br><br>";


									$assoc[$row['playername']][W1]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L1]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);


//-----------------------hardcore database


$database = 'ddraig_sdhiscoresh';
if (!mysql_select_db($database))
 die("Can't select hardcore database");

//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][W2]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L2]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);


//--------------------non-infighting database

$database = 'ddraig_sdhiscoresni';

if (!mysql_select_db($database))
	die("Can't select ni database");

//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][W3]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L3]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);


//--------------------non-infighting & hardcore database

$database = 'ddraig_sdhiscoresnih';

if (!mysql_select_db($database))
	die("Can't select nih database");



//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][W4]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L4]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
mysql_free_result($result);


//--PS

					$db_host = 'localhost';

					$db_user = 'ddraig_admin';

					$db_pwd = 'yourpwd';



					$database = 'ddraig_ps_sdhiscores';



					if (!mysql_connect($db_host, $db_user, $db_pwd))

						  die("Can't connect to database");



					if (!mysql_select_db($database))

						  die("Can't select database");




/*


					$result = mysql_query("select playername from players");



					if (!mysql_num_rows($result)) {



						$message="Player name not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[strtolower($row['playername'])]=array($row['playername'],0,0,0,0,0,0,0,0,0,0);


								}



					}



	if($failed==1){



	}

mysql_free_result($result);
*/

//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {

								//print_r ($row);
								//echo "<br><br><br>";


									$assoc[$row['playername']][W1P]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L1P]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);


//-----------------------hardcore database


$database = 'ddraig_ps_sdhiscoresh';
if (!mysql_select_db($database))
 die("Can't select hardcore database");

//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][W2P]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L2P]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);


//--------------------non-infighting database

$database = 'ddraig_ps_sdhiscoresni';

if (!mysql_select_db($database))
	die("Can't select ni database");

//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][W3P]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L3P]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);


//--------------------non-infighting & hardcore database

$database = 'ddraig_ps_sdhiscoresnih';

if (!mysql_select_db($database))
	die("Can't select nih database");



//_sdhiscores: GET all hi scores set from wad_main


					$result = mysql_query("select lower(playername) as playername, wadhiscore from wad_main WHERE wadhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][W4P]+=$row['wadhiscore'];
									$assoc[$row['playername']][WT]+=$row['wadhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);

//_sdhiscores: GET all hi scores set from wad_main_level


					$result = mysql_query("select lower(playername) as playername, levelhiscore from wad_main_level WHERE levelhiscore > 0 ORDER BY playername");



					if (!mysql_num_rows($result)) {



						$message="scores not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {



									$assoc[$row['playername']][L4P]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}
//mysql_free_result($result);

//--END PS

/*foreach($assoc as $key=>$value){

	echo "playername: ".$key."<br>";
	echo "W1: ".$value[W1]."<br>";
	echo "L1: ".$value[L1]."<br>";
	echo "W2: ".$value[W2]."<br>";
	echo "L2: ".$value[L2]."<br>";
	echo "W3: ".$value[W3]."<br>";
	echo "L3: ".$value[L3]."<br>";
	echo "W4: ".$value[W4]."<br>";
	echo "L4: ".$value[L4]."<br>";
	echo "WT: ".$value[WT]."<br>";
	echo "LT: ".$value[LT]."<br><hr><br>";



}


$assoc['testplayer'][0]='TESTPLAYER';
$assoc['testplayer'][W1]=8384366;
$assoc['testplayer'][L1]=0;
$assoc['testplayer'][W2]=0;
$assoc['testplayer'][L2]=0;
$assoc['testplayer'][W3]=0;
$assoc['testplayer'][L3]=0;
$assoc['testplayer'][W4]=0;
$assoc['testplayer'][L4]=0;
$assoc['testplayer'][WT]=0;
$assoc['testplayer'][LT]=0;
*/

function cmpW1($a, $b)
{
    if ($a[W1] == $b[W1]) {
        return 0;
    }
    return ($a[W1] < $b[W1]) ? 1 : -1;
}


function cmpL1($a, $b)
{
    if ($a[L1] == $b[L1]) {
        return 0;
    }
    return ($a[L1] < $b[L1]) ? 1 : -1;

}

function cmpW2($a, $b)
{
    if ($a[W2] == $b[W2]) {
        return 0;
    }
    return ($a[W2] < $b[W2]) ? 1 : -1;
}

function cmpL2($a, $b)
{
    if ($a[L2] == $b[L2]) {
        return 0;
    }
    return ($a[L2] < $b[L2]) ? 1 : -1;

}


function cmpW3($a, $b)
{
    if ($a[W3] == $b[W3]) {
        return 0;
    }
    return ($a[W3] < $b[W3]) ? 1 : -1;
}


function cmpL3($a, $b)
{
    if ($a[L3] == $b[L3]) {
        return 0;
    }
    return ($a[L3] < $b[L3]) ? 1 : -1;

}

function cmpW4($a, $b)
{
    if ($a[W4] == $b[W4]) {
        return 0;
    }
    return ($a[W4] < $b[W4]) ? 1 : -1;
}


function cmpL4($a, $b)
{
    if ($a[L4] == $b[L4]) {
        return 0;
    }
    return ($a[L4] < $b[L4]) ? 1 : -1;

}


function cmpWT($a, $b)
{
    if ($a[WT] == $b[WT]) {
        return 0;
    }
    return ($a[WT] < $b[WT]) ? 1 : -1;
}


function cmpLT($a, $b)
{
    if ($a[LT] == $b[LT]) {
        return 0;
    }
    return ($a[LT] < $b[LT]) ? 1 : -1;

}

//--PS

function cmpW1P($a, $b)
{
    if ($a[W1P] == $b[W1P]) {
        return 0;
    }
    return ($a[W1P] < $b[W1P]) ? 1 : -1;
}


function cmpL1P($a, $b)
{
    if ($a[L1P] == $b[L1P]) {
        return 0;
    }
    return ($a[L1P] < $b[L1P]) ? 1 : -1;

}

function cmpW2P($a, $b)
{
    if ($a[W2P] == $b[W2P]) {
        return 0;
    }
    return ($a[W2P] < $b[W2P]) ? 1 : -1;
}

function cmpL2P($a, $b)
{
    if ($a[L2P] == $b[L2P]) {
        return 0;
    }
    return ($a[L2P] < $b[L2P]) ? 1 : -1;

}


function cmpW3P($a, $b)
{
    if ($a[W3P] == $b[W3P]) {
        return 0;
    }
    return ($a[W3P] < $b[W3P]) ? 1 : -1;
}


function cmpL3P($a, $b)
{
    if ($a[L3P] == $b[L3P]) {
        return 0;
    }
    return ($a[L3P] < $b[L3P]) ? 1 : -1;

}

function cmpW4P($a, $b)
{
    if ($a[W4P] == $b[W4P]) {
        return 0;
    }
    return ($a[W4P] < $b[W4P]) ? 1 : -1;
}


function cmpL4P($a, $b)
{
    if ($a[L4P] == $b[L4P]) {
        return 0;
    }
    return ($a[L4P] < $b[L4P]) ? 1 : -1;

}

//END PS


uasort($assoc, "cmpW1");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW1=0;
$playerW1="";
$scoreW1="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW1=$prevscore=$value[W1];
		$playerW1=$value[0];


	}
	if($i==1){

		if($value[W1]==$prevscore)
			$tiedW1=1;

		break;
	}

	$i++;

}

$W1text=($tiedW1==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW1."&#39;s profile.' target='new' href='profile.php?playername=".$playerW1."'>".$playerW1."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW1."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW1."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";

//echo "tiedW1: ".$tiedW1;

uasort($assoc, "cmpL1");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL1=0;
$playerL1="";
$scoreL1="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL1=$prevscore=$value[L1];
		$playerL1=$value[0];


	}
	if($i==1){

		if($value[L1]==$prevscore)
			$tiedL1=1;

		break;
	}

	$i++;

}


$L1text=($tiedL1==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL1."&#39;s profile.' target='new' href='profile.php?playername=".$playerL1."'>".$playerL1."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL1."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL1."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";


uasort($assoc, "cmpW2");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW2=0;
$playerW2="";
$scoreW2="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW2=$prevscore=$value[W2];
		$playerW2=$value[0];


	}
	if($i==1){

		if($value[W2]==$prevscore)
			$tiedW2=1;

		break;
	}

	$i++;

}


$W2text=($tiedW2==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW2."&#39;s profile.' target='new' href='profile.php?playername=".$playerW2."'>".$playerW2."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW2."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW2."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";


uasort($assoc, "cmpL2");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL2=0;
$playerL2="";
$scoreL2="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL2=$prevscore=$value[L2];
		$playerL2=$value[0];


	}
	if($i==1){

		if($value[L2]==$prevscore)
			$tiedL2=1;

		break;
	}

	$i++;

}


$L2text=($tiedL2==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL2."&#39;s profile.' target='new' href='profile.php?playername=".$playerL2."'>".$playerL2."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL2."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL2."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";

uasort($assoc, "cmpW3");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW3=0;
$playerW3="";
$scoreW3="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW3=$prevscore=$value[W3];
		$playerW3=$value[0];


	}
	if($i==1){

		if($value[W3]==$prevscore)
			$tiedW3=1;

		break;
	}

	$i++;

}

$W3text=($tiedW3==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW3."&#39;s profile.' target='new' href='profile.php?playername=".$playerW3."'>".$playerW3."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW3."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW3."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";


uasort($assoc, "cmpL3");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL3=0;
$playerL3="";
$scoreL3="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL3=$prevscore=$value[L3];
		$playerL3=$value[0];


	}
	if($i==1){

		if($value[L3]==$prevscore)
			$tiedL3=1;

		break;
	}

	$i++;

}

$L3text=($tiedL3==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL3."&#39;s profile.' target='new' href='profile.php?playername=".$playerL3."'>".$playerL3."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL3."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL3."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";


uasort($assoc, "cmpW4");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW4=0;
$playerW4="";
$scoreW4="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW4=$prevscore=$value[W4];
		$playerW4=$value[0];


	}
	if($i==1){

		if($value[W4]==$prevscore)
			$tiedW4=1;

		break;
	}

	$i++;

}

$W4text=($tiedW4==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW4."&#39;s profile.' target='new' href='profile.php?playername=".$playerW4."'>".$playerW4."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW4."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW4."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";


uasort($assoc, "cmpL4");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL4=0;
$playerL4="";
$scoreL4="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL4=$prevscore=$value[L4];
		$playerL4=$value[0];


	}
	if($i==1){

		if($value[L4]==$prevscore)
			$tiedL4=1;

		break;
	}

	$i++;

}


$L4text=($tiedL4==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL4."&#39;s profile.' target='new' href='profile.php?playername=".$playerL4."'>".$playerL4."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL4."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL4."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";

//--PS

uasort($assoc, "cmpW1P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW1=0;
$playerW1="";
$scoreW1="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW1=$prevscore=$value[W1P];
		$playerW1=$value[0];


	}
	if($i==1){

		if($value[W1P]==$prevscore)
			$tiedW1=1;

		break;
	}

	$i++;

}

$W1Ptext=($tiedW1==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW1."&#39;s profile.' target='new' href='profile.php?playername=".$playerW1."'>".$playerW1."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW1."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW1."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";

//echo "tiedW1: ".$tiedW1;

uasort($assoc, "cmpL1P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL1=0;
$playerL1="";
$scoreL1="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL1=$prevscore=$value[L1P];
		$playerL1=$value[0];


	}
	if($i==1){

		if($value[L1P]==$prevscore)
			$tiedL1=1;

		break;
	}

	$i++;

}


$L1Ptext=($tiedL1==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL1."&#39;s profile.' target='new' href='profile.php?playername=".$playerL1."'>".$playerL1."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL1."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL1."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";


uasort($assoc, "cmpW2P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW2=0;
$playerW2="";
$scoreW2="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW2=$prevscore=$value[W2P];
		$playerW2=$value[0];


	}
	if($i==1){

		if($value[W2P]==$prevscore)
			$tiedW2=1;

		break;
	}

	$i++;

}


$W2Ptext=($tiedW2==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW2."&#39;s profile.' target='new' href='profile.php?playername=".$playerW2."'>".$playerW2."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW2."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW2."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";


uasort($assoc, "cmpL2P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL2=0;
$playerL2="";
$scoreL2="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL2=$prevscore=$value[L2P];
		$playerL2=$value[0];


	}
	if($i==1){

		if($value[L2P]==$prevscore)
			$tiedL2=1;

		break;
	}

	$i++;

}


$L2Ptext=($tiedL2==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL2."&#39;s profile.' target='new' href='profile.php?playername=".$playerL2."'>".$playerL2."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL2."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL2."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";

uasort($assoc, "cmpW3P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW3=0;
$playerW3="";
$scoreW3="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW3=$prevscore=$value[W3P];
		$playerW3=$value[0];


	}
	if($i==1){

		if($value[W3P]==$prevscore)
			$tiedW3=1;

		break;
	}

	$i++;

}

$W3Ptext=($tiedW3==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW3."&#39;s profile.' target='new' href='profile.php?playername=".$playerW3."'>".$playerW3."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW3."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW3."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";


uasort($assoc, "cmpL3P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL3=0;
$playerL3="";
$scoreL3="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL3=$prevscore=$value[L3P];
		$playerL3=$value[0];


	}
	if($i==1){

		if($value[L3P]==$prevscore)
			$tiedL3=1;

		break;
	}

	$i++;

}

$L3Ptext=($tiedL3==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL3."&#39;s profile.' target='new' href='profile.php?playername=".$playerL3."'>".$playerL3."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL3."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL3."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";


uasort($assoc, "cmpW4P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedW4=0;
$playerW4="";
$scoreW4="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreW4=$prevscore=$value[W4P];
		$playerW4=$value[0];


	}
	if($i==1){

		if($value[W4P]==$prevscore)
			$tiedW4=1;

		break;
	}

	$i++;

}

$W4Ptext=($tiedW4==0)?"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerW4."&#39;s profile.' target='new' href='profile.php?playername=".$playerW4."'>".$playerW4."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreW4."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Wad Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreW4."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";


uasort($assoc, "cmpL4P");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedL4=0;
$playerL4="";
$scoreL4="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreL4=$prevscore=$value[L4P];
		$playerL4=$value[0];


	}
	if($i==1){

		if($value[L4P]==$prevscore)
			$tiedL4=1;

		break;
	}

	$i++;

}


$L4Ptext=($tiedL4==0)?"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><a style='color:#93DB70;' title='Click here to see ".$playerL4."&#39;s profile.' target='new' href='profile.php?playername=".$playerL4."'>".$playerL4."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreL4."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Level Hi-Score Champion: </span><span style='color:#CE0000;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreL4."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";



//END PS


uasort($assoc, "cmpWT");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedWT=0;
$playerWT="";
$scoreWT="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreWT=$prevscore=$value[WT];
		$playerWT=$value[0];


	}
	if($i==1){

		if($value[WT]==$prevscore)
			$tiedWT=1;

		break;
	}

	$i++;

}

$WTtext=($tiedWT==0)?"<b><span style='color:#D9D919;'>Supreme Wad Hi-Score Champion: </span><a style='color:#93DB70;font-size:15px;' title='Click here to see ".$playerWT."&#39;s profile.' target='new' href='profile.php?playername=".$playerWT."'>".$playerWT."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreWT."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Supreme Wad Hi-Score Champion: </span><span style='color:#CE0000;font-size:15px;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreWT."</span><span style='color:#CE0000;'> total wad hi-score points!</span></b>";

//echo $WTtext."<br>";

uasort($assoc, "cmpLT");



$i=0; //check for tied hi scores, as well as get leading playername & score
$tiedLT=0;
$playerLT="";
$scoreLT="";

foreach($assoc as $key=>$value){
	if($i==0){
		$scoreLT=$prevscore=$value[LT];
		$playerLT=$value[0];


	}
	if($i==1){

		if($value[LT]==$prevscore)
			$tiedLT=1;

		break;
	}

	$i++;

}


$LTtext=($tiedLT==0)?"<b><span style='color:#D9D919;'>Supreme Level Hi-Score Champion: </span><a style='color:#93DB70;font-size:15px;' title='Click here to see ".$playerLT."&#39;s profile.' target='new' href='profile.php?playername=".$playerLT."'>".$playerLT."</a><span style='color:#CE0000;'> with </span><span style='color:#D9D919;'>".$scoreLT."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>":"<b><span style='color:#D9D919;'>Supreme Wad Hi-Score Champion: </span><span style='color:#CE0000;font-size:15px;'>TIED</span><span style='color:#CE0000;'> at </span><span style='color:#D9D919;'>".$scoreLT."</span><span style='color:#CE0000;'> total level hi-score points!</span></b>";
//echo $LTtext."<br>";

?>











<style type="text/css">

<!--

.style1 {font-size: 11pt;color:white;color:rgb(255, 218, 185);}

.style2 {font-size: 12pt;color:white;color:rgb(255, 218, 185);}

.style2b {font-size: 12pt;color:white;font-weight:bold;color:rgb(255, 218, 185);}

.indent {left:10px;position:relative;}

.emph {color:rgb(255, 192, 203);}

table {border-width:0px;border-color:black;border-style:outset;border-bottom:0px;color:black;font-weight:bold;}

td  {border-width:0px;border-color:black;border-style:inset;border-left;0px;border-right:0px;}

th  {border-width:0px;border-color:black;border-style:inset;border-left;0px;border-right:0px;}



A {

	color: rgb(255, 218, 185);

}

A:link, A:visited

{

	text-decoration: none;

	color: rgb(255, 192, 203);

}



A:hover, A:active

{

	text-decoration: underline;

	//color: #bfbfff;

}



.parTable th {

color:rgb(255, 218, 185);





}



.parTable td {

color: rgb(255, 192, 203);





}

-->

</style>

</head>

<body background="TALLYHO!readme_files/stars.gif" style="font-family:Verdana,Arial,Helvetica,sans-serif;font-size:75%;background-color:black;color:rgb(211, 211, 211);">
<!--<center>
<p>
<b><i>"Doom feels more like 1st person Robotron than a modern FPS"</i></b>
<br>
- <a href="http://vectorpoem.com/news/?p=74" target="sdwin">vector poem</a>
</p>
</center>
-->

<br>
<center>
<p  style="font-weight:bold;">
<span class="style1" style="font-size:16pt;">ScoreDoom 3.x Hi Score Tables</span><br>
(<a style="font-size: 11pt; font-weight: bold;color: red;" target="sdwin2" href="/archive/hiscoretables.php">Click here to view the ScoreDoom 2.x Hi Score Table Archives</a>)
</p>


<table style="width:1000px;padding-left:100px;"><tr><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;">Regular Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a><br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W1text."<BR>".$L1text;

?>
</div>
<br><br>
</td><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;"><span style="color: red;">Hardcore</span> Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/hardcore/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/hardcore/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);" href="http://www.scoredoom.com/hardcore/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a>
<br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W2text."<BR>".$L2text;

?>
</div>
<br><br>

</td></tr><tr><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;">Regular w. <span style="color: white;">Non-Infighting</span> Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ni/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ni/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ni/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a>
<br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W3text."<BR>".$L3text;

?>
</div>
<br>
</td><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;"><span style="color: red;">Hardcore</span> w. <span style="color: white;">Non-Infighting</span> Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ni/hardcore/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ni/hardcore/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ni/hardcore/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a>
<br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W4text."<BR>".$L4text;

?>
</div>

<br>
</td></tr></table>

</center>


<center>
<p  style="font-weight:bold;">
<span class="style1" style="font-size:14pt; color:#93DB70;">Pistol Start Mode Tables</span><br>
(<span style="font-size: 11pt; font-weight: bold;color: red;">Best for individual level score attacks</span>)
</p>


<table style="width:1000px;padding-left:100px;"><tr><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;">Regular Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a><br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W1Ptext."<BR>".$L1Ptext;

?>
</div>
<br><br>
</td><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;"><span style="color: red;">Hardcore</span> Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/hardcore/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/hardcore/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);" href="http://www.scoredoom.com/ps/hardcore/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a>
<br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W2Ptext."<BR>".$L2Ptext;

?>
</div>
<br><br>

</td></tr><tr><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;">Regular w. <span style="color: white;">Non-Infighting</span> Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/ni/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/ni/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/ni/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a>
<br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W3Ptext."<BR>".$L3Ptext;

?>
</div>
<br>
</td><td>
<span style="color:rgb(156, 156, 222);font-weight:bold;font-size:13pt;"><span style="color: red;">Hardcore</span> w. <span style="color: white;">Non-Infighting</span> Mode: </span><br>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/ni/hardcore/wad_main.php?&amp;addonpack=0" target="sdwin">Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/ni/hardcore/wad_main.php?&amp;addonpack=1" target="sdwin">Add-On Pack Hi Scores</a><br>

<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/ps/ni/hardcore/wad_main_level_latest2.php" target="sdwin">View The Latest Level Hi Scores Set</a>
<br>
<div style="font-size:9px;margin-top:5px;">
<?

echo $W4Ptext."<BR>".$L4Ptext;

?>
</div>

<br>
</td></tr></table>

</center>


<center>
<p  style="font-weight:bold;">
<span class="style1" style="font-size:16pt;">Supreme Hi Scorer Leaderboards</span><br>
</p>

<table style="width:1000px;padding-left:100px;"><tr><td>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/bestplayers.php?sorttype=wad" target="sdwin">Supreme Wad Hi Scorer Leaderboard</a>
</td><td>
<a style="font-size: 14px; font-weight: bold; color: rgb(206, 0, 0);"  href="http://www.scoredoom.com/bestplayers.php?sorttype=lvl" target="sdwin">Supreme Level Hi Scorer Leaderboard</a>

</td>

</tr>
</table>

<p>
<?
echo $WTtext."<BR>";
echo $LTtext;
?>
</p>
<br>

</center>

<p  style="font-weight:bold;">
<span class="style1" style="color: rgb(156, 156, 222);">Hi-Score Tables Explained</span><br>
</p>
<i>For questions, and much more detailed information relating to this overview, please check out the <a href="scoredoomfaq.html" TARGET="sdwin">ScoreDoom FAQ</a></i>
<p>
Regular ScoreDoom (not ScoreDoomST) supports hi scores. In offline mode, you keep local hi scores. In online mode, you compete with others on online hi-score tables to be the highest scorer for the level or game (wad) you are playing on. Both offline and online modes support Regular/Hardcore modes combined with Infighting/Non-Infighting modes and Pistol Start mode. You can mix and match modes:
<ul>
<li><b>Regular Mode(default)</b>: You can save at anytime during a level.  Can be played with Infighting or Non-Infighting mode .
</li>
<li><b>Hardcore Mode</b>: You can only use auto-saves at the beginning of a level. When you die, you start the level over. Hardcore save-games have their own names prefixed with 'hardcore'. Can be played with Infighting Non-Infighting mode.
</li>
<li><b>Infighting Mode (default)</b>: Monsters infight like in vanilla Doom. Can be played with Regular or Hardcore mode.
</li>
<li><b>Non-Infighting Mode</b>: Monsters neither infight, or damage each other. Can be played with Regular or Hardcore mode.
</li>
<li><b>Pistol Start Mode</b>: Players start each level with default inventory. Can be played with Regular, Non-Infighting and/or Hardcore mode.
</li>
</ul>

So there are 8 distinct online hi score tables for: <br>Regular (default), Hardcore, Regular w. (Non-Infighting) and Hardcore w.(Non-Infighting), and 4 more with all these in Pistol Start mode!<br><br>

<i>Note: Completing very hard mega-wads like Hell Revealed, can be virtually impossible on ultra-violence in Hardcore Mode. But Hardcore w.(Non-Infighting) represents the greatest challenge for any level or wad.</i>
<p><b><span class="style1" style="color: rgb(156, 156, 222);">Wad(/Episode) Hi-Scores vs Level Hi-Scores</span></b></p>
Wad Hi Scores are based on scores added up for a single session of play across one level or more within a wad configuration. ScoreDoom supports saved games for this. Level Hi Scores are also recorded per level. It is possible to play a single level and beat it's hi score without changing the Wad Hi-Score, thus the sum of the Level Hi-Scores can be greater than the Wad Hi-Score, for a wad configuration. <i>Note: Level Hi-Scores are probably better attempted during a session of play (this includes saved games) across several levels or full megawads, since players likely start off with extra weapons at the start of levels.</i>

<p><b><span class="style1" style="color: rgb(156, 156, 222);">Hi-Score Champions Explained</span></b></p>
Each of the 8 distinct hi score tables has 2 hi score champions. One for the most total cumulative wad hi scores amassed, and another for the most total cumulative level hi scores amassed.<br>
There is also a supreme ScoreDoom wad hi score champion and supreme ScoreDoom level hi score champion representing the most total cumulative hi scores across all 8 hi score tables.


</p>
<p  style="font-weight:bold;">
<span class="style1" style="color: rgb(156, 156, 222);">Online Player Profiles and Passwords</span>

</p>
When using online hi-scoring, players are recommended to use a unique player-name and unique hi-score server password. This will properly identify you when setting online hi-scores, instead of sharing '<a href="http://www.scoredoom.com/profile.php?playername=Player" target="sdwin"><b>Player</b></a>' :-)<br>
You should also create an online profile for yourself.<br><br>
<b>You may change your password and player name in the ScoreDoom client.</b><br><br>
<b>Once you are happy with your player name and password, feel free to upload an image and say something about yourself <a href="http://www.scoredoom.com/upload.php" target="sdwin">here</a>.</b>



</body>