<html><head>
<link rel="stylesheet" type="text/css" href="css/styles.css" />
<title>wad_main_level</title>




</head><body><center>
<?php
$db_host = 'localhost';
$db_user = 'ddraig_admin';
$db_pwd = 'yourpwd';

$database = 'ddraig_ps_sdhiscoresh';
$table = 'wad_main_level';

if (!mysql_connect($db_host, $db_user, $db_pwd))
   die("Can't connect to database");

if (!mysql_select_db($database))
   die("Can't select database");

//get any level names from the levelnames table if an id is set
if(isset($_GET["levelnamesid"])&&$_GET["levelnamesid"]!=""){
	$levelnamesid=$_GET["levelnamesid"];

	$result2 = mysql_query("SELECT * FROM wad_main_levelnames WHERE wad_main_levelnames.id=".$levelnamesid." ORDER BY LOWER(LEVEL)");

}

while($row2 = mysql_fetch_assoc($result2))
{
	$row2['levelname'];
	$row2['level'];

	$assoc[$row2['level']] = $row2['levelname']; //build dynamic assoc. array


}


// sending query
if(isset($_GET["epi"])&&$_GET["epi"]!=""){
	$result = mysql_query("SELECT * FROM wad_main_level WHERE wad_main_level.wadhash='".$_GET['wadhash']."' AND wad_main_level.episode='".$_GET['epi']."' ORDER BY LOWER(LEVELNAME)");
}else{
	$result = mysql_query("SELECT * FROM wad_main_level WHERE  wad_main_level.wadhash='".$_GET['wadhash']."' ORDER BY LOWER(LEVELNAME)");

}

if (!$result) {
   die("Query to show fields from table failed");
}

$fields_num = mysql_num_fields($result);

if(isset($_GET["wadtitle"])&&$_GET["wadtitle"]!=""){
	echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br><span style='color:#93DB70'>Pistol Start Mode</span><br>'".$_GET["wadtitle"]."' <span style='color:red;font-style:italic;'>Hardcore</span> Level Hi Score Table</p>";
}else{
	echo "<p class='header'><a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><br><span style='color:#93DB70'>Pistol Start Mode</span><br>ScoreDoom Official Wad Hi Score Table</p>";
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

	/*if($row['full_level_name']){
		$full_level_name=": ".$row['full_level_name'];
	}else{
		$full_level_name="";
	}
	*/

	if($assoc[$row['levelname']]){ //check if exists in assoc. array built from levelnames
		$full_level_name=": ".$assoc[$row['levelname']];
	}else{
		$full_level_name="";
	}

	if($row['levelhiscore']==0){
			$levelhiscore="<span style='color:#FF2400'>0</span>";

		}else{
			$levelhiscore=$row['levelhiscore'];

	}

   echo "<tr style='background-color:".$bgcolor."';>";

	//echo "<td class='lvlslink'><a href='wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."' title='Click here to see the level hi scores'>".$row['wad_epi_desc']."</a></td><td>".$episode."</td>";
	echo "<td>".$row['levelname'].$full_level_name."</td><td class='hiscore'>".$levelhiscore."</td><td class='player'><a href='http://www.scoredoom.com/profile.php?playername=".urlencode($row['playername'])."' target=new title='Click here to see ".$row['playername']."&#39;s profile.'>".$row['playername']."</a></td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";



   // $row is array... foreach( .. ) puts every element
   // of $row to $cell variable
  // foreach($row as $cell)
      // echo "<td>$cell</td>";

   echo "</tr>\n";
}
mysql_free_result($result);
?>
</table>
<a class="back" href="javascript:history.go(-1);">back</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;<a class="back" href="wad_main.php">view hardcore wad hi score table</a>
</center>

</body></html>