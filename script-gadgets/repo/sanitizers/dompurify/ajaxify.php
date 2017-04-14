<html>
<head>
<!-- jQuery -->
<script src="//ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.js" nonce="secret"></script>

<!-- jQuery ScrollTo Plugin -->
<script src="//balupton.github.io/jquery-scrollto/lib/jquery-scrollto.js" nonce="secret"></script>

<!-- History.js -->
<script src="//browserstate.github.io/history.js/scripts/bundled/html4+html5/jquery.history.js" nonce="secret"></script>

<!-- Ajaxify -->
<script src="//rawgithub.com/browserstate/ajaxify/master/ajaxify-html5.js" nonce="secret"></script>
<script src="../../../third_party/dompurify/purify.js"></script>
<script nonce="random">
    function getParam(name) {
      var params = location.search.slice(1).split("&");
      for (var i in params) {
        var param = params[i];
        var equalIndex = param.indexOf("=");
        var paramName = param.substring(0, equalIndex);
        var paramValue = param.substring(equalIndex + 1);

        if (paramName == name) {
          return unescape(paramValue);
        }
      }

      return undefined;
    }
  </script>
</head>
<body>

<script nonce="random">
  document.write(DOMPurify.sanitize(getParam("inj")));
</script>

</body>
</html>
