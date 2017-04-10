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
			$csp = "object-src 'none'; script-src 'nonce-random' 'strict-dynamic' https: http:;";
			break;
		case 'ue':
			$csp = "object-src 'none'; script-src 'nonce-random' 'unsafe-eval' 'self'";
			break;
		case 'wh':
			$csp = "object-src 'none'; script-src 'self' https://google.github.io";
			break;
	}
	echo $csp;
?>
">
</head>

<body>

<?php echo $_GET['inj'] ?>
<script nonce="random" type="text/javascript" src="https://google.github.io/closure-library/source/closure/goog/base.js"></script>

</html>
