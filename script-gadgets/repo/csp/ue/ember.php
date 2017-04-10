<?php
header('x-xss-protection: 0');
?>
<meta http-equiv=content-security-policy content="default-src 'none'; script-src 'unsafe-eval' 'nonce-random';">

<?php echo $_GET['inj']; ?>

<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js" nonce=random></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/ember.js/2.10.2/ember.debug.js" nonce=random></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/ember.js/2.10.2/ember-runtime.js" nonce=random></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/ember.js/2.10.2/ember-template-compiler.js"nonce=random></script>
<script nonce=random>
  App = Ember.Application.create();
</script>
