<?php
  header("x-xss-protection: 1");
?><html>
  <head>
    <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.6.1/angular.min.js"></script>
  </head>
  <body>
     <?php echo $_GET['inj']; ?>
  </body>
</html>
