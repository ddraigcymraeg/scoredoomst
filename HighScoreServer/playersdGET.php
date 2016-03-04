
<?
ini_set('display_errors', '1');
error_reporting(E_ALL);



$message="";
$failed=0;

if(isset($_GET['newpwd'])&&$_GET['newpwd']!=""){





	$db_host = 'localhost';
	$db_user = 'ddraig_admin';
	$db_pwd = 'yourpwd';

	$database = 'ddraig_sdhiscores';
	//$table = 'wad_main';

	if (!mysql_connect($db_host, $db_user, $db_pwd))
	   die("Can't connect to database");

	if (!mysql_select_db($database))
	   die("Can't select database");


		//if playername does not exist yet, create, and
		//create a new pwd, then exit.
	 	$result = mysql_query("select * from players where playername='".$_GET["playername"]."'");

		if (!mysql_num_rows($result)) {

			//$message="Player name and password not found.";

			//$failed=1;
			//mysql_free_result($result);

			//get gmt_timestamp
			$curtime=time();


			if(!mysql_query("INSERT INTO players (playername,pwd,enabled,gmt_timestamp) VALUES ('".$_GET["playername"]."','".$_GET['newpwd']."',1,".$curtime.")")){
				echo "returnval:1:Error inserting new player into the hi score server database\n:";
				//mysql_close($database);
				exit;

			}else{

				$successmsg="Player name ".$_GET["playername"]." and password added to the hi score server\n";

				echo "returnval:0:".$successmsg."\n:";

				//mysql_close($database);
				exit;
			}



		}



	//playername exists, so try a password change...

	if(isset($_GET['oldpwd'])&&$_GET['oldpwd']!=""){ //lookup player with former password to verify
		$result = mysql_query("select * from players where pwd='".$_GET['oldpwd']."' AND playername='".$_GET["playername"]."'");

		if (!mysql_num_rows($result)) {

			$message="Player name and password not found";

			$failed=1;
			//mysql_free_result($result);
			echo "returnval:1:".$message."\n:";
			//mysql_close($database);
			exit;

		}


	}else{

		$failed=1;
		$message="Player old password cannot be empty";
		echo "returnval:1:".$message."\n:";
		//mysql_close($database);
		exit;


	}

	if($failed!=1){
		// sending query
		$result = mysql_query("UPDATE players SET pwd='".$_GET['newpwd']."' where pwd='".$_GET['oldpwd']."' AND playername='".$_GET["playername"]."'");
		if (!$result) {
		   $message="Update Password failed.";
			echo "returnval:1:".$message."\n:";


		}
		//mysql_free_result($result);
		//mysql_close($database);
		//exit;
	}

	if($failed==1){
		//echo "<br><p class='style1'>Could not set a new password: <i style='color:#FF2400;font-weight:bold;'>".$message."</i><br><br><input type=button value='Back' onClick='history.go(-1)'></p>";
		$message="Could not set a new password for".$_GET["playername"];
		echo "returnval:1:".$message."\n:";
		//mysql_close($database);
		exit;

	}else{

		//echo "<br><p class='style1'>Successfully set new password.<br><br><input type=button value='Back' onClick='history.go(-1)'></p>";

		$message="New Password set for".$_GET["playername"];
		echo "returnval:0:".$message."\n:";

		//mysql_close($database);
		exit;
	}

}else{

	$message="Player new password cannot be empty";
	echo "returnval:1:".$message."\n:";
	exit;

}

echo "returnval:1:WTF!!!\n:";
?>
