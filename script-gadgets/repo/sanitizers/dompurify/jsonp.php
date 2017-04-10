<?php
  header("Content-Type: application/javascript");
  echo $_GET["callback"]."({'foo':'bar'});";
?>
