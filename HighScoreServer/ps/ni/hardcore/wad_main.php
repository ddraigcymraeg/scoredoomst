<html><head>
<link rel="stylesheet" type="text/css" href="css/styles.css" />
<title>ScoreDoom Hi Score Tables</title>


</head><body  background="../../TALLYHO!readme_files/stars.gif" style="font-family:Verdana,Arial,Helvetica,sans-serif;font-size:60%;"><center>
<?php
$db_host = 'localhost';
$db_user = 'ddraig_admin';
$db_pwd = 'yourpwd';

$database = 'ddraig_ps_sdhiscoresnih';
$table = 'wad_main';

$sorttype="";
$sortorder="";
$extrasql="";
$addonpackquery="";
$sortquery="";

if(isset($_GET['addonpack'])&&$_GET['addonpack']!=""){
	$addonpackquery="&addonpack=".$_GET['addonpack'];

		if($_GET['addonpack']=="1"){
			$extrasql.=" WHERE isADDONPACK = 1";

		}elseif($_GET['addonpack']=="0"){
			$extrasql.=" WHERE isADDONPACK != 1 OR isADDONPACK IS NULL";


		}


}

$tableheader="<th><a href='wad_main.php?sorttype=wad_epi_desc&sortorder=desc".$addonpackquery."'>Name</a></th><th>Extra PWADS & Patches</th><th><a href='wad_main.php?sorttype=levels&sortorder=desc".$addonpackquery."'>Levels</a></th><th><a href='wad_main.php?sorttype=wadhiscore&sortorder=desc".$addonpackquery."'>Wad Hi Score</a></th><th><a href='wad_main.php?sorttype=playername&sortorder=desc".$addonpackquery."'>Hi Score Player Name</a></th><th><a href='wad_main.php?sorttype=gmt_timestamp&sortorder=desc".$addonpackquery."'>Set At (GMT)</a></th>";

if((isset($_GET['sorttype'])&&$_GET['sorttype']!="")&&(isset($_GET['sortorder'])&&$_GET['sortorder']!="")){

	$sorttype=$_GET['sorttype'];
	$sortorder=$_GET['sortorder'];

	if($sorttype=="wad_epi_desc"||$sorttype=="playername"){
		$extrasql.=" ORDER BY LOWER(".$sorttype.")";
	}else{
		$extrasql.=" ORDER BY ".$sorttype;
	}

	if($sortorder=="desc"){


		$tableheader=str_replace("sorttype=".$sorttype."&sortorder=desc","sorttype=".$sorttype."&sortorder=asc",$tableheader);

		$extrasql.=" DESC";

		$sortquery="sorttype=".$sorttype."&sortorder=desc";

	}else{
		//desc: this will never be called the way it works now...
		str_replace("sorttype=".$sorttype."&sortorder=asc","sorttype=".$sorttype."&sortorder=desc",$tableheader);

		$extrasql.=" ASC";

		$sortquery="sorttype=".$sorttype."&sortorder=asc";


	}


}


//echo $extrasql;


if (!mysql_connect($db_host, $db_user, $db_pwd))
   die("Can't connect to database");

if (!mysql_select_db($database))
   die("Can't select database");

// sending query
$result = mysql_query("SELECT * FROM {$table}".$extrasql);
if (!$result) {
   die("Query to show fields from table failed");
}

$fields_num = mysql_num_fields($result);

echo "<a href='http://www.scoredoom.com'><img height='30px;' border=0 src='images/scoredoom.gif'/></a><br><span style='color:#93DB70;font-size:16pt;font-weight:bold;'>Pistol Start Mode</span><br><span style='color: rgb(156, 156, 222); font-weight: bold; font-size: 13pt;'><span style='color: red;'>Hardcore</span> w. <span style='color: white;'>Non-Infighting</span> Mode: </span><br> <span style='color: white;font-size:13pt;font-weight:bold;'>Hi Score Table</span></div><br>(server recognized wad configurations)";
echo "<table border='1' class='hiscoreTable'><tr>";
// printing table headers

echo $tableheader; //"<th>Name</th><th>Extra PWADS & Patches</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";
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
 		$wadhiscore="";//"<span style='color:#FF2400'>0</span>";

 	}else{
 		$wadhiscore=$row['wadhiscore'];

 	}

 		if($row['wadhiscore2']==0){
 			$wadhiscore2="";//"<span style='color:#FF2400'>0</span>";

 		}else{
 			$wadhiscore2=$row['wadhiscore2'];

 		}


 	if($row['wadhiscore3']==0){
 		$wadhiscore3=""; //"<span style='color:#FF2400'>0</span>";

 	}else{
 		$wadhiscore3=$row['wadhiscore3'];

	}

   echo "<tr style='background-color:".$bgcolor."';>";

	echo "<td class='lvlslink'><a href='wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&addonpack_hash=".urlencode($row['addonpack_hash'])."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";
	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."<br><span style='color:silver' class='smalltext'>".$wadhiscore2."</span><br><span style='color:#A67D3D' class='smalltext'>".$wadhiscore3."</span></td><td class='player'><a href='http://www.scoredoom.com/profile.php?playername=".urlencode($row['playername'])."' target=new title='Click here to see ".$row['playername']."&#39;s profile.'>".$row['playername']."</a><br><a class='smalltext' href='http://www.scoredoom.com/profile.php?playername=".urlencode($row['playername2'])."' target=new title='Click here to see ".$row['playername2']."&#39;s profile.'>".$row['playername2']."</a><br><a class='smalltext' href='http://www.scoredoom.com/profile.php?playername=".urlencode($row['playername3'])."' target=new title='Click here to see ".$row['playername3']."&#39;s profile.'>".$row['playername3']."</a></td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";



   // $row is array... foreach( .. ) puts every element
   // of $row to $cell variable
  // foreach($row as $cell)
      // echo "<td>$cell</td>";

   echo "</tr>\n";
}
mysql_free_result($result);
?>
</table>
<br>
<a class="back" href="#" onclick="window.close();">close</a><!--&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;<a class="back" href="wad_main_public.php">view public <span style='font-style:italic;'>Non-Infighting</span> hardcore hi score table</a>-->
</center>
<div style="position:absolute;top:15px;left:15px;text-align:center;width:360px;">
<center>
<table CELLPADDING=0 CELLSPACING=0><tr><td ALIGN=RIGHT>
<a id="hscontrols" href="wad_main.php?<?php echo $sortquery; ?>">View all</a></td><td>&nbsp;&nbsp;|&nbsp;&nbsp;</td>
<td ALIGN=LEFT><a id="hscontrols" href="wad_main.php">Default View</a>
</td></tr><tr><td>
<a id="hscontrols" href="wad_main.php?<?php echo $sortquery; ?>&addonpack=0">Regular Wads</a></td><td>&nbsp;&nbsp;|&nbsp;&nbsp;</td>
<td><a id="hscontrols" href="wad_main.php?<?php echo $sortquery; ?>&addonpack=1">ADDONPACK Wads</a>
</td></tr></table>
</center>
</div>
<div style="position:absolute;top:60px;left:0px;text-align:center;width:360px;">
<a id="hscontrols" href="#" onclick="window.close();">Close</a>
</div>


<div style="position:absolute;top:15px;right:20px;width:285px;text-align:center;font-size:8pt;color:white;">
<i>All non-commercial wads may be downloaded from <a href="http://www.doomworld.com/idgames/">Doomworld</a>,
and <a href="http://www.doomwadstation.com/">Doom Wad Station</a>, unless the name has a
<span style="color:green;text-decoration:underline">d/l</span> link next to it. You must use that version of the wad.</i>

</div>


</body></html>