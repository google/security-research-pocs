<?php 
		header('X-XSS-Protection: 0');
?>

<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, minimum-scale=1, initial-scale=1, user-scalable=yes">

    <!-- See https://goo.gl/OOhYW5 -->
    <link rel="manifest" href="/manifest.json">

    <script nonce=random src="https://victim.example.com/polymer-start/bower_components/webcomponentsjs/webcomponents-lite.js"></script>

    <link rel="import" nonce=random href="https://victim.example.com/polymer-start/dist/polymer-app.vulcanized.html">
  </head>
  <body>
    <polymer-app></polymer-app>
      injection: <?php echo $_GET['inj'] ?>
  </body>
</html>
