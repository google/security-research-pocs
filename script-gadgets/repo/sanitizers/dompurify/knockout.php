<head>
<script src="https://code.jquery.com/jquery-3.1.1.js"></script>
<script src="http://knockoutjs.com/downloads/knockout-3.4.1.debug.js"></script>

<script nonce="random" src="purify.js"></script>
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
<script>
  ko.applyBindings();
</script>
</body>