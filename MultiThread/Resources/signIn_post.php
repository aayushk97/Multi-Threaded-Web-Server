
<html>
<body>

Welcome 

<?php if (!isset($_SERVER["HTTP_HOST"])) {
  parse_str($argv[1], $_GET);
  parse_str($argv[1], $_POST);
}

echo $_POST["name"]; ?>
<br>

Your email address is: <?php if (!isset($_SERVER["HTTP_HOST"])) {
  parse_str($argv[1], $_GET);
  parse_str($argv[1], $_POST);
}

echo $_POST["email"]; ?>

</body>
</html> 
