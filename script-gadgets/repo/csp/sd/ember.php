
<?php
  header("X-XSS-Protection: 0");
?>
<meta http-equiv=content-security-policy content="script-src 'nonce-random' 'unsafe-eval' 'strict-dynamic'; default-src 'none'">


<script nonce="random" src="http://ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
<script nonce="random" src="https://cdnjs.cloudflare.com/ajax/libs/ember.js/2.10.2/ember.prod.js"></script>
<script nonce="random" src="https://cdnjs.cloudflare.com/ajax/libs/ember.js/2.10.2/ember-runtime.js"></script>
<script nonce="random" src="https://cdnjs.cloudflare.com/ajax/libs/ember.js/2.10.2/ember-template-compiler.js"></script>


<?php
  echo $_GET["inj"];
?>



<script nonce="random">
  App = Ember.Application.create();
</script>
