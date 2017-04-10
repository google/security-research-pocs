<meta http-equiv=content-security-policy content="style-src code.jquery.com; script-src 'nonce-random' 'unsafe-eval'; ">

<script nonce="random"  src="http://knockoutjs.com/downloads/knockout-3.4.1.debug.js"></script>

<?php
  echo $_GET["inj"];
?>

<div data-bind="value:'bar'"></div>
<script nonce="random">
  ko.applyBindings();
</script>
