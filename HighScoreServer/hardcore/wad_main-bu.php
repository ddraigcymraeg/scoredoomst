<html><head>
<link rel="stylesheet" type="text/css" href="css/styles.css" />
<title>wad_main</title>


</head><body><center>
<?php
$db_host = 'localhost';
$db_user = 'ddraig_admin';
$db_pwd = 'yourpwd';

$database = 'ddraig_sdhiscoresh';
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

echo "<a href='http://www.scoredoom.com'><img height='30px;' border=0 src='images/scoredoom.gif'/></a><div class='header'><span style='color:red;font-style:italic;'>Hardcore</span> Hi Score Table</div>(server recognized wad configurations)";
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
		$wadhiscore="<span style='color:#FF2400'>0</span>";

	}else{
		$wadhiscore=$row['wadhiscore'];

	}

   echo "<tr style='background-color:".$bgcolor."';>";

	echo "<td class='lvlslink'><a href='wad_main_level.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&wadtitle=".urlencode($row['wad_epi_desc'])."&levelnamesid=".$row['levelnamesid']."' title='Click here to see the ".$row['wad_epi_desc']." level hi scores'>".$row['wad_epi_desc'].$dl_url."</a></td>";
	echo "<td class='smalltext'>".$row['wad_config']."</td><td>".$row['levels']."</td><td class='hiscore'>".$wadhiscore."</td><td class='player'><a href='http://www.scoredoom.com/profile.php?playername=".urlencode($row['playername'])."' target=new title='Click here to see ".$row['playername']."&#39;s profile.'>".$row['playername']."</a></td><td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";



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
<a class="back" href="http://www.scoredoom.com">back</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;<a class="back" href="wad_main_public.php">view public hardcore hi score table</a>
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
<div style="position:absolute;top:60px;left:15px;text-align:center;width:360px;">
<a id="hscontrols" href="wad_main_level_latest.php">View the Latest <span style="color:red;">Hardcore</span> Level Hi Scores Set</a>
</div>


<div style="position:absolute;top:15px;right:20px;width:285px;text-align:center;font-size:8pt;color:white;">
<i>All non-commercial wads may be downloaded from <a href="http://www.doomworld.com/idgames/">Doomworld</a>,
and <a href="http://www.doomwadstation.com/">Doom Wad Station</a>, unless the name has a
<span style="color:green;text-decoration:underline">d/l</span> link next to it. You must use that version of the wad.</i>

</div>


</body></html>