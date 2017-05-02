<?php header('X-XSS-Protection: 0'); ?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="content-security-policy" content="script-src 'nonce-random' 'strict-dynamic';">
<script nonce=random src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.js"></script>
<script nonce=random>
$(document).ready(function(){
    // code taken from http://api.jquery.com/after/
    $( ".container" ).after( $( ".child" ) );
});
</script>
</head>
<body>


<?php echo $_GET['inj'] ?>


<p class="container"></p>

</body>
</html>
