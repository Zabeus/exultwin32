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

  if($ForumSecurity > SEC_OPTIONAL && empty($phorum_auth)){
    header("Location: $forum_url/login.$ext?target=$REQUEST_URI");
    exit();
  }

  require "$include_path/post_functions.php";

  $thread=(int)initvar("t");
  $action=initvar("a");
  $id=(int)initvar("i");
  $parent=(int)initvar("p");

  if($num==0 || $ForumName==''){
    Header("Location: $forum_url?$GetVars");
    exit;
  }

  $ip = getenv('REMOTE_HOST');
  if(!$ip){
    $ip = getenv('REMOTE_ADDR');
  }
  if(!$ip){
    $ip = $REMOTE_ADDR;
  }
  if(!$ip){
    $ip = $REMOTE_HOST;
  }

  $host = @GetHostByAddr($ip);

  $IsError = @check_data($host, $author, $subject, $body, $email);


  // Attachment handling:
  if(is_array($HTTP_POST_FILES) && count($HTTP_POST_FILES)>0){
    // PHP4 style
    $attachments=&$HTTP_POST_FILES;
  }
  else{
    // PHP3 style
    $max=min($MaximumNumberAttachments, $ForumMaxUploads);
    for($x=0;$x<$max;$x++){
      $var="attachment_$x";
      if(isset($$var)){
        $attachments[$x]["tmp_name"]=$$var;
        $var="attachment_$x"."_name";
        $attachments[$x]["name"]=$$var;
        $var="attachment_$x"."_size";
        $attachments[$x]["size"]=$$var;
        $var="attachment_$x"."_type";
        $attachments[$x]["type"]=$$var;
      }
    }
  }

  if (@is_array($attachments) && $AllowAttachments && $ForumAllowUploads == 'Y') {

    while(list($key, $arr)=each($attachments)){
      if(is_uploaded_file($arr["tmp_name"])){
        $min_size=1024*min((int)$ForumUploadSize, (int)$AttachmentSizeLimit);
        if (!ereg("^[-A-Za-z0-9_\.]+$", trim($arr["name"]))) {
          $IsError="$lInvalidFile ($arr[name])";
        }
        elseif(!empty($ForumUploadTypes) && !strstr($ForumUploadTypes, strtolower(substr($arr["name"], strrpos($arr["name"], ".")+1)))){
          $IsError=$lInvalidType.strtoupper(ereg_replace(";", " ", $ForumUploadTypes));
        }
        elseif($min_size>0  && $arr["size"]>$min_size){
          $IsError=$lInvalidSize1.$arr["name"]."<br>".$lInvalidSize2.(string)min($ForumUploadSize, $AttachmentSizeLimit)."k";
        }
      }
    }

  }

  if($IsError || !$action){
    if(file_exists("$include_path/header_$ForumConfigSuffix.php")){
      include "$include_path/header_$ForumConfigSuffix.php";
    }
    else{
      include "$include_path/header.php";
    }

  //////////////////////////
  // START NAVIGATION     //
  //////////////////////////

    $menu=array();
    if($ActiveForums>1)
      // Forum List
      addnav($menu, $lForumList, "$forum_page.$ext?f=$ForumParent$GetVars");
    // Go To Top
    addnav($menu, $lGoToTop, "$list_page.$ext?f=$num$GetVars");
    // Search
    addnav($menu, $lSearch, "$search_page.$ext?f=$num$GetVars");

    // Log Out/Log In
      if($ForumSecurity){
        if(!empty($phorum_auth)){
          addnav($menu, $lLogOut, "login.$ext?logout=1");
          addnav($menu, $lMyProfile, "profile.$ext?f=$f&id=$phorum_user[id]$GetVars");
        }
        else{
          addnav($menu, $lLogIn, "login.$ext");
        }
      }

  $nav=getnav($menu);
    $TopLeftNav=$nav;

  //////////////////////////
  // END NAVIGATION       //
  //////////////////////////

    include "$include_path/form.php";
    if(file_exists("$include_path/footer_$ForumConfigSuffix.php")){
      include "$include_path/footer_$ForumConfigSuffix.php";
    }
    else{
      include "$include_path/footer.php";
    }
    exit();
  }

  $author=trim($author);
  $subject=trim($subject);
  $email=trim($email);
  $body=chop($body);

  if($UseCookies){
    $name_cookie="phorum_name";
    $email_cookie="phorum_email";

    if((!IsSet($$name_cookie)) || ($$name_cookie != $author)) {
      SetCookie($name_cookie,stripslashes($author),time()+ 31536000);
    }
    if((!IsSet($$email_cookie)) || ($$email_cookie != $email)) {
      SetCookie($email_cookie,stripslashes($email),time()+ 31536000);
    }
  }

  list($author, $subject, $email, $body) = censor($author, $subject, $email, $body);

  if(!get_magic_quotes_gpc()){
    $author = addslashes($author);
    $email = addslashes($email);
    $subject = addslashes($subject);
    $body = addslashes($body);
  }

  $datestamp = date("Y-m-d H:i:s");

  $plain_author=stripslashes($author);
  $plain_subject=stripslashes(ereg_replace("<[^>]+>", "", $subject));
  $plain_body=stripslashes(ereg_replace("<[^>]+>", "", $body));

  $author = htmlspecialchars($author);
  $email = htmlspecialchars($email);
  $subject = htmlspecialchars($subject);

  if(!empty($phorum_user["moderator"])){
    $author = "<b>$author</b>";
    $subject = "<b>$subject</b>";
    $body="<HTML>$body</HTML>";
  }
  else{
    $body=eregi_replace("</*HTML>", "", $body);
    if($ForumAllowHTML=="Y"){
      $body="<HTML>$body</HTML>";
    }
  }

  $more="";

  if (!check_dup() && check_parent($parent)) {
    // generate a message id for the email if needed.
    $msgid="<".md5(uniqid(rand())).".$ForumName>";

    // add the users signature if requested
    if(isset($use_sig)){
        $body.="\n\n".PHORUM_SIG_MARKER;
    }

    // This will add the message to the database, and email the
    // moderator if required.
    $id = post_to_database();
    if (!$id) {
      echo $error;
      exit();
    }

    // mark this message as read in their cookies since they wrote it.
    $haveread_cookie="phorum-haveread-$ForumTableName";
    if(empty($$haveread_cookie)){
      $$haveread_cookie=$id;
    }
    else{
      $$haveread_cookie.=".";
      $$haveread_cookie.="$id";
    }
    SetCookie("phorum-haveread-$ForumTableName",$$haveread_cookie,0);

    // if it is not a new message and not float to top
    // send them to the message.
    if($thread!=$id && $ForumMultiLevel!=2){
      $more = $thread+1;
      $more = "&a=2&t=$more";
    }

    // Attachment handling:
    if(@is_array($attachments)){
      reset($attachments);
      while(list($key, $attachment)=each($attachments)){
        if($attachment["name"])
          $IsError=add_attachment($attachment, $id);

        if($IsError){

          if(file_exists("$include_path/header_$ForumConfigSuffix.php")){
            include "$include_path/header_$ForumConfigSuffix.php";
          }
          else{
            include "$include_path/header.php";
          }

          //////////////////////////
          // START NAVIGATION     //
          //////////////////////////

            $menu=array();
            if($ActiveForums>1)
              // Forum List
              addnav($menu, $lForumList, "$forum_page.$ext?f=$ForumParent$GetVars");
            // Go To Top
            addnav($menu, $lGoToTop, "$list_page.$ext?f=$num$GetVars");
            // Search
            addnav($menu, $lSearch, "$search_page.$ext?f=$num$GetVars");
            // Log Out/Log In
            if($ForumSecurity){
              if(!empty($phorum_auth)){
                addnav($menu, $lLogOut, "login.$ext?logout=1");
                addnav($menu, $lMyProfile, "profile.$ext?f=$f&id=$phorum_user[id]$GetVars");
              }
              else{
                addnav($menu, $lLogIn, "login.$ext");
              }
            }

            $TopLeftNav=getnav($menu);

          //////////////////////////
          // END NAVIGATION       //
          //////////////////////////

          include "$include_path/form.php";
          if(file_exists("$include_path/footer_$ForumConfigSuffix.php")){
            include "$include_path/footer_$ForumConfigSuffix.php";
          }
          else{
            include "$include_path/footer.php";
          }
          exit();


        }
      }
    }

    // This will send email to the mailing list, if applicable,
    // and send email replies to earlier posters, if necessary.
    // Note that when posting to a mailing list, active moderation
    // does not apply.
    post_to_email();
  }

  if(initvar("attach")){
    Header ("Location: $forum_url/$attach_page.$ext?f=$num&i=$id$GetVars");
  }
  else{
    Header ("Location: $forum_url/$list_page.$ext?f=$num$more$GetVars");
  }
?>