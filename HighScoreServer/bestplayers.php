<head>



<?php

define("W1", 1);define("L1", 2);
define("W2", 3);define("L2", 4);
define("W3", 5);define("L3", 6);
define("W4", 7);define("L4", 8);
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



									$assoc[strtolower($row['playername'])]=array($row['playername'],0,0,0,0,0,0,0,0,0,0);


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

$database = 'ddraig_ps_sdhiscores';
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

								//print_r ($row);
								//echo "<br><br><br>";


									//$assoc[$row['playername']][W1P]+=$row['wadhiscore'];
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



									//$assoc[$row['playername']][L1P]+=$row['levelhiscore'];
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



									//$assoc[$row['playername']][W2P]+=$row['wadhiscore'];
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



									//$assoc[$row['playername']][L2P]+=$row['levelhiscore'];
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



									//$assoc[$row['playername']][W3P]+=$row['wadhiscore'];
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



									//$assoc[$row['playername']][L3P]+=$row['levelhiscore'];
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



									//$assoc[$row['playername']][W4P]+=$row['wadhiscore'];
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



									//$assoc[$row['playername']][L4P]+=$row['levelhiscore'];
									$assoc[$row['playername']][LT]+=$row['levelhiscore'];

								}



					}



	if($failed==1){



	}



//END PS

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

$text="";
if(isset($_GET['sorttype']))
	$sorttype=$_GET['sorttype'];
else
	$sorttype="";


if($sorttype=="wad"){
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

	$text="Supreme Wad Hi Scorer Leaderboard";

	//echo $WTtext."<br>";
}else if($sorttype=="lvl"){

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


	$text="Supreme Level Hi Scorer Leaderboard";
}
?>










<link rel="stylesheet" type="text/css" href="css/styles.css" />


<style type="text/css">



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
<center>
<?

echo "<div class='header'><a href='http://www.scoredoom.com'><img height='30px;' border='0' src='images/scoredoom.gif'></a><br><span style='color:rgb(156, 156, 222);'>".$text."</span></div>";
echo "<br><br><br><table border='1' class='hiscoreTable' style='width:800px;'><tr>";
// printing table headers

if($sorttype=="wad")
	echo "<th>Name</th><th>Total Wad Hi Score Points</th>";
else if($sorttype=="lvl")
	echo "<th>Name</th><th>Total Level Hi Score Points</th>";

echo "</tr>\n";


// printing table rows
$cnt=0;
$bgcolor="#111111";
$dl_url="";

foreach($assoc as $key=>$value)
{
	$cnt+=1;

	if($cnt%2==0){
		$bgcolor="#111111";
	}else{
		$bgcolor="#222222";
	}



   echo "<tr style='background-color:".$bgcolor."';>";

	if($sorttype=="wad"){
		if($value[WT]==0)
			break;

		echo "<td class='player'><span style='float:left;color:#FFC0CB'>".$cnt."</span><a href='profile.php?playername=".urlencode($value[0])."' target=new title='Click here to see ".$value[0]."&#39;s profile.'>".$value[0]."</a></td><td class='hiscore'>".$value[WT]."</td>";
	}else if($sorttype=="lvl")
	{
		if($value[LT]==0)
			break;

		echo "<td class='player'><span style='float:left;color:#FFC0CB'>".$cnt."</span><a href='profile.php?playername=".urlencode($value[0])."' target=new title='Click here to see ".$value[0]."&#39;s profile.'>".$value[0]."</a></td><td class='hiscore'>".$value[LT]."</td>";

	}

   echo "</tr>\n";
}

?>
</table>
</center>
<br>
<a class="back" style="color:#FF2400;" href="#" onclick="window.close();" >Close</a>


</body>