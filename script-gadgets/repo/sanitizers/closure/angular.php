<head>
    <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.6.1/angular.min.js"></script>


  <script nonce="random" src=closure-library/closure/goog/base.js></script>
  <script nonce="random" src="data:,goog.require('goog.html.sanitizer.HtmlSanitizer');goog.require('goog.html.SafeHtml');"></script>
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
<body ng-app>
	
<script nonce="random">
  var sanitizer = new goog.html.sanitizer.HtmlSanitizer.Builder().build();
  document.write(goog.html.SafeHtml.unwrap(sanitizer.sanitize(getParam("inj"))));
</script>
<div>Hello {{"Angular"}}!</div>
</body>
