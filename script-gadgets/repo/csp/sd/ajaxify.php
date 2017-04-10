<html>
<head>
<meta http-equiv="content-security-policy" content="script-src 'nonce-secret' 'strict-dynamic'; default-src 'self'">
<!-- jQuery -->
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js" nonce="secret"></script>

<!-- jQuery ScrollTo Plugin -->
<script src="//balupton.github.io/jquery-scrollto/lib/jquery-scrollto.js" nonce="secret"></script>

<!-- History.js -->
<script src="//browserstate.github.io/history.js/scripts/bundled/html4+html5/jquery.history.js" nonce="secret"></script>

<!-- Ajaxify -->
<script src="//rawgithub.com/browserstate/ajaxify/master/ajaxify-html5.js" nonce="secret"></script>
</head>
<body>

<!--  <a href="ajaxify.php?inj=<div class='document-script'>alert(1)</div>">Test</a> -->

XSS XSS XSS XSS
<div id="content">
 <?php
   echo $_GET["inj"];
 ?> 
</div>
XSS XSS XSS XSS

</body>
</html>
