<?php
  header('access-control-allow-origin: *');
  header('access-control-allow-headers: X-Requested-With');
  header('content-type: text/html');
?>
<div data-role=page data-url="<img src=x onerror=alert(document.domain) foo=">
