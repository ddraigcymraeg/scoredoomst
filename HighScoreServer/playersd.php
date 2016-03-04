
<?
ini_set('display_errors', '1');
error_reporting(E_ALL);



$message="";
$failed=0;

if(isset($_POST['newpwd'])&&$_POST['newpwd']!=""){

$pwd=$_POST['newpwd'];
//dont allow blank passwords
if(strlen(trim($pwd))==0){
	echo "returnval:1:Invalid password for the hi score server\n:";
	//mysql_close($dbcnx);
	exit;
}

//check for maxmimum password size (can be set in zdoom.cfg)
if(strlen($pwd)>15){
	echo "returnval:1:Invalid password for the hi score server.\n Passwords larger than 15 characters not permitted.\n:";
	//mysql_close($dbcnx);
	exit;
}


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
	 	$result = mysql_query("select * from players where playername='".$_POST["playername"]."'");

		if (!mysql_num_rows($result)) {

			//$message="Player name and password not found.";

			//$failed=1;
			//mysql_free_result($result);

			//get gmt_timestamp
			$curtime=time();


			if(!mysql_query("INSERT INTO players (playername,pwd,enabled,gmt_timestamp) VALUES ('".$_POST["playername"]."','".$_POST['newpwd']."',1,".$curtime.")")){
				echo "returnval:1:Error inserting new player into the hi score server database\n:";
				//mysql_close($database);
				exit;

			}else{

				$successmsg="Player name ".$_POST["playername"]." and password added to the hi score server\n";

				echo "returnval:0:".$successmsg."\n:";

				//mysql_close($database);
				exit;
			}



		}



	//playername exists,
	//so if the NEW password is the same as the current, then exit ok, allowing the client
	//to set it locally ok.
	//This will be the case with returning players with new/fresh installs

	$row = mysql_fetch_array($result);

		if($row['pwd']==$_POST['newpwd']){

			$successmsg="Player name ".$_POST["playername"]." password successfully recognized";

			echo "returnval:0:".$successmsg."\n:";
			exit;

		}



	//else try a password change using the old password...
	//the case with users with valid old passwords who want to change to the NEW pasword




	if(isset($_POST['oldpwd'])&&$_POST['oldpwd']!=""){ //lookup player with former password to verify
		$result = mysql_query("select * from players where pwd='".$_POST['oldpwd']."' AND playername='".$_POST["playername"]."'");

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
		$result = mysql_query("UPDATE players SET pwd='".$_POST['newpwd']."' where pwd='".$_POST['oldpwd']."' AND playername='".$_POST["playername"]."'");
		if (!$result) {
		   $message="Update Password failed.";
			echo "returnval:1:".$message."\n:";
			$failed=1;

		}
		//mysql_free_result($result);
		//mysql_close($database);
		//exit;
	}

	if($failed==1){
		//echo "<br><p class='style1'>Could not set a new password: <i style='color:#FF2400;font-weight:bold;'>".$message."</i><br><br><input type=button value='Back' onClick='history.go(-1)'></p>";
		$message="Could not set a new password for ".$_POST["playername"];
		echo "returnval:1:".$message."\n:";
		//mysql_close($database);
		exit;

	}else{

		//echo "<br><p class='style1'>Successfully set new password.<br><br><input type=button value='Back' onClick='history.go(-1)'></p>";

		$message="New Password set for" .$_POST["playername"];
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
