<?php
if(isset($_GET)){
  while(list($var, $val)=each($_GET)){
    $$var=$val;
  }
}
if(isset($_POST)){
  while(list($var, $val)=each($_POST)){
    $$var=$val;
  }
}
if(isset($_COOKIE)){
  while(list($var, $val)=each($_COOKIE)){
    $$var=$val;
  }
}
if(isset($_SERVER)){
  while(list($var, $val)=each($_SERVER)){
    $$var=$val;
  }
}
?>
