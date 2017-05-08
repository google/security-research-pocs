<?php 
	if (!empty($_GET['xssfilter'])) {
		header('X-XSS-Protection: 1'); 
	} else {
		header('X-XSS-Protection: 0');
	}?>
<!doctype html>
<html lang="en">
  <head>
  	
<meta http-equiv=Content-Security-Policy content="
<?php
	if (!isset($_GET['csp'])) {
		$_GET['csp'] = 'sd';
	}
	$csp = "";
	switch ($_GET['csp']) {
		case 'sd':
			$csp = "object-src 'none'; script-src 'nonce-random' 'unsafe-inline' 'unsafe-eval' 'strict-dynamic' https: http:;";
		break;
	}
	echo $csp;
?>
">
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, minimum-scale=1, initial-scale=1, user-scalable=yes">

    <title>polymer</title>
    <meta name="description" content="polymer description">

    <!-- See https://goo.gl/OOhYW5 -->
    <link rel="manifest" href="/manifest.json">

    <script nonce=random src="bower_components/webcomponentsjs/webcomponents-lite.js"></script>

    <link rel="import" nonce=random href="dist/polymer-app.vulcanized.html">
  </head>
  <body>
    <polymer-app></polymer-app>
    XSS: <div style="background: red"><?php echo $_GET['xss'] ?> </div>
    <textarea width=90% height=300 id=text><polymer-app></polymer-app></textarea>
    <p><button id=postme>Update DOM</button></p>
    <div id=postmessage></div>
    <script nonce=random>
    	document.querySelector('#postme').onclick = function() {
    		self.postMessage(document.querySelector('#text').value, '*');
    	};
    	onmessage = function(e) {
    		document.querySelector('#postmessage').innerHTML = e.data;
    	}
    </script>
  </body>
</html>

<!-- <template is=dom-bind><template is=dom-repeat items='["afterEnd","<img src=x onerror=alert(1)>"]' sort="insertAdjacentHTML"></template></template> -->