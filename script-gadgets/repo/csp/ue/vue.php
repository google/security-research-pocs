<meta http-equiv=content-security-policy content="script-src 'nonce-random' 'unsafe-eval'  https://unpkg.com/; default-src 'none'">

<script src="https://unpkg.com/vue/dist/vue.js"></script>

<?php
  echo $_GET['inj'];
?>

<div id="app-4">
 Hi
</div>
<script nonce="random">
var app4 = new Vue({
  el: '#app-4',
  data: {}
})
</script>
