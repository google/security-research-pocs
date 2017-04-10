<head>
</head>

<script nonce="random-secret" src="https://cdnjs.cloudflare.com/ajax/libs/underscore.js/1.8.3/underscore.js"></script>

<?php
  echo $_GET["inj"];
?>

<div type="underscore/template">Hi</div>
<div id="result">

</div>

<script nonce="random-secret">
 document.getElementById("result").innerHTML =
     _.template(document.querySelector("[type='underscore/template']").innerText)();
</script>

