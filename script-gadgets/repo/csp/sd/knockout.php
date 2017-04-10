<meta http-equiv=content-security-policy content="script-src 'nonce-random' 'unsafe-eval' 'strict-dynamic'; ">

<script nonce="random" src="https://code.jquery.com/jquery-3.1.1.js"></script>
<script nonce="random"  src="http://knockoutjs.com/downloads/knockout-3.4.1.debug.js"></script>

<?php
  echo $_GET['inj'];
?>

<script nonce="random">
  ko.applyBindings();
</script>
