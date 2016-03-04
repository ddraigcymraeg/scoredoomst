<?
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(-1);
//$balanceduri =1, or 2 if dashboard request
$API_KEY_SECRET="";
function makeRequest($url, $params=NULL, $cookieparams=null, $ch=null,$balanceduri=null) {
	global $API_KEY_SECRET;
    if (!$ch) {
      $ch = curl_init();
    }


    $opts = array(
    CURLOPT_CONNECTTIMEOUT => 10,
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_TIMEOUT        => 60,
    CURLOPT_SSL_VERIFYPEER        => FALSE,
    );



   // if ($this->useFileUploadSupport()) {
      //$opts[CURLOPT_POSTFIELDS] = $params;
  //  } else {

	if($balanceduri){
		$opts[CURLOPT_USERPWD] = $API_KEY_SECRET;
		
		if($balanceduri==2){
			//$bodyData = array (
				//'json' => json_encode($params)
			//);
			//$bodyStr = http_build_query($bodyData);
			curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);

			//curl_setopt($ch, CURLOPT_VERBOSE, TRUE);
			//$curlPath = __DIR__.DIRECTORY_SEPARATOR.'curl.txt';
			//$fp = fopen("/var/www/vhosts/jobbidding.com/httpdocs/curl.txt", "w+"); 
			//curl_setopt($ch, CURLOPT_STDERR, $fp); 
			curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
			curl_setopt($ch, CURLOPT_POSTFIELDS,json_encode($params));
			curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));  
			//curl_setopt($ch, CURLOPT_HTTPHEADER, array('Accept: application/json'));  
			//curl_setopt($ch, CURLOPT_BINARYTRANSFER, TRUE);  
			//curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json', 'Content-Length: ' . strlen(json_encode($params))));
			//curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/x-www-form-urlencoded','Content-Length: '.strlen($bodyStr)));
			//curl_setopt($ch, CURLOPT_POST, 1);
			//curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($params));  
			//curl_setopt($ch, CURLOPT_POSTFIELDS, array('json'=>json_encode($params)));
			//curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($params));
			
			//print_r(json_encode($params));

		}else{
			if($params)
				$opts[CURLOPT_POSTFIELDS] = http_build_query($params, null, '&');
				
				  // disable the 'Expect: 100-continue' behaviour. This causes CURL to wait
    // for 2 seconds if the server does not support this header.
  
		   if (isset($opts[CURLOPT_HTTPHEADER])) {
			  $existing_headers = $opts[CURLOPT_HTTPHEADER];
			  $existing_headers[] = 'Expect:';
			  $opts[CURLOPT_HTTPHEADER] = $existing_headers;
			} else {
			  $opts[CURLOPT_HTTPHEADER] = array('Expect:');
			}

		
		}
			
	}else{

		if($params)
			$opts[CURLOPT_POSTFIELDS] = http_build_query($params, null, '&');
			
				  // disable the 'Expect: 100-continue' behaviour. This causes CURL to wait
		// for 2 seconds if the server does not support this header.
  
		   if (isset($opts[CURLOPT_HTTPHEADER])) {
			  $existing_headers = $opts[CURLOPT_HTTPHEADER];
			  $existing_headers[] = 'Expect:';
			  $opts[CURLOPT_HTTPHEADER] = $existing_headers;
			} else {
			  $opts[CURLOPT_HTTPHEADER] = array('Expect:');
			}		
	
	}


    //}
    $opts[CURLOPT_URL] = $url;

    if($cookieparams)
		$opts[CURLOPT_COOKIE]=$cookieparams;




    // disable the 'Expect: 100-continue' behaviour. This causes CURL to wait
    // for 2 seconds if the server does not support this header.
  /* 
   if (isset($opts[CURLOPT_HTTPHEADER])) {
      $existing_headers = $opts[CURLOPT_HTTPHEADER];
      $existing_headers[] = 'Expect:';
      $opts[CURLOPT_HTTPHEADER] = $existing_headers;
    } else {
      $opts[CURLOPT_HTTPHEADER] = array('Expect:');
    }
*/
 	
//curl_setopt($ch, CURLINFO_HEADER_OUT, true);  //debug

//print_r($opts[CURLOPT_HTTPHEADER]); //debug

    curl_setopt_array($ch, $opts);
    $result = curl_exec($ch);
	
	// if (curl_errno($ch)) {
       // print curl_error($ch);

    if ($result === false) {

		 //echo curl_errno($ch)." : ".curl_error($ch);
    }
	;
	//print_r(curl_getinfo($ch,CURLINFO_HEADER_OUT)); //debug
    curl_close($ch);
    return $result;
  }

$data = makeRequest( "http://199.168.96.146:78/dayz/test.php", NULL);
print_r($data);
?>