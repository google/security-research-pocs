<?php 
		header('X-XSS-Protection: 1'); 
?>
<!doctype html>
<html lang="en">
  <body>
  <?php echo $_GET['inj'] ?>
  
  <script src="https://ajax.googleapis.com/ajax/libs/dojo/1.12.2/dojo/dojo.js"></script>
	<script >
		require([
			"dojo/parser",
			"dijit/form/Button",
			"dojo/domReady!"
		], function(parser){
			parser.parse();
		});
	</script>
  	Dojo
  	<?php echo $_GET['inj_post'] ?>

  
