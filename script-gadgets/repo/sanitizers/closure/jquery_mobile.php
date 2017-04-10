<?php
  /*noscript ignores this*/header("x-xss-protection: 0");
?>
<html>
<head>

<!-- Include meta tag to ensure proper rendering and touch zooming -->
<meta name="viewport" content="width=device-width, initial-scale=1">

<!-- Include jQuery Mobile stylesheets -->
<link rel="stylesheet" href="https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css">

<!-- Include the jQuery library -->
<script src="https://code.jquery.com/jquery-1.10.2.js"></script>

<!-- Include the jQuery Mobile library -->
<script src="https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.js"></script>

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

<script nonce="random">
  var sanitizer = new goog.html.sanitizer.HtmlSanitizer.Builder().build();
  document.write(goog.html.SafeHtml.unwrap(sanitizer.sanitize(getParam("inj"))));
</script>



</body>
</html>
