<html><head>
<link rel="stylesheet" type="text/css" href="css/styles.css" />
<title>ScoreDoom Hi Score Tables</title>




</head><body background="TALLYHO!readme_files/stars.gif" style="font-family:Verdana,Arial,Helvetica,sans-serif;font-size:75%;"><center>
<?php
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


	$result = mysql_query("SELECT * FROM `wad_main_level` WHERE levelhiscore > 0 ORDER BY `gmt_timestamp` DESC LIMIT 0 , 30");



if (!$result) {
   die("Query to show fields from table failed");
}

$fields_num = mysql_num_fields($result);

if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){
	echo "<p class='header'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br><span style='color:#93DB70'>Pistol Start Mode</span><br><span style='color:rgb(156, 156, 222);'>Regular Mode: </span>".$_GET["wadtitle"]." Level Hi Score Table</p>";
}else{
	echo "<p class='header'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br><span style='color:#93DB70'>Pistol Start Mode</span><br><span style='color:rgb(156, 156, 222);'>Regular Mode: </span>Latest Level Hi Scores Set</p>";
}


echo "<table border='1' class='hiscoreTable'><tr>";
// printing table headers

echo "<th>Level</th><th>Level Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";
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

	//echo "<td class='lvlslink'><a href='wad_main_level.php?wadhash=".$row['wadhash']."&addonpack_hash=".$row['addonpack_hash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$full_wad_name."</a></td><td>".$episode."</td>";
	echo "<td style='text-align:left;'><a class='lvlslink' style='font-size:8pt;' href='wad_main_level.php?wadhash=".$row['wadhash']."&addonpack_hash=".$row['addonpack_hash']."&epi=".$row['episode']."&wadtitle=".urlencode($full_wad_name)."&levelnamesid=".$id."' title='Click here to see the level hi scores for".$full_wad_name."'>".$full_wad_name."</a>: ".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$row['levelhiscore']."</td><td class='player'><a href='http://www.scoredoom.com/profile.php?playername=".urlencode($row['playername'])."' target=new title='Click here to see ".$row['playername']."&#39;s profile.'>".$row['playername']."</a></td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";



   // $row is array... foreach( .. ) puts every element
   // of $row to $cell variable
  // foreach($row as $cell)
      // echo "<td>$cell</td>";

   echo "</tr>\n";
}
mysql_free_result($result);
?>
</table>
<a class="back" href="#" onclick="window.close();">close</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;<a class="back" href="wad_main.php">view wad hi score table</a>
</center>
<div style="position:absolute;top:45px;left:15px;text-align:center;width:360px;">
<a id="hscontrols" href="#" onclick="window.close();">Close</a>
</div>

</body></html>