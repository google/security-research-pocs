<head>
<meta http-equiv=content-security-policy 
content="object-src 'none';script-src 'unsafe-eval' 'nonce-random-secret' ; default-src 'none'">
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

<pre>Notes:
 - Unsafe-eval is needed: https://github.com/jashkenas/underscore/blob/master/underscore.js#L1588
 - Syntax resembles ERB
 - Exploitable by hijacking the selector
 - Doc: http://underscorejs.org/#template
</pre>
