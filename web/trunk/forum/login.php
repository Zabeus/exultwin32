<?php
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//   Copyright (C) 2000  Phorum Development Team                              //
//   http://www.phorum.org                                                    //
//                                                                            //
//   This program is free software. You can redistribute it and/or modify     //
//   it under the terms of either the current Phorum License (viewable at     //
//   phorum.org) or the Phorum License that was distributed with this file    //
//                                                                            //
//   This program is distributed in the hope that it will be useful,          //
//   but WITHOUT ANY WARRANTY, without even the implied warranty of           //
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                     //
//                                                                            //
//   You should have received a copy of the Phorum License                    //
//   along with this program.                                                 //
////////////////////////////////////////////////////////////////////////////////

  require "./common.php";

  settype($Error, "string");

  if(empty($target)){
    if(isset($HTTP_REFERER)){
      $target=$HTTP_REFERER;
    }
    else{
      $target="$forum_url/$forum_page.$ext";
    }
  }

  initvar("phorum_auth");

//  $target=str_replace("phorum_auth=$phorum_auth", '', $target);

  if(isset($logout)){
    unset($phorum_auth);
    SetCookie("phorum_auth", "");
    $SQL="update $pho_main"."_auth set sess_id='' where sess_id='$phorum_auth'";
    $q->query($DB, $SQL);
    header("Location: $target");
    exit();
  }

  if(!empty($username) && !empty($password)){
    $crypt_pass=crypt($password, substr($password, 0, CRYPT_SALT_LENGTH));
    $uname=str_replace("'", "\\'", $username);
    $SQL="Select id from $pho_main"."_auth where username='$uname' and password='$crypt_pass'";
    $q->query($DB, $SQL);
    $rec=$q->getrow();
    if(!empty($rec["id"])){
      $sess_id=md5($username.$password);
      phorum_login_user($sess_id, $rec["id"]);
      if(!strstr($target, "?")){
        $target.="?f=0$GetVars";
      }
      else{
        $target.="$GetVars";
      }
      header("Location: $target");
      exit();
    }
    else{
      $Error=$lLoginError;
    }
  }

  if(basename($PHP_SELF)=="login.$ext"){
    $title = " - $lLoginCaption";
    if(file_exists("$include_path/header_$ForumConfigSuffix.php")){
      include "$include_path/header_$ForumConfigSuffix.php";
    }
    else{
      include "$include_path/header.php";
    }
  }

  // hack
  $signup_page="register";

  //////////////////////////
  // START NAVIGATION     //
  //////////////////////////

    $menu="";
    if($ActiveForums>1){
      addnav($menu, $lForumList, "$forum_page.$ext?f=0$GetVars");
    }
    addnav($menu, $lRegisterLink, "$signup_page.$ext?f=$f&target=$target$GetVars");
    $nav=getnav($menu);

  //////////////////////////
  // END NAVIGATION       //
  //////////////////////////


  if($Error){
    echo "<p><b>$Error</b>";
  }
?>
<form action="<?php echo "login.$ext"; ?>" method="post">
<input type="hidden" name="f" value="<?php echo $f; ?>">
<input type="hidden" name="target" value="<?php echo $target; ?>">
<?php echo $PostVars; ?>
<table cellspacing="0" cellpadding="0" border="0">
<tr>
    <td <?php echo bgcolor($default_nav_color); ?>>
      <table cellspacing="0" cellpadding="2" border="0">
        <tr>
          <td><?php echo $nav; ?></td>
        </tr>
      </table>
    </td>
</tr>
<tr>
    <td <?php echo bgcolor($default_nav_color); ?>>
        <table class="PhorumListTable" cellspacing="0" cellpadding="2" border="0">
        <tr>
            <td height="21" colspan="2" <?php echo bgcolor($default_table_header_color); ?>><FONT color="<?php echo $default_table_header_font_color; ?>">&nbsp;<?php echo $lLoginCaption; ?></font></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lUserName;?>:</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="Text" name="username" size="30" maxlength="50"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lPassword;?>:</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="Password" name="password" size="30" maxlength="20"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap>&nbsp;</td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="submit" value="<?php echo $lLogin; ?>">&nbsp;<br><img src="images/trans.gif" width=3 height=3 border=0></td>
        </tr>
        </table>
    </td>
</tr>
</table>
</FORM>
<?php
  if(basename($PHP_SELF)=="login.$ext"){
    if(file_exists("$include_path/footer_$ForumConfigSuffix.php")){
      include "$include_path/footer_$ForumConfigSuffix.php";
    }
    else{
      include "$include_path/footer.php";
    }
  }
?>
