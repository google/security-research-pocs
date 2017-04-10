<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.css"></link>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.js"></script>

<script src="purify.js"></script>
<script>
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
<script>
  document.write(DOMPurify.sanitize(getParam("inj")));
</script>

<script>
   $('[data-toggle="tooltip"]').tooltip(); 
</script>
