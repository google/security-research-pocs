<script src="https://unpkg.com/vue/dist/vue.js"></script>

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

<script nonce="random">
  document.write(DOMPurify.sanitize(getParam("inj")));
</script>


<div id="app-4">
 Hi
</div>
<script nonce="random">
var app4 = new Vue({
  el: '#app-4',
  data: {}
})
</script>
