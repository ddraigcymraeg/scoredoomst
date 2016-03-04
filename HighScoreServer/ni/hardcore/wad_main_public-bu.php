<html><head>
<link rel="stylesheet" type="text/css" href="css/styles.css" />
<title>wad_main_public</title>


</head><body><center>
<?php
$db_host = 'localhost';
$db_user = 'ddraig_admin';
$db_pwd = 'yourpwd';

$database = 'ddraig_sdhiscoresh';
$table = 'wad_main_public';


$sorttype="";
$sortorder="";
$extrasql="";


$tableheader="<th><a href='wad_main_public.php?sorttype=wadepiname&sortorder=desc'>Wads & Episode</a></th><th>Patches</th><th><a href='wad_main_public.php?sorttype=wadhiscore&sortorder=desc'>Wad Hi Score</a></th><th><a href='wad_main_public.php?sorttype=playername&sortorder=desc'>Hi Score Player Name</a></th><th><a href='wad_main_public.php?sorttype=gmt_timestamp&sortorder=desc'>Set At (GMT)</a></th>";

if((isset($_GET['sorttype'])&&$_GET['sorttype']!="")&&(isset($_GET['sortorder'])&&$_GET['sortorder']!="")){

	$sorttype=$_GET['sorttype'];
	$sortorder=$_GET['sortorder'];

	if($sorttype=="wadepiname"||$sorttype=="playername"){
		$extrasql=" ORDER BY LOWER(".$sorttype.")";
	}else{
		$extrasql=" ORDER BY ".$sorttype;
	}

	if($sortorder=="desc"){


		$tableheader=str_replace("sorttype=".$sorttype."&sortorder=desc","sorttype=".$sorttype."&sortorder=asc",$tableheader);

		$extrasql.=" DESC";


		//echo $extrasql;


	}else{
		//desc: this will never be called the way it works now...
		str_replace("sorttype=".$sorttype."&sortorder=asc","sorttype=".$sorttype."&sortorder=desc",$tableheader);

		$extrasql.=" ASC";


	}


}




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

echo "<a href='http://www.scoredoom.com'><img height='25px;' border=0 src='images/scoredoom.gif'/></a><div class='header'>Public <span style='color:red;font-style:italic;'>Hardcore</span> Hi Score Table</div>(player added wad configurations)";
echo "<table border='1' class='hiscoreTable'><tr>";
// printing table headers

echo $tableheader; //"<th>Wads & Episode</th><th>Patches</th><th>Wad Hi Score</th><th>Hi Score Player Name</th><th>Set At (GMT)</th>";
echo "</tr>\n";
// printing table rows
$cnt=0;
$bgcolor="#111111";

$name="";
$wadsepi="";
$patches="";

while($row = mysql_fetch_assoc($result))
{
	$cnt+=1;

	$name="";
	$wadsepi=str_replace("|"," | ",$row['wadepiname']);

	$patches="";

	if($cnt%2==0){
		$bgcolor="#111111";
	}else{
		$bgcolor="#222222";
	}

	if($row['patches'])
		$patches=str_replace("|"," | ",$row['patches']);

	$name= $wadsepi.$patches;

	if($row['episode'])
		$episode=strtoupper($row['episode']);

	if($row['wadhiscore']==0){
		$wadhiscore="<span style='color:#FF2400'>0</span>";

	}else{
		$wadhiscore=$row['wadhiscore'];

	}

   echo "<tr style='background-color:".$bgcolor."';>";

	echo "<td class='lvlslink'><a href='wad_main_level_public.php?wadhash=".$row['wadhash']."&epi=".$row['episode']."&wadtitle=".urlencode($name)."' title='Click here to see the ".$name." level hi scores'>".$wadsepi."</a></td><td class='smalltext'>".$patches."</td>";
	echo "<td class='hiscore'>".$wadhiscore."</td><td class='player'>".$row['playername']."</td>";
	echo "<td class='smalltext'>".gmdate("M d Y H:i:s",$row['gmt_timestamp'])."</td>";


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
<a class="back" href="http://www.scoredoom.com">back</a>&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;<a class="back" href="wad_main.php">view main hardcore hi score table</a>
</center></body></html>