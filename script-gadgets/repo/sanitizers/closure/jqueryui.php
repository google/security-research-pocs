<!doctype html>
<html lang="en">
<head>
  <!-- <meta http-equiv=content-security-policy content="style-src code.jquery.com; script-src 'self' 'nonce-random' 'strict-dynamic'; "> -->
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>jQuery UI Tabs - Default functionality</title>
  <link rel="stylesheet" href="//code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css">
  <script nonce="random" src="https://code.jquery.com/jquery-3.1.1.js"></script>
  <script nonce="random" src="https://code.jquery.com/ui/1.12.1/jquery-ui.js"></script>
  <script nonce="random">
  $( function() {
    $( "#tabs" ).tabs();
  } );
  </script>
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
<body>

<script nonce="random">
  var sanitizer = new goog.html.sanitizer.HtmlSanitizer();
  document.write(goog.html.SafeHtml.unwrap(sanitizer.sanitize(getParam("inj"))));
</script>
 
 
</body>
</html>
