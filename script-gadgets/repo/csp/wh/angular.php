<?php
  header("x-xss-protection: 0");
?><html>
  <head>
    <meta http-equiv=content-security-policy content="object-src 'none';script-src https://ajax.googleapis.com/ajax/libs/angularjs/1.6.1/angular.min.js;">
    <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.6.1/angular.min.js"></script>
  </head>
  <body>
     <?php echo $_GET['inj']; ?>
  </body>
</html>
