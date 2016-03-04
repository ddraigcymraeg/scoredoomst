<?php
ini_set('display_errors', '1');
error_reporting(E_ALL);




//GHK CONFIG VARS

//Allowed scoredoom client hashes for this server..
//first 3 are v2.9
$GLOBAL_HASH=array("a2404ebb3bfe29dfd4946493e3074001","b9589b6ab071a2fe52673da1de679288","096f41131c165175256c3bcd720e3cb3","b21e7fc4afd06272f62c5fe98b6c5750","b8768b5724086b59f985f1101157563f", "4f9d5947b432a3717f513823f2380f42","3297c92ee3d45d204ab05427814bca48","139fa3712b6d40f09edffbe25f96862e","8ca462d9c07e9498f816033a5117a45f","3ad934193d3709e725ac7922991a0af7","5da050820d4575c0f3fc539da4159c50","33f210bddc387ecd3e9c30e72d2487ec","df2b27e22a7c490e3c7a94002075fe2f","8509f5358656b270c6276748d31e47e6","7f4c8c4b06e7ec07b2801a61c29caa7e","75f995f89490585aad0d37907fca4a2b","f395a1cdc16b0bea4c509916cff78761","37803dc9d138a889fb58df264a023671","8bdc6d005d62025a54ce1678c36bec40","0e4ce03ed24023014ca82b5d6d081718"); //f3c4acd663b83656a4092664643733a8 //"d62f893b0122b1f66f290e5aa7ec57fc" //,"ea1a5741f5b8b48ab37cabf03aec9280","4919603cf537a2ee1daaf7737c2d6a2e","c6e736afffdd1dddc53629cf3137a62b");
//real,upx
$GLOBAL_HASH=array("c09169fcba9661fe882d0c6bf067744d","8aa537d6c0207fc081406af28a66b431"); //,"a2404ebb3bfe29dfd4946493e3074001","b9589b6ab071a2fe52673da1de679288","096f41131c165175256c3bcd720e3cb3","b21e7fc4afd06272f62c5fe98b6c5750","b8768b5724086b59f985f1101157563f", "4f9d5947b432a3717f513823f2380f42","3297c92ee3d45d204ab05427814bca48","139fa3712b6d40f09edffbe25f96862e","8ca462d9c07e9498f816033a5117a45f","3ad934193d3709e725ac7922991a0af7","5da050820d4575c0f3fc539da4159c50","33f210bddc387ecd3e9c30e72d2487ec","df2b27e22a7c490e3c7a94002075fe2f","8509f5358656b270c6276748d31e47e6","7f4c8c4b06e7ec07b2801a61c29caa7e","75f995f89490585aad0d37907fca4a2b","f395a1cdc16b0bea4c509916cff78761","37803dc9d138a889fb58df264a023671","8bdc6d005d62025a54ce1678c36bec40","0e4ce03ed24023014ca82b5d6d081718"); //f3c4acd663b83656a4092664643733a8 //"d62f893b0122b1f66f290e5aa7ec57fc" //,"ea1a5741f5b8b48ab37cabf03aec9280","4919603cf537a2ee1daaf7737c2d6a2e","c6e736afffdd1dddc53629cf3137a62b");

//echo "returnval:1:Hi Score Server Currently Down for an update. Should be back up in a few hours. Check scoredoom dot com for new updates.\n:";
//exit;

$PUBLIC_WADS=1;

$AUTO_PLAYER_ADD=1;

$SD_TABLE_ADMIN_EMAIL="";

$ALLOW_HARDCORE=1;
//



$errflag="0";
$successmsg="";
$wad_main=1; //flag used in logic to keep code cleaner
$playername=$_GET["playername"];
$pwd=$_GET["pwd"];
$globalhash=$_GET["globalhash"];
$wadhash=$_GET["wadhash"];
$levelname=$_GET["levelname"];
$wadepiname=$_GET["wadepiname"];
$patches=$_GET["patchinfo"];
$wadhiscoreset=0;
$levelhiscoreset=0;
$lvlnum=0; //lvlnum pared from levelname (doom2+: map01->01->1, map11->11, udoom: E2M3->23) used for setting valid level range)
$isCustomLvlName=0; //support for custom level names MAX is 8 chars for this lumpname (i.e. $levelname)
$addonpackhash=$_GET["addonpackhash"];
$configvalueshash=$_GET["configvalueshash"];
$isfinale=0; //used for properly displaying gold text for finale screen, sort of a hack.

if((isset($_GET["isfinale"])&&$_GET["isfinale"]==1)){

	$isfinale=1;

}

//add code to handle no-infighting.
//Allow servers to handle this, not the client, as no-infighting makes sense with scoring.

if(isset($_GET["noinfighting"])&&$_GET["noinfighting"]){
	$noinfighting=true;
	//echo "returnval:1:You must enable monster infighting.\nSet 'sd_notarget' to 'false'.\n:";
	//exit;

}else{
	$noinfighting=false;
}



if((isset($_GET["mode"])&&$_GET["mode"]>1)&&$ALLOW_HARDCORE==1){
	//hardcore database!! :-)

	$noinfighting?$dbname="ddraig_sdhiscoresnih":$dbname="ddraig_sdhiscoresh";


}else{
	//wimps database :-(
	$noinfighting?$dbname="ddraig_sdhiscoresni":$dbname="ddraig_sdhiscores";

}

$levelname=strtoupper($levelname); //need this for levelnames table cross reference




//hack for heretic episode 5, (and episode 6) and wads like 2002ado, for some reason SD/DOOM does send an |e5 in $wadepiname
//check for levels like EYMX where Y>4 and X is numeric (1->9 typically)...


//if(strstr(strtoupper($wadepiname),"HERETIC.WAD")!==FALSE){


	if(strlen($levelname)==4){

		$arr = str_split(strtoupper($levelname));

		if((is_numeric($arr[1])&&is_numeric($arr[3]))&&($arr[0]=="E"&&$arr[2]=="M")){


			if((int)$arr[1]>4){
				$wadepiname.="|e".$arr[1];

			}
		}
	}

//}

//$shunttype=1,shift from hiscore, 2=shift from silver.
function shuntPlayers($dbcnx,$dbname,$row,$shunttype,$hiscoretype,$udepi,$wadhash,$addonpackhash,$levelname){

	if($hiscoretype==1){

		if($shunttype==1){

			if($udepi!=""){

				$result2=mysql_query("UPDATE ".$dbname.".wad_main SET playername3='".$row['playername2']."',wadhiscore3=".$row['wadhiscore2']." where wadhash ='".$wadhash."' and episode ='".$udepi."' and addonpack_hash='".$addonpackhash."'");
				$result2=mysql_query("UPDATE ".$dbname.".wad_main SET playername2='".$row['playername']."',wadhiscore2=".$row['wadhiscore']." where wadhash ='".$wadhash."' and episode ='".$udepi."' and addonpack_hash='".$addonpackhash."'");
			}else{
				$result2=mysql_query("UPDATE ".$dbname.".wad_main SET playername3='".$row['playername2']."',wadhiscore3=".$row['wadhiscore2']." where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");
				$result2=mysql_query("UPDATE ".$dbname.".wad_main SET playername2='".$row['playername']."',wadhiscore2=".$row['wadhiscore']." where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

			}

		}elseif($shunttype==2){

			if($udepi!=""){
				$result2=mysql_query("UPDATE ".$dbname.".wad_main SET playername3='".$row['playername2']."',wadhiscore3=".$row['wadhiscore2']." where wadhash ='".$wadhash."' and episode ='".$udepi."' and addonpack_hash='".$addonpackhash."'");
			}else{
				$result2=mysql_query("UPDATE ".$dbname.".wad_main SET playername3='".$row['playername2']."',wadhiscore3=".$row['wadhiscore2']." where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

			}


		}
	}elseif($hiscoretype==2){

		if($shunttype==1){

			if($udepi!=""){
				$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET playername3='".$row['playername2']."',levelhiscore3=".$row['levelhiscore2']." where wadhash ='".$wadhash."' and episode ='".$udepi."'  and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");
				$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET playername2='".$row['playername']."',levelhiscore2=".$row['levelhiscore']." where wadhash ='".$wadhash."' and episode ='".$udepi."'  and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");
			}else{
				$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET playername3='".$row['playername2']."',levelhiscore3=".$row['levelhiscore2']." where wadhash ='".$wadhash."'  and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");
				$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET playername2='".$row['playername']."',levelhiscore2=".$row['levelhiscore']." where wadhash ='".$wadhash."'  and levelname='".$levelname."'  and addonpack_hash='".$addonpackhash."'");

			}


		}elseif($shunttype==2){

			if($udepi!=""){
				$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET playername3='".$row['playername2']."',levelhiscore3=".$row['levelhiscore2']." where wadhash ='".$wadhash."' and episode ='".$udepi."'  and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");
			}else{
				$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET playername3='".$row['playername2']."',levelhiscore3=".$row['levelhiscore2']." where wadhash ='".$wadhash."'  and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");

			}


		}

	}


}

function setSilverBronze($curtime,$dbcnx,$dbname,$hiscoretype,$playername,$wadhiscore,$levelhiscore,$wdhs1,$wdhs2,$lvlhs1,$lvlhs2,$udepi,$wadhash,$addonpackhash,$pname1,$pname2,$row,$levelname){

	if((int)$wadhiscore>(int)$wdhs1&&$hiscoretype==1){

					shuntPlayers($dbcnx,$dbname,$row,2,$hiscoretype,$udepi,$wadhash,$addonpackhash,$levelname);
							if($udepi!=""){

								$result2=mysql_query("UPDATE ".$dbname.".wad_main SET wadhiscore2=".$wadhiscore." , playername2='".$playername."' where wadhash ='".$wadhash."' and episode ='".$udepi."' and addonpack_hash='".$addonpackhash."'");


							}else{

								$result2=mysql_query("UPDATE ".$dbname.".wad_main SET wadhiscore2=".$wadhiscore." , playername2='".$playername."' where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

							}
							if (!$result2) {
								echo "returnval:1:Error updating the hi score server (wad_main)\n:";
								mysql_close($dbcnx);
								exit;

							}

							return 2;

	}elseif((int)$wadhiscore>(int)$wdhs2&&$hiscoretype==1){

							if($udepi!=""){

								$result2=mysql_query("UPDATE ".$dbname.".wad_main SET wadhiscore3=".$wadhiscore." , playername3='".$playername."' where wadhash ='".$wadhash."' and episode ='".$udepi."' and addonpack_hash='".$addonpackhash."'");


							}else{

								$result2=mysql_query("UPDATE ".$dbname.".wad_main SET wadhiscore3=".$wadhiscore." , playername3='".$playername."' where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

							}
							if (!$result2) {
								echo "returnval:1:Error updating the hi score server (wad_main)\n:";
								mysql_close($dbcnx);
								exit;

							}

							return 3;


	}

	if((int)$levelhiscore>(int)$lvlhs1&&$hiscoretype==2){

					shuntPlayers($dbcnx,$dbname,$row,2,$hiscoretype,$udepi,$wadhash,$addonpackhash,$levelname);

						if($udepi!=""){
							$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET levelhiscore2=".$levelhiscore." , playername2='".$playername."' where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");
						}else{
							$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET levelhiscore2=".$levelhiscore." , playername2='".$playername."' where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");

						}


						if (!$result2) {
							echo "returnval:1:Error updating the hi score server (wad_main_level)\n:";
							mysql_close($dbcnx);
							exit;

						}
						return 2;


	}elseif((int)$levelhiscore>(int)$lvlhs2&&$hiscoretype==2){



						if($udepi!=""){
							$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET levelhiscore3=".$levelhiscore." , playername3='".$playername."' where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");
						}else{
							$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET levelhiscore3=".$levelhiscore." , playername3='".$playername."' where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");



						}


						if (!$result2) {
							echo "returnval:1:Error updating the hi score server (wad_main_level)\n:";
							mysql_close($dbcnx);
							exit;

						}

						return 3;


	}




}

function getlvlnum($lvlname,$epi) {

	$retval="";


	$arr = str_split($lvlname);

	if($epi!=""){ //udoom
		$retval=$arr[1].$arr[3];

	}else{
		$retval=$arr[3].$arr[4];

	}

	return (int)$retval; //return as an integer



}




//to hold current db vals...
$dbwadplayer="";
$dbwadhiscore=0;
$dblevelplayer="";
$dblevelhiscore=0;


//get gmt_timestamp
$curtime=time();
$utc_str = gmdate("M d Y H:i:s", time());


//SHOULD PARSE OUT non-alpha numerics int the pwd and player names.
//or return an error at least with explanation.



//check that the globalhash is ok

//if(array_search($globalhash, $GLOBAL_HASH)===FALSE){
	//echo "returnval:1:Your version of scoredoom is not supported by the hi score server\nPlease grab the latest version from www.scoredoom.com\n:";
	//exit;
//}




//check db connection
$dbcnx = mysql_connect("localhost:3306", "ddraig_admin", "yourpwd");


if (!$dbcnx) {
  	echo "returnval:1:Problems connecting to the hi score server database\n:";
  	mysql_close($dbcnx);
	exit;
}



//check player table

//dont allow blank passwords
if(strlen(trim($pwd))==0){
	echo "returnval:1:Invalid password logging into the hi score server\n:";
	mysql_close($dbcnx);
	exit;
}

//check for maxmimum password size (can be set in zdoom.cfg)
if(strlen($pwd)>15){
	echo "returnval:1:Invalid password logging into the hi score server. Passwords greater than 15 characters not permitted.\n:";
	mysql_close($dbcnx);
	exit;
}



//only deal with main regular database when working with player data.
$dbname2="ddraig_sdhiscores"; //ddraig_sdhiscores


$result = mysql_query("SELECT * FROM ".$dbname2.".players where playername ='".$playername."'");


if (!mysql_num_rows($result)) {

	//allow player to be added.
	if($AUTO_PLAYER_ADD==1){

		if(!mysql_query("INSERT INTO ".$dbname2.".players (playername,pwd,enabled,gmt_timestamp) VALUES ('".$playername."','".$pwd."',1,".$curtime.")")){
			echo "returnval:1:Error inserting new player into the hi score server database\n:";
			mysql_close($dbcnx);
			exit;

		}else{
			$successmsg.="Player name added to hi score server\n";

		}


	}else{
		if(isset($SD_TABLE_ADMIN_EMAIL)&&$SD_TABLE_ADMIN_EMAIL!=""){
			echo "returnval:1:Player name not found on the hi score server.You will need to be added by the server admin (".$SD_TABLE_ADMIN_EMAIL.")\n:";
			mysql_close($dbcnx);
			exit;
		}else{
			echo "returnval:1:Player name not found on the hi score server. You will need to be added by the server admin\n:";
			mysql_close($dbcnx);
			exit;

		}
	}


}else{
//found player
//should only ever have 1 row...

	while ( $row = mysql_fetch_array($result) ) {

		if($row["pwd"]!=$pwd){
			echo "returnval:1:Player password not valid on the hi score server\n:";
			mysql_close($dbcnx);
			exit;
		}else if($row["enabled"]==0){
			echo "returnval:1:Player name disabled on the hi score server\n:";
			mysql_close($dbcnx);
			exit;

		}

	}


}




//Player is validated, continue...

//check if we have an Ultimate Doom wad and get episode, since we need
//to key off of wadhash AND episode number for Ultimate Doom

$udepi="";

//check for '|e' string near the end of the $wadepiname string var
//where for udoom wads the last 3 chars are '|eX' X=1,2,3,4

if(substr($wadepiname,strlen($wadepiname)-3,2)=="|e"){
	$udepi=substr($wadepiname,strlen($wadepiname)-2,3); //get e1,e2 etc...
}

//add support for custom level names, not of the format MAPxx and ExMx
//This is important as there is code which expects this format in wad_main
//for levelfirst,levellast,levelsecret1,levelsecret2.
//set a flag if not in either of these formats.
//For instance some newer wads will send level names like 'TITLE','MAPX0' and 'DOWNTOWN'

if(strlen($levelname)==5){ //check for doom2
	$ar=str_split(strtoupper($levelname),3);

	if(!($ar[0]=="MAP"&&is_numeric($ar[1])))
		$isCustomLvlName=1;




}else if(strlen($levelname)==4){ //check for udoom
	$ar=str_split(strtoupper($levelname));


	if(!(is_numeric($arr[1])&&is_numeric($arr[3]))&&($arr[0]=="E"&&$arr[2]=="M"))
		$isCustomLvlName=1;



}else{ //obv. a custom level name, or something funky similar to 'TITLEMAP'
	$isCustomLvlName=1;
}

if($addonpackhash!="0"){

	//only deal with main regular database when working with addonpack hash data.



	$result = mysql_query("SELECT * FROM ".$dbname2.".addonpack_main where addonpack_hash ='".$addonpackhash."'");


	if (!mysql_num_rows($result)) {

		echo "returnval:1:Add-On Pack not supported by the hi score server.\n:";
		mysql_close($dbcnx);
		exit;



	}else{
		$result37 = mysql_query("SELECT * FROM ".$dbname2.".addonpack_main where addonpack_hash ='".$addonpackhash."' and addonpack_configvaluehash='".$configvalueshash."'");

		if (!mysql_num_rows($result37)) {

				echo "returnval:1:Add-On Pack is OK, but hi score server requires \ndefault configuration settings.\n:";
				mysql_close($dbcnx);
				exit;



		}

	}

}


//Check if we are getting hi scores or setting hi scores.
if(isset($_GET["wadhiscore"])){




	//setting
	$wadhiscore=$_GET["wadhiscore"];
	$levelhiscore=$_GET["levelhiscore"];

	//CHECK for TITLEMAP level which can be sent over
	//and ignore, but fool the client into thinking
	//everything is ok
	if($levelname=="TITLEMAP"){
		echo "returnval:0:".$successmsg.":0:0:";
		mysql_close($dbcnx);
		exit;

	}


	//SQL injection check:
	//if(!is_numeric($wadhiscore)||!is_numeric($levelhiscore)){
		//echo "returnval:1:Non numeric scores sent to the hi score server\n:";
		//mysql_close($dbcnx);
		//exit;
	//}



	//check wad_main first

		if($udepi!=""){
			//udoom. Episodes have to always be added manually with lower case 'e1' etc...
			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main where wadhash ='".$wadhash."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");


		}else{
			//doom2 etc...

			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

		}


		if (!mysql_num_rows($result)) {


				//no result in main wad table, check if we can add to the public wad table
				if($PUBLIC_WADS==1){
					$wad_main=0;
					//here any old wad configuration can be added, if it is not there.

					if($udepi!=""){
							//udoom. Episodes have to always added manually with lower case 'e1' etc...
							$result2 = mysql_query("SELECT * FROM ".$dbname.".wad_main_public where wadhash ='".$wadhash."' and episode='".$udepi."'  and addonpack_hash='".$addonpackhash."'");


						}else{
							//doom2 etc...

							$result2 = mysql_query("SELECT * FROM ".$dbname.".wad_main_public where wadhash ='".$wadhash."'  and addonpack_hash='".$addonpackhash."'");

					}

					if (!mysql_num_rows($result2)) {
					//SHOULD NEVER HAVE TO DO AN INSERT WHEN TRYING TO SET A HI SCORE
					//SINCE THE DATA IS RETRIEVED WHEN THE LEVEL IS LOADED
					//SAVE GAMES??
					//add this wad config hash
					//add playernam adding as well as 0 score.
						//if(!mysql_query("INSERT INTO ".$dbname.".wad_main_public (wadhash,wad_epi_desc,wadepiname,episode,wad_config,wadhiscore,playername,addonpack_hash) VALUES ('".$wadhash."','',',".$wadepiname."','".$udepi."','',".$wadhiscore.",'".$playername."','".$addonpackhash."')")){
							echo "returnval:1:Error inserting wad configuration on the hi score server\n:";
							mysql_close($dbcnx);
							exit;

						//}else{
							//$successmsg.="New wad configuration added to the hi score server\n";
							//$dbwadplayer=$playername;
							//$dbwadhiscore="0";

						//}

					}else{
					//set the hiscore info if applicable
					//should only be 1 row ever...


							while ( $row2 = mysql_fetch_array($result2) ) {

										if($isfinale==1){
											if((strtolower($playername)==strtolower($row2["playername"])) and ((int)$wadhiscore >= (int)$row2["wadhiscore"])){
												$wadhiscoreset=1;

											}

										}


										//double check that this is ok. check int maxsize as well
										if((int)$wadhiscore > (int)$row2["wadhiscore"]){


											//now update the hi score and hi score playe name for wad in wad_main

											if($udepi!=""){
												$result3=mysql_query("UPDATE ".$dbname.".wad_main_public SET wadhiscore=".$wadhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and episode ='".$udepi."'  and addonpack_hash='".$addonpackhash."'");

											}else{
												$result3=mysql_query("UPDATE ".$dbname.".wad_main_public SET wadhiscore=".$wadhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."'  and addonpack_hash='".$addonpackhash."'");



											}
											if (!$result3) {
												echo "returnval:1:Error updating the hi score server (wad_main_public)\n:";
												mysql_close($dbcnx);
												exit;

											}else{
												$successmsg.="Wad Hi Score Set!\n";
												$wadhiscoreset=1;

											}

										}else {
											//failed to beat the wad/epi hi score

										}

							}

					}



				}else{
					if(isset($SD_TABLE_ADMIN_EMAIL)&&$SD_TABLE_ADMIN_EMAIL!=""){
						echo "returnval:1:Wad configuration not allowed on the hi score server. You must only play with wad configurations defined on the hi score server.\nTry contacting the server admin(".$SD_TABLE_ADMIN_EMAIL.")\n:";
					}else{
						echo "returnval:1:Wad configuration not allowed on the hi score server. You must only play with wad configurations defined on the hi score server.\nTry contacting the server admin\n:";

					}

					mysql_close($dbcnx);
					exit;


				}



		}else{



			//found wadhash (and udepi) in wad_main
			//should only ever be 1 row...
			while ( $row = mysql_fetch_array($result) ) {


										if($isfinale==1){
											if((strtolower($playername)==strtolower($row["playername"])) and ((int)$wadhiscore >= (int)$row["wadhiscore"])){
												$wadhiscoreset=1;

											}

										}

						//double check that this is ok. check int maxsize as well
						if((int)$wadhiscore > (int)$row["wadhiscore"]){

							shuntPlayers($dbcnx,$dbname,$row,1,1,$udepi,$wadhash,$addonpackhash,"");

							//now update the hi score and hi score player name for wad in wad_main

							if($udepi!=""){

								$result2=mysql_query("UPDATE ".$dbname.".wad_main SET wadhiscore=".$wadhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and episode ='".$udepi."' and addonpack_hash='".$addonpackhash."'");


							}else{

								$result2=mysql_query("UPDATE ".$dbname.".wad_main SET wadhiscore=".$wadhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

							}
							if (!$result2) {
								echo "returnval:1:Error updating the hi score server (wad_main)\n:";
								mysql_close($dbcnx);
								exit;

							}else{
								$successmsg.="Wad Hi Score Set!\n";
								$wadhiscoreset=1;

							}

						}else {
							//failed to beat the wad/epi hi score

							$retval=setSilverBronze($curtime,$dbcnx,$dbname,1,$playername,$wadhiscore,$levelhiscore,$row["wadhiscore2"],$row["wadhiscore3"],0,0,$udepi,$wadhash,$addonpackhash,$row["playername2"],$row["playername3"],$row,"");

							if($retval==2){
								$wadhiscoreset=2;
								$successmsg.="Wad Silver Hi Score Set!\n";

							}elseif($retval==3){
								$wadhiscoreset=3;
								$successmsg.="Wad Bronze Hi Score Set!\n";
							}


						}

				}



	}

//now set level scores, if applicable...

	if($wad_main==1){

		if($udepi!=""){
			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");
		}else{
			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level where wadhash ='".$wadhash."' and levelname='".$levelname."'  and addonpack_hash='".$addonpackhash."'");

		}

		if (!mysql_num_rows($result)) {

		//*should never have to insert into wad_main_level when setting a hi score
		//since scoredoom will try and insert when the level
		//is first loaded, when it gets the data. Throw an error instead, since clearly the load code didnt work.
		//what about save games???*

			echo "returnval:1:Error getting level information from the hi score server (wad_main_level)\n:";
			mysql_close($dbcnx);
			exit;


		}else{
			//found wadhash and levelname in wad_main_level
			//should only ever be 1 row...
			while ( $row = mysql_fetch_array($result) ) {

									if($isfinale==1){
											if((strtolower($playername)==strtolower($row["playername"])) and ((int)$levelhiscore >= (int)$row["levelhiscore"])){
												$levelhiscoreset=1;

											}

										}

					//double check that this is ok. check int maxsize as well
					if((int)$levelhiscore > (int)$row["levelhiscore"]){

						shuntPlayers($dbcnx,$dbname,$row,1,2,$udepi,$wadhash,$addonpackhash,$levelname);

						//now update the hi score and hi score player name for wad in wad_main

						if($udepi!=""){
							$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET levelhiscore=".$levelhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");
						}else{
							$result2=mysql_query("UPDATE ".$dbname.".wad_main_level SET levelhiscore=".$levelhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");

						}


						if (!$result2) {
							echo "returnval:1:Error updating the hi score server (wad_main_level)\n:";
							mysql_close($dbcnx);
							exit;

						}else{
								$successmsg.="Level Hi Score Set!\n";
								$levelhiscoreset=1;


						}

					}else {
						//failed to beat the level hi score

							$retval=setSilverBronze($curtime,$dbcnx,$dbname,2,$playername,$wadhiscore,$levelhiscore,0,0,$row["levelhiscore2"],$row["levelhiscore3"],$udepi,$wadhash,$addonpackhash,$row["playername2"],$row["playername3"],$row,$levelname);

							if($retval==2){
								$levelhiscoreset=2;
								$successmsg.="Level Silver Hi Score Set!\n";
							}elseif($retval==3){
								$levelhiscoreset=3;
								$successmsg.="Level Bronze Hi Score Set!\n";
							}



					}

			}

		}



	}else{
		//wad_main_level_public levels

				if($udepi!=""){
					$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level_public where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");


				}else{
					$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level_public where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");

				}

				if (!mysql_num_rows($result)) {


				//*should never have to insert into wad_main_level when setting a hi score
				//since scoredoom will try and insert when the level
				//is first loaded, when it gets the data. Throw an error instead, since clearly the load code didnt work.
				//what about save games???*

					echo "returnval:1:Error getting level information from the hi score server (wad_main_level_public)\n:";
					mysql_close($dbcnx);
					exit;


				}else{
					//found wadhash and levelname in wad_main_level
					//should only ever be 1 row...
					while ( $row = mysql_fetch_array($result) ) {

									if($isfinale==1){
											if((strtolower($playername)==strtolower($row["playername"])) and ((int)$levelhiscore >= (int)$row["levelhiscore"])){
												$levelhiscoreset=1;

											}

										}
							//double check that this is ok. check int maxsize as well
							if((int)$levelhiscore > (int)$row["levelhiscore"]){



								if($udepi!=""){
									$result2=mysql_query("UPDATE ".$dbname.".wad_main_level_public SET levelhiscore=".$levelhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");
								}else{
									$result2=mysql_query("UPDATE ".$dbname.".wad_main_level_public SET levelhiscore=".$levelhiscore." , playername='".$playername."',gmt_timestamp=".$curtime." where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");
								}

								if (!$result2) {
									echo "returnval:1:Error updating the hi score server (wad_main_level_public)\n:";
									mysql_close($dbcnx);
									exit;

								}else{
										$successmsg.="Level Hi Score Set!\n";
										$levelhiscoreset=1;


								}

							}else {
								//failed to beat the level hi score


							}

					}

		}

	}



//we made it this far,so we had no errors in checking/setting the hi scores, return message to scoredoom.
echo "returnval:0:".$successmsg.":".$levelhiscoreset.":".$wadhiscoreset.":";
mysql_close($dbcnx);
exit;


}else{
	//getting


	//CHECK for TITLEMAP level which can be sent over
		//and ignore, but fool the client into thinking
		//everything is ok
		if($levelname=="TITLEMAP"){
			echo "returnval:0:".$successmsg.":".$playername.":0:".$playername.":0:";
			mysql_close($dbcnx);
			exit;

	}

	//check wad_main first

	if($udepi!=""){
		//udoom. Episodes have to always added manually with lower case 'e1' etc...
		$result = mysql_query("SELECT * FROM ".$dbname.".wad_main where wadhash ='".$wadhash."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");


	}else{
		//doom2 etc...

		$result = mysql_query("SELECT * FROM ".$dbname.".wad_main where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

	}



	//no result in main wad table, check if we can add to the public wad table
	if (!mysql_num_rows($result)) {





			if($PUBLIC_WADS==1){
				$wad_main=0;
				//here any old wad configuration can be added, if it is not there.

				if($udepi!=""){
						//udoom. Episodes have to always be added manually with lower case 'e1' etc...
						$result2 = mysql_query("SELECT * FROM ".$dbname.".wad_main_public where wadhash ='".$wadhash."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");


					}else{
						//doom2 etc...

						$result2 = mysql_query("SELECT * FROM ".$dbname.".wad_main_public where wadhash ='".$wadhash."' and addonpack_hash='".$addonpackhash."'");

				}

				if (!mysql_num_rows($result2)) {
				//add this wad config hash
				//add playernam adding as well as 0 score.
					if(!mysql_query("INSERT INTO ".$dbname.".wad_main_public (wadhash,wad_epi_desc,wadepiname,episode,wad_config,wadhiscore,playername,patches,gmt_timestamp,addonpack_hash) VALUES ('".$wadhash."','','".$wadepiname."','".$udepi."','',0,'".$playername."','".$patches."',".$curtime.",'".$addonpackhash."')")){
						echo "returnval:1:Error inserting wad configuration on the hi score server\n:";
						mysql_close($dbcnx);
						exit;

					}else{

						$successmsg="New wad configuration added to the hi score server\n";
						$dbwadplayer=$playername;
						$dbwadhiscore="0";

					}

				}else{
				//get the hiscore info
				//should only be 1 row ever...
					while ( $row2 = mysql_fetch_array($result2) ) {

						$dbwadplayer=$row2["playername"];
						$dbwadhiscore=$row2["wadhiscore"];


					}


				}



			}else{
				if(isset($SD_TABLE_ADMIN_EMAIL)&&$SD_TABLE_ADMIN_EMAIL!=""){
						echo "returnval:1:Wad configuration not allowed on the hi score server. You must only play with wad configurations defined on the hi score server.\nTry contacting the server admin(".$SD_TABLE_ADMIN_EMAIL.")\n:";
				}else{
						echo "returnval:1:Wad configuration not allowed on the hi score server. You must only play with wad configurations defined on the hi score server.\nTry contacting the server admin\n:";

				}
				mysql_close($dbcnx);
				exit;


			}



	}else{
		//found wadhash (and udepi) in wad_main
		//should only ever be 1 row...
		while ( $row = mysql_fetch_array($result) ) {

					$dbwadplayer=$row["playername"];
					$dbwadhiscore=$row["wadhiscore"];

					//Now check the levelfirst and levellast fields for this wadhash
					//against the level being sent
					//They define the range of valid levels for this wadhash.
					//If not, then stop hi scoring with return message.

					//add support for wads like DVII
					//this field needs to have proper formatting
					//like uppercase comma delimited map name list: MAP01,MAP02,MAP12,MAP32 etc...
					//this actually depricates the levelfirst/levellast implementation and is
					//far better, but Im too lazy to get rid of the other implementation.
					if($row["levellist"]&&$isCustomLvlName!=1){
						$allowedmaps = array();

						$allowedmaps=explode(",",$row["levellist"]);

						//now search if $levelname is in this allowed list.

						if(array_search($levelname,$allowedmaps)===FALSE){
							echo "returnval:1:Not a valid map for this wad configuration\n:";
							exit;
						}


					}else{


						if($row["levelfirst"]&&$row["levellast"]&&$isCustomLvlName!=1){ //both fields need to be defined, and ignore if a custom level name
							$lvlnum=getlvlnum($levelname,$udepi);
							$onSecret=0;

							//look for any defined secret levels, since some wads will be
							//say MAP01->MAP20 with MAP32 as the secret level, allow for this contigency...
							//only all for 2 secret levels that should be enough for a vast majority.

							if($row["levelsecret1"]){

								if($lvlnum==getlvlnum($row["levelsecret1"],$udepi))
									$onSecret=1;


							}
							if($row["levelsecret2"]){

								if($lvlnum==getlvlnum($row["levelsecret2"],$udepi))
									$onSecret=1;

							}

							if($onSecret==0){
								$lvlfirst=getlvlnum($row["levelfirst"],$udepi);
								$lvllast=getlvlnum($row["levellast"],$udepi);

								if(!($lvlnum>=$lvlfirst&&$lvlnum<=$lvllast)){
									echo "returnval:1:Not a valid map for this wad configuration\n:";
									mysql_close($dbcnx);
									exit;

								}

							}


						}
					}


					//Due to kdizd, now also allow levellist to be defined for custom levels!!
					if($isCustomLvlName==1&&$row["levellist"]){
						$allowedmaps = array();

						$allowedmaps=explode(",",$row["levellist"]);

						//now search if $levelname is in this allowed list.

						if(array_search($levelname,$allowedmaps)===FALSE){
							echo "returnval:1:Not a valid map for this wad configuration\n:";
							exit;
						}

					}


		}



	}



	//now get level scores...

	if($wad_main==1){

		if($udepi!=""){

			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");

		}else{
			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");


		}



		if (!mysql_num_rows($result)) {
		//insert into wad_main_level

				if(!mysql_query("INSERT INTO ".$dbname.".wad_main_level (wadhash,levelname,levelhiscore,playername,episode,gmt_timestamp,addonpack_hash) VALUES ('".$wadhash."','".$levelname."',0,'".$playername."','".$udepi."',".$curtime.",'".$addonpackhash."')")){
					echo "returnval:1:Error adding new level information on the hi score server (wad_main_level)\n:";
					mysql_close($dbcnx);
					exit;

				}else{
					$successmsg="New wad level configuration added for ".$levelname." to the hi score server\n";
					$dblevelplayer=$playername;
					$dblevelhiscore="0";

				}


		}else{
			//found wadhash and levelname in wad_main_level
			//should only ever be 1 row...
			while ( $row = mysql_fetch_array($result) ) {

				$dblevelplayer=$row["playername"];
				$dblevelhiscore=$row["levelhiscore"];


			}



		}


	}else{
		//wad_main_public

		if($udepi!=""){
			$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level_public where wadhash ='".$wadhash."' and levelname='".$levelname."' and episode='".$udepi."' and addonpack_hash='".$addonpackhash."'");

		}else{
				$result = mysql_query("SELECT * FROM ".$dbname.".wad_main_level_public where wadhash ='".$wadhash."' and levelname='".$levelname."' and addonpack_hash='".$addonpackhash."'");
		}
				if (!mysql_num_rows($result)) {
				//insert into wad_main_level_public

						if(!mysql_query("INSERT INTO ".$dbname.".wad_main_level_public (wadhash,levelname,levelhiscore,playername,episode,gmt_timestamp,addonpack_hash) VALUES ('".$wadhash."','".$levelname."',0,'".$playername."','".$udepi."',".$curtime.",'".$addonpackhash."')")){
							echo "returnval:1:Error adding new level information on the hi score server (wad_main_level_public)\n:";
							mysql_close($dbcnx);
							exit;

						}else{
							$successmsg="New wad level configuration added for ".$levelname." to the hi score server\n";
							$dblevelplayer=$playername;
							$dblevelhiscore="0";

						}


				}else{
					//found wadhash and levelname in wad_main_level
					//should only ever be 1 row...
					while ( $row = mysql_fetch_array($result) ) {

						$dblevelplayer=$row["playername"];
						$dblevelhiscore=$row["levelhiscore"];


					}



		}


	}

	//we made it this far,so we had no errors in getting the hi scores, a success.
	echo "returnval:0:".$successmsg.":".$dbwadplayer.":".$dbwadhiscore.":".$dblevelplayer.":".$dblevelhiscore.":";
	mysql_close($dbcnx);
	exit;
	//returnval:0:'retmessage':'wad player name':'wadhiscore':'level player name':'levelhiscore':


}


?>