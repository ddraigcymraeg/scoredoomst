<html><head>
<link rel="stylesheet" type="text/css" href="css/styles.css" />
<title>Player Password Reset</title>

<SCRIPT language="javascript">

function checkform(){


if(document.getElementById('playername').value==""||document.getElementById('oldpwd').value==""
||document.getElementById('newpwd').value==""||document.getElementById('newpwd2').value==""){

	alert('Invalid Form Entry');
	return false;

}


if(document.getElementById('newpwd').value!=document.getElementById('newpwd2').value){
	alert('New Password Fields Are Different');
	return false;

}


return true;


}
</SCRIPT>

 <style type="text/css">

	<!--

	.style1 {font-size: 14pt;color:white;color:rgb(255, 218, 185);}

	.style2 {font-size: 12pt;color:white;color:rgb(255, 218, 185);}

	.style2b {font-size: 12pt;color:white;font-weight:bold;color:rgb(255, 218, 185);}

	.indent {left:10px;position:relative;}

	.emph {color:rgb(255, 192, 203);}

	table {background-color:white;border-width:0px;border-color:black;border-style:outset;border-bottom:0px;color:black;font-weight:bold;}

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



</head><body style="font-family:Verdana,Arial,Helvetica,sans-serif;font-size:75%;"><center>

<?php
$message="";
$failed=0;

if(isset($_POST['newpwd'])&&$_POST['newpwd']!=""){





	$db_host = 'localhost';
	$db_user = 'ddraig_admin';
	$db_pwd = 'yourpwd';

	$database = 'ddraig_sdhiscores';
	//$table = 'wad_main';

	if (!mysql_connect($db_host, $db_user, $db_pwd))
	   die("Can't connect to database");

	if (!mysql_select_db($database))
	   die("Can't select database");


	if(isset($_POST['oldpwd'])&&$_POST['oldpwd']!=""){ //lookup player with former password to verify
		$result = mysql_query("select * from players where pwd='".$_POST['oldpwd']."' AND playername='".$_POST["playername"]."'");

		if (!mysql_num_rows($result)) {

			$message="Player name & password not found.";

			$failed=1;
			mysql_free_result($result);
		}


	}else{
		$message="Player not valid.";
		$failed=1;

	}

	if($failed!=1){
		// sending query
		$result = mysql_query("UPDATE players SET pwd='".$_POST['newpwd']."' where pwd='".$_POST['oldpwd']."' AND playername='".$_POST["playername"]."'");
		if (!$result) {
		   $message="Update failed.";
			$failed=1;
		}
		mysql_free_result($result);
	}

	if($failed==1){
		echo "<br><p class='style1'>Could not set a new password: <i style='color:#FF2400;font-weight:bold;'>".$message."</i><br><br><input type=button value='Back' onClick='history.go(-1)'></p>";


	}else{

		echo "<br><p class='style1'>Successfully set new password.<br><br><input type=button value='Back' onClick='history.go(-1)'></p>";


	}

}else{
	echo "<h1 class='header2'>ScoreDoom Hi Score Server Player Password Reset</h1>";
	echo '<center><br><span style="color:#CE0000;font-size:12px;font-style:italic;font-weight:bold;">(Use this form to set your new password on the hi score server, after you set it on the ScoreDoom client)</span><br><br><a href="upload.php">Player Profile Upate Page </a>&nbsp;|&nbsp;<a href="index.html">Home</a></center>';
	echo "<form action='player.php' method='POST' onsubmit='javascript:return checkform(this);'>";
	echo "<table class='hiscoreTable' style='width:400;'><tr><td align=right>Player Name: </td><td> <input type='text' id='playername' name='playername' value='' length='16'/></td></tr>";
	echo "<tr><td align=right>Current Password: </td><td> <input name='oldpwd' id='oldpwd' type='password' value='' length='16'/></td></tr>";
	echo "<tr><td align=right>New Password: </td><td> <input name='newpwd' id='newpwd' type='password' value='' length='16'/></td></tr>";
	echo "<tr><td align=right>Confirm New Password: </td><td> <input name='newpwd2' id='newpwd2' type='password' value='' length='16'/></td></tr>";
	echo "<tr><td align=center colspan=2><input name='SUBMIT' type='submit' value='SUBMIT'/></td></tr></table>";
	echo "</form>";

}

?>
<br>
</center>
<div style="position:absolute;top:45px;left:0px;text-align:center;width:360px;">
<a id="hscontrols" href="#" onclick="window.close();">Close</a>
</div>
</body></html>