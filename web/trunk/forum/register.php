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

  initvar("name");
  initvar("password");
  initvar("user");
  initvar("email");
  initvar("webpage");
  initvar("image");
  initvar("signature");
  initvar("icq");
  initvar("yahoo");
  initvar("aol");
  initvar("jabber");
  initvar("msn");
  initvar("done");
  initvar("Error");
  initvar("process");

  if(empty($target)){
    if(isset($HTTP_REFERER)){
      $target=$HTTP_REFERER;
    }
    else{
      $target="$forum_url/$forum_page.$ext";
    }
  }

  if(!empty($user) && !empty($name) && !empty($email) && !empty($password) && !empty($checkpassword)){
  $SQL="select username, name, email from ".$pho_main."_auth where (username='$user' or name='$name' or email='$email')";
//    if(!empty($id)) $SQL.=" and id!=$id";
    //run query
    $q->query($DB,$SQL);
    if($q->numrows()>0){
      $rec=$q->getrow();
      if($rec['username']==$user)
        $Error=$lDupUsername;
      if($rec['name']==$name)
        $Error=$lDupName;
      if($rec['email']==$email)
        $Error=$lDupEmail;
      if($password!=$checkpassword)
        $Error=$lNoPassMacth;
    }else{
      $crypt_pass=crypt($password, substr($password, 0, CRYPT_SALT_LENGTH));
      $id=$DB->nextid($pho_main."_auth");
      $SQL="Insert into $pho_main"."_auth
            (id, name, username, email, webpage, image, password, signature, icq, yahoo, aol, msn, jabber)
            values
            ($id, '$name', '$user', '$email', '$webpage', '$image', '$crypt_pass', '$signature', '$icq', '$yahoo', '$aol', '$msn', '$jabber')";
      $q->query($DB, $SQL);
      echo $q->error();
      if($DB->type=="mysql")
        $id=$DB->lastid();
      $sess_id=md5($user.$password);
      phorum_login_user($sess_id, $id);
      $done=true;
    }
  }
  elseif($process){
    $Error=$lFillInAll;
  }

  $title = " - $lRegisterCaption";
  if(file_exists("$include_path/header_$ForumConfigSuffix.php")){
    include "$include_path/header_$ForumConfigSuffix.php";
  }
  else{
    include "$include_path/header.php";
  }

  // hack
  $login_page="login";

  //////////////////////////
  // START NAVIGATION     //
  //////////////////////////

    if(count($ActiveForums)>1){
      addnav($menu, $lForumList, "$forum_page.$ext?f=$ForumParent$GetVars");
    }
    addnav($menu, $lLoginLink, "$login_page.$ext?f=$f&target=$target$GetVars");
    $nav=getnav($menu);

  //////////////////////////
  // END NAVIGATION       //
  //////////////////////////
  if($Error)
    echo "<p><b>$Error</b>";
  if(!$done){ ?>
<SCRIPT LANGUAGE="JavaScript">
    function textlimit(field, limit) {
        if (field.value.length > limit)
            field.value = field.value.substring(0, limit);
    }
</script>
<form action="<?php echo $PHP_SELF; ?>" method="post">
<input type="hidden" name="process" value="1">
<input type="hidden" name="target" value="<?php echo $target; ?>">
<input type="hidden" name="f" value="<?php echo $f; ?>">
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
            <td height="21" colspan="2" <?php echo bgcolor($default_table_header_color); ?>><FONT color="<?php echo $default_table_header_font_color; ?>">&nbsp;<?php echo $lRegisterCaption; ?></font></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lFormName;?>*:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="name" size="30" maxlength="50" value="<?php echo $name; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lFormEmail;?>*:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="email" size="30" maxlength="50" value="<?php echo $email; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lUserName;?>*:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="user" size="30" maxlength="50" value="<?php echo $user; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lPassword;?>*:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="password" name="password" size="20" maxlength="20" value=""></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lPassAgain; ?>*:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="password" name="checkpassword" size="20" maxlength="20" value=""></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lWebpage;?>:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="webpage" size="50" maxlength="100" value="<?php echo $webpage; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lImageURL;?>:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="image" size="50" maxlength="100" value="<?php echo $image; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;ICQ:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="icq" size="50" maxlength="50" value="<?php echo $icq; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;AOL:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="aol" size="50" maxlength="50" value="<?php echo $aol; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;Yahoo:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="yahoo" size="50" maxlength="50" value="<?php echo $yahoo; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;MSN:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="msn" size="50" maxlength="50" value="<?php echo $msn; ?>"></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;Jabber:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="text" name="jabber" size="50" maxlength="50" value="<?php echo $jabber; ?>"></td>
        </tr>
        <tr>
            <td valign=top <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>">&nbsp;<?php echo $lSignature;?>:&nbsp;&nbsp;</font></td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><textarea onKeyDown="textlimit(this.form.signature,255);" onKeyUp="textlimit(this.form.signature,255);" cols="30" rows="6" name="signature"><?php echo $signature; ?></textarea></td>
        </tr>
        <tr>
            <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap>&nbsp;</td>
            <td <?php echo bgcolor($default_table_body_color_1); ?>><input type="submit" value="<?php echo $lRegister; ?>">&nbsp;<br><img src="images/trans.gif" width=3 height=3 border=0></td>
        </tr>
        </table>
    </td>
</tr>
</table>
</FORM>
<?php
    }else{
      if(empty($QUERY_STRING) || substr($target, -1)!="?"){
        $target.="?$GetVars";
      }
      else{
        $target="&$GetVars";
      }
?>
<table class="PhorumListTable" cellspacing="0" cellpadding="2" border="0">
<tr>
    <td height="21" <?php echo bgcolor($default_table_header_color); ?>><FONT color="<?php echo $default_table_header_font_color; ?>">&nbsp;<?php echo $lRegisterThanks; ?></font></td>
</tr>
<tr>
    <td <?php echo bgcolor($default_table_body_color_1); ?> nowrap><font color="<?php echo $default_table_body_font_color_1; ?>"><a href="<?php echo $target; ?>"><?php echo $lRegisterReturn; ?></a></font></td>
</tr>
</table>
<?php } ?>
<?php
  if(file_exists("$include_path/footer_$ForumConfigSuffix.php")){
    include "$include_path/footer_$ForumConfigSuffix.php";
  }
  else{
    include "$include_path/footer.php";
  }
?>