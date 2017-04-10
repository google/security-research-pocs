<meta http-equiv=content-security-policy content="script-src 'unsafe-eval' 'self' https://code.jquery.com http://knockoutjs.com; ">

<script src="https://code.jquery.com/jquery-3.1.1.js"></script>
<script src="http://knockoutjs.com/downloads/knockout-3.4.1.debug.js"></script>
<div data-bind="html:location.hash"></div>


<?php
  echo $_GET['inj'];
?>
knockoutjs requires unsafe-eval.
<script src=knockout_init.js></script>