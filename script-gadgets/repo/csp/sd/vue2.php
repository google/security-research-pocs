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
			$csp = "object-src 'none'; script-src 'self'";
			break;	
	}
	echo $csp;
?>">
<link href=../../../third_party/vue/sample-app/static/css/app.831a17d7f7f8a55820ed0c1e0d1f1654.css rel=stylesheet></head>
<body>
	<!-- done with https://vuejs.org/v2/guide/installation.html#CLI 
	npm install -g vue-cli
	vue init webpack vue-start (choose runtime only - no compiler)
	cd vue-start 
	npm run build 
	dist/ has the files.
	-->
	<?php echo $_GET['inj'] ?>
	
	<div id=app></div>
	<script nonce=random type=text/javascript src=../../../third_party/vue/sample-app/static/js/manifest.08329b2bcb3d9058dc9d.js></script>
	<script nonce=random type=text/javascript src=../../../third_party/vue/sample-app/static/js/vendor.c7565d5a25692514a74b.js></script>
	<script nonce=random type=text/javascript src=../../../third_party/vue/sample-app/static/js/app.cd2bce32cc6702496bf0.js></script>
</body></html>
