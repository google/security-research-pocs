<?php
  if (!empty($_GET['xssfilter'])) {
    header('X-XSS-Protection: 1');
  } else {
    header('X-XSS-Protection: 0');
  }?>
<!doctype html>
<html lang="en">
  <head>

<meta http-equiv=Content-Security-Policy content="
<?php
  if (!isset($_GET['csp'])) {
    $_GET['csp'] = 'sd';
  }
  $csp = "";
  switch ($_GET['csp']) {
    case 'sd':
      $csp = "object-src 'none'; script-src 'nonce-random' 'strict-dynamic' https: http:";
      break;
    case 'ue':
      $csp = "object-src 'none'; script-src 'nonce-random' 'unsafe-eval' 'self' https://download.dojotoolkit.org/ https://ajax.googleapis.com/";
      break;
    case 'wh':
      $csp = "object-src 'none'; script-src 'self' https://download.dojotoolkit.org/ https://ajax.googleapis.com/";
      break;
  }
  echo $csp;
?>
">
<?php if ($_GET['csp'] != 'ue') : ?>
  <script nonce=random src=dojoconfig.js></script>
<?php endif; ?>

<?php if ($_GET['sanitize']) : ?>
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
<?php endif; ?>
<?php if ($_GET['sanitize'] == 'closure') : ?>
  <script nonce="random" src=/repo/sanitizers/closure/closure-library/closure/goog/base.js></script>
  <script nonce="random" src="data:,goog.require('goog.html.sanitizer.HtmlSanitizer');goog.require('goog.html.SafeHtml');"></script>
<?php elseif ($_GET['sanitize'] == 'dompurify') : ?>
<script src=https://cure53.de/purify.js></script>
<?php endif; ?>
  </head>
  <body>

<?php if ($_GET['sanitize'] == 'closure'): ?>
<script nonce="random">
  var sanitizer = new goog.html.sanitizer.HtmlSanitizer();
  document.write(goog.html.SafeHtml.unwrap(sanitizer.sanitize(getParam("inj"))));
</script>
<?php elseif ($_GET['sanitize'] == 'dompurify') : ?>
<script nonce=random>
  document.write(DOMPurify.sanitize(getParam('inj')));
</script>
<?php endif ?>

  <?php if (!$_GET['sanitize']) { echo $_GET['inj']; } ?>

  <script nonce=random src="https://ajax.googleapis.com/ajax/libs/dojo/1.12.2/dojo/dojo.js"></script>
  <!--<script nonce=random src="https://download.dojotoolkit.org/release-1.12.2/dojo.js.uncompressed.js"></script>-->
  <script nonce=random>
    require([
      "dojo/parser",
      "dijit/form/Button",
      "dojo/domReady!"
    ], function(parser){
      parser.parse();
    });
  </script>
    Dojo
    <?php echo $_GET['inj_post'] ?>


