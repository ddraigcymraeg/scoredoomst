



<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">







<head>

<meta http-equiv="PRAGMA" content="NO-CACHE">

<meta http-equiv="EXPIRES" content="-1">

<link rel="stylesheet" type="text/css" href="css/styles.css" />



    <title>ScoreDoom Player Profile Settings</title>



    <script language="javascript">

    function countCharsLeft(objTA){



		document.getElementById('charFree').innerHTML=300-parseInt(objTA.value.length);







	}



	function imposeMaxLength(objTA)

	{

		if(objTA.value.length >= 300){

			objTA.value=objTA.value.substring(0,299);



		}



	  //return (objTA.value.length < MaxLen);

	}









	function checkform(){





	if(document.getElementById('playername').value==""||document.getElementById('pwd').value==""){



		alert('Invalid Form Entry');

		return false;



	}





		return true;





	}

</script>


    <style type="text/css">



	<!--



	.style1 {font-size: 14pt;color:white;color:rgb(255, 218, 185);}



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




    <style>
        fieldset {
            width: 50%;
            margin: 15px 0px 25px 0px;
            padding: 15px;
        }
        legend {
            font-weight: bold;
        }
        .button {
            text-align: right;
        }
        .button input {
            font-weight: bold;
        }

    </style>


</head>



<body background="TALLYHO!readme_files/stars.gif" style="color: rgb(211, 211, 211);font-family:Verdana,Arial,Helvetica,sans-serif;font-size:60%;">

<center>

<?php



ini_set ( "memory_limit", "40M");

$message="";

$failed=0;

//error_reporting(E_ALL);





//**TODO: Need to add extension field to player table in database. If it exists, there is an image.

//If not, there is not an image.





// we first include the upload class, as we will need it here to deal with the uploaded file

include('class.upload.php');



if(isset($_GET['playername'])){



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







					$result = mysql_query("select * from players where  playername='".$_GET['playername']."'");



					if (!mysql_num_rows($result)) {



						$message="Player name & password not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {

									$image_ext=$row['image_ext'];

									$playername=$row['playername'];

									$bio=$row['Bio'];



								}



					}



	if($failed==1){





						// db error occured

						echo '<fieldset>';

						echo '  <legend>database error occured</legend>';

						echo '  Error: ' . $message . '';

					echo '</fieldset>';



	}

}



/*

echo '<p><a href="index.html">do another test</a></p>';



echo '<pre>';

echo($handle->log);

echo '</pre>';

*/

echo '<h1 class="header2">ScoreDoom Player Profile </h1>';



?>










	<?php



	//if(isset($_POST['view'])&&$_POST['view']>0&&$failed!=1){



$db_host = 'localhost';

					$db_user = 'ddraig_admin';

					$db_pwd = 'yourpwd';



					$database = 'ddraig_sdhiscores';



					if (!mysql_connect($db_host, $db_user, $db_pwd))

						  die("Can't connect to database");



					if (!mysql_select_db($database))

						  die("Can't select database");







					$result = mysql_query("select * from players where playername='".$_GET['playername']."'");



					if (!mysql_num_rows($result)) {



						$message="Player name & password not found.";



						$failed=1;

						mysql_free_result($result);

					}else{

								while ( $row = mysql_fetch_array($result) ) {

									$image_ext=$row['image_ext'];

									$playername=$row['playername'];

									$bio=$row['Bio'];



								}



					}









		echo '<table style="width: 600px;background:url(\'TALLYHO!readme_files/stars.gif\');" class="hiscoreTable"><tbody><tr>

		        <td style="font-size: 20pt; font-weight: bold; color: rgb(170, 0, 0);">'.$playername.'</td></tr>



				<tr><td>';



				if($image_ext!=''){

					$playername2 = strtolower($playername);

					 $playername2 = str_replace(array(' ', '-'), array('_','_'), $playername2) ;

                	$playername2 = ereg_replace('[^A-Za-z0-9_]', '', $playername2) ;



					echo '<img src="imagecache/'.str_replace(" ","_",$playername2).'.'.$image_ext.'?'.time().'"/>';

				}





echo '</td></tr>';

echo '<tr><td style="width:500px;color: rgb(255, 218, 185); font-size: 11pt; font-weight: normal;">'.$bio.'

				</td></tr></table>';



//echo '<script language="javascript">document.getElementById("playername").value="'.$playername.'";

//document.getElementById("pwd").value="'.$_POST['pwd'].'";

//document.getElementById("addInfoTextArea").value="'.$bio.'";

//</script>';



//GET Regular Wad Hi Scores...



$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscores';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;">Regular Mode: </span> Wad Hi Scores Set</h2>';



}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers



if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';







$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscores';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;">Regular Mode: </span> Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';















$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscoresh';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: red;">Hardcore</span> Mode: </span> Wad Hi Scores Set</h2>';

}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscoresh';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: red;">Hardcore</span> Mode: </span> Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";



}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





















$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscoresni';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;">Regular w. <span style="color: white;">Non-Infighting</span> Mode: </span> Wad Hi Scores Set</h2>';

}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/ni/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscoresni';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;">Regular w. <span style="color: white;">Non-Infighting</span> Mode: </span> Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";



}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/ni/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/ni/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';

















$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscoresnih';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: red;">Hardcore</span> w. <span style="color: white;">Non-Infighting</span> Mode: </span> Wad Hi Scores Set</h2>';

}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/ni/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_sdhiscoresnih';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: red;">Hardcore</span> w. <span style="color: white;">Non-Infighting</span> Mode: </span>  Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";



}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/ni/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/ni/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';


//}


//--PS

//GET Regular Wad Hi Scores...



$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscores';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> Regular Mode: </span> Wad Hi Scores Set</h2>';



}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers



if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/ps/ad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';







$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscores';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> Regular Mode: </span> Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/ps/ad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/ps/ad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';















$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscoresh';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> <span style="color: red;">Hardcore</span> Mode: </span> Wad Hi Scores Set</h2>';

}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/ps/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscoresh';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> <span style="color: red;">Hardcore</span> Mode: </span> Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";



}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/ps/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/ps/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





















$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscoresni';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> Regular w. <span style="color: white;">Non-Infighting</span> Mode: </span> Wad Hi Scores Set</h2>';

}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/ps/ni/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscoresni';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> Regular w. <span style="color: white;">Non-Infighting</span> Mode: </span> Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";



}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/ps/ni/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/ps/ni/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';

















$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscoresnih';

$table = 'wad_main';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");



// sending query

$result = mysql_query("SELECT * FROM {$table} WHERE playername='".addslashes($playername)."' AND wadhiscore>0 ORDER BY WAD_EPI_DESC ");

if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);







if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> <span style="color: red;">Hardcore</span> w. <span style="color: white;">Non-Infighting</span> Mode: </span> Wad Hi Scores Set</h2>';

}

echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Name</th><th>Extra PWADS & Patches</th><th>Levels</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";

}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";

$dl_url="";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($row['dl_url']){

		$dl_url="&nbsp;<a href='".$row['dl_url']."' style='color:green;font-style:italic;font-size:8pt;text-decoration:underline;'>d/l</a>";



   	}else{

   		$dl_url="";



   	}



   	if($row['wadhiscore']==0){

		$wadhiscore="<span style='color:#FF2400'>0</span>";



	}else{

		$wadhiscore=$row['wadhiscore'];



	}



   echo "<tr style='background-color:".$bgcolor."';>";



	echo "<td class='lvlslink'><a href='/ps/ni/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";

	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';





$db_host = 'localhost';

$db_user = 'ddraig_admin';

$db_pwd = 'yourpwd';



$database = 'ddraig_ps_sdhiscoresnih';

$table = 'wad_main_level';



if (!mysql_connect($db_host, $db_user, $db_pwd))

   die("Can't connect to database");



if (!mysql_select_db($database))

   die("Can't select database");







$result2 = mysql_query("SELECT * FROM wad_main_levelnames ORDER BY id");





while($row2 = mysql_fetch_assoc($result2))

{

	$row2['levelname'];

	$row2['level'];



	$assoc[$row2['id']."+".$row2['level']] = $row2['levelname']; //build dynamic assoc. array





}





$result3 = mysql_query("SELECT * FROM wad_main");





while($row3 = mysql_fetch_assoc($result3))

{





	$assocWM[$row3['wadhash']."+".$row3['addonpack_hash']."+".$row3['episode']] = array( $row3['wad_epi_desc'], $row3['levelnamesid'] ); //build dynamic assoc. array





}





// sending query





	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 AND playername = '".addslashes($playername)."' ORDER BY `gmt_timestamp` DESC");







if (!$result) {

   die("Query to show fields from table failed");

}



$fields_num = mysql_num_fields($result);



if(mysql_num_rows($result)>0){

	echo '<h2><span style="color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;"><span style="color: #93DB70;">Pistol Start</span> <span style="color: red;">Hardcore</span> w. <span style="color: white;">Non-Infighting</span> Mode: </span>  Level Hi Scores Set</h2>';

}



//if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){

	//echo "<p class='header'>".$_GET["wadtitle"]." Level Hi Score Table</p>";

//}else{

	//echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br>Latest Level Hi Scores Set</p>";

//}





echo "<table border='1' class='hiscoreTable'><tr>";

// printing table headers

if(mysql_num_rows($result)>0){

	echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";



}

echo "</tr>\n";

// printing table rows

$cnt=0;

$bgcolor="#111111";



while($row = mysql_fetch_assoc($result))

{

	$cnt+=1;



	if($cnt%2==0){

		$bgcolor="#111111";

	}else{

		$bgcolor="#222222";

	}



	if($row['episode'])

		$episode=strtoupper($row['episode']);



	if($assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0]){ //check if exists in assoc. array built from wad_main

				$full_wad_name=$assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][0];

			}else{

				$full_wad_name="";

	}

		$id = $assocWM[$row['wadhash']."+".$row['addonpack_hash']."+".$row['episode']][1];

	if($assoc[$id."+".$row['levelname']]){ //check if exists in assoc. array built from levelnames

		$full_level_name=": ".$assoc[$id."+".$row['levelname']];

	}else{

		$full_level_name="";

	}









   echo "<tr style='background-color:".$bgcolor."';>";



	//echo "<td class='lvlslink'><a href='/ps/ni/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";

	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='/ps/ni/hardcore/wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".$row['addonpack_hash']."&wadtitle=".$full_wad_name."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'>".$row['playername']."</td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";







   // $row is array... foreach( .. ) puts every element

   // of $row to $cell variable

  // foreach($row as $cell)

      // echo "<td>$cell</td>";



   echo "</tr>\n";

}

mysql_free_result($result);



echo '</table><br>';


//}




// END PS




?>






<br>
<a class="back" style="color:#FF2400" href="#" onclick="window.close();">close</a>
</center>
<div style="position:absolute;top:45px;left:15px;text-align:center;width:360px;">
<a id="hscontrols" href="#" onclick="window.close();">Close</a>
</div>

</body>



</html>