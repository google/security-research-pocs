<?php 
  header('X-XSS-Protection: 1');
?> 
<!doctype html>
<html lang="en">

<body>

<?php echo $_GET['inj'] ?>
<script type="text/javascript" src="https://google.github.io/closure-library/source/closure/goog/base.js"></script>

</html>
