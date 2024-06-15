<?php
if (!isset($_SERVER["HTTP_HOST"])) {
  parse_str($argv[1], $_GET);
  parse_str($argv[1], $_POST);
}
   if( $_POST["name"] || $_POST["email"] ) {
      if (preg_match("/[^A-Za-z'-]/",$_POST['email'] )) {
         die ("invalid name or email");
      }
      echo "Welcome ". $_POST['name']."\n";
      echo "Your email is ". $_POST['email']. ".\n";
      
      exit();
   }
?>
<html>
   <body>
   
      <form action = "<?php $_PHP_SELF ?>" method = "POST">
         Name: <input type = "text" name = "name" />
         Age: <input type = "text" name = "age" />
         <input type = "submit" />
      </form>
   
   </body>
</html>
