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

  $thread=(int)initvar("t");
  $action=(int)initvar("a");

  if($num==0 || $ForumName==''){
    header("Location: $forum_url?$GetVars");
    exit;
  }

  $phcollapse="phorum-collapse-$ForumTableName";
  $new_cookie="phorum-new-$ForumTableName";
  $haveread_cookie="phorum-haveread-$ForumTableName";

  if($UseCookies){

    if (initvar("r")==1) {
      $SQL = "Select max(id) as max_id FROM $ForumTableName WHERE approved='Y'";
      $q->query($DB, $SQL);
      $aryRow=$q->getrow();
      if(isset($aryRow['max_id'])){
        $max_id=$aryRow['max_id'];
        $$new_cookie=$max_id;
        SetCookie($new_cookie,$$new_cookie,time()+ 31536000);
        SetCookie($haveread_cookie,$$new_cookie); //destroy session cookie
        unset($$haveread_cookie);
      }
    }

    if(!IsSet($$new_cookie)){
      $$new_cookie='0';
    }

    $use_haveread=false;
    if(IsSet($$haveread_cookie)) {
      $arr=explode(".", $$haveread_cookie);
      $old_message=reset($arr);
      array_walk($arr, "explode_haveread");
      $use_haveread=true;
    }
    else{
      $old_message=$$new_cookie;
    }

    if(IsSet($collapse)){
      $$phcollapse=$collapse;
      SetCookie("phorum-collapse-$ForumTableName",$collapse,time()+ 31536000);
    }
    elseif(!isset($$phcollapse)){
      $$phcollapse=$ForumCollapse;
    }

  }
  else{
    if(IsSet($collapse)){
      $$phcollapse=$collapse;
    }
    else{
      $$phcollapse=$ForumCollapse;
    }
  }

  if($DB->type=="postgresql"){
    $limit="";
    $q->query($DB, "set QUERY_LIMIT TO '$ForumDisplay'");
  }
  else{
    $limit=" limit $ForumDisplay";
  }

  if ($ForumMultiLevel==2) {
    $myflag="";
    if ($thread!=0) {
      $myflag = " AND modifystamp < $thread";
      if($action==1){
        $SQL = "Select modifystamp as maxtime from $ForumTableName WHERE approved='Y'  AND modifystamp > $thread ORDER BY modifystamp DESC".$limit;

        $q->query($DB, $SQL);
        if($q->numrows()>0){
          $rec=$q->getrow();
          if (isset($rec["maxtime"])) {
            $myflag = " AND modifystamp <= ".$rec["maxtime"];
          }
        }
      }
    }

    if($$phcollapse==0){
      $SQL = "SELECT thread, modifystamp FROM $ForumTableName WHERE approved='Y' $myflag GROUP BY thread, modifystamp ORDER BY modifystamp desc, thread desc".$limit;
    } else {
      if($DB->type=="mysql"){
        $convfunc="FROM_UNIXTIME";
      }
      else{
        $convfunc="datetime";
      }
      $SQL = "SELECT thread, modifystamp, count(id) AS tcount, $convfunc(modifystamp) AS latest, max(id) as maxid FROM $ForumTableName WHERE approved='Y' $myflag GROUP BY thread, modifystamp ORDER BY modifystamp desc, thread desc".$limit;
    }

    $thread_list = new query($DB, $SQL);

    if($DB->type=="postgresql"){
      $q->query($DB, "set QUERY_LIMIT TO '0'");
    }

    $rec=$thread_list->getrow();

    if(empty($rec["thread"]) && $action!=0){
      Header("Location: $forum_url/$list_page.$ext?f=$num$GetVars");
      exit();
    }

    $aryThreadstring=array();
    $threads=array();
    $max=0;
    $min=0;
    while (is_array($rec)){
      if($rec["modifystamp"]>$max) {
        $max=$rec["modifystamp"];
        if($min==0) {
          $min=$max;
        }
      } elseif ($rec["modifystamp"]<$min) {
        $min=$rec["modifystamp"];
      }
      $aryThreadstring[$rec["thread"]] = $rec["thread"];
      $threads[]=$rec;
      $rec=$thread_list->getrow();
    }
    $threadstring = implode(",",$aryThreadstring);

    if (!empty($threadstring)) {

      if($$phcollapse==0){
        $SQL = "Select id,parent,thread,subject,author,datestamp,userid from $ForumTableName WHERE approved='Y' $myflag and thread IN (".$threadstring.") order by modifystamp desc, id asc";
      } else {
        $SQL = "Select id,0 as parent,thread,subject,author,datestamp,userid from $ForumTableName WHERE approved='Y' AND parent = 0 $myflag and thread IN (".$threadstring.") order by modifystamp desc";
      }
    }
  } else {

    if($thread==0 || $action==0){
      $SQL = "Select max(thread) as thread from $ForumTableName where approved='Y'";
      $q->query($DB, $SQL);
      if($q->numrows()>0){
        $rec=$q->getrow();
        $maxthread = isset($rec["thread"]) ? $rec["thread"] : 0;
      }
      else{
        $maxthread=0;
      }
      if ($maxthread > $cutoff) {
        $cutoff_thread=$maxthread-$cutoff;
      } else {
        $cutoff_thread = 0;
      }
      if($$phcollapse==0){
        $SQL = "Select thread from $ForumTableName where thread > $cutoff_thread and approved='Y' order by thread desc".$limit;
      }
      else{
        $SQL = "Select thread, count(id) as tcount, max(datestamp) as latest, max(id) as maxid from $ForumTableName where approved='Y' AND thread > $cutoff_thread group by thread order by thread desc".$limit;
      }
    }
    else{
      if($action==1){
        $cutoff_thread=$thread+$cutoff;
        if($$phcollapse==0){
          $SQL = "Select thread from $ForumTableName where approved='Y' AND thread < $cutoff_thread AND thread > $thread order by thread".$limit;
        }
        else{
          $SQL = "Select thread from $ForumTableName where thread=id AND approved='Y' AND thread < $cutoff_thread AND thread > $thread order by thread".$limit;
        }

        $q=new query($DB, $SQL);
        $rec=$q->getrow();
        if(!empty($rec["thread"])){
          $keepgoing=true;
          $x=0;
          while (is_array($rec) && $keepgoing){
            $thread = $rec["thread"];
            $rec=$q->getrow();
            $x++;
          }
        }
        $thread=$thread+1;
      }
      if ($thread > $cutoff) {
        $cutoff_thread=$thread-$cutoff;
      } else {
        $cutoff_thread = 0;
      }
      if($$phcollapse==0){
        $SQL = "Select thread from $ForumTableName where approved='Y' and thread < $thread and thread > $cutoff_thread order by thread desc".$limit;
      }
      else{
        $SQL = "Select thread, COUNT(id) AS tcount, MAX(datestamp) AS latest, MAX(id) AS maxid FROM $ForumTableName WHERE approved='Y' AND thread < $thread AND thread > $cutoff_thread GROUP BY thread ORDER BY thread DESC".$limit;
      }
    }

    $thread_list = new query($DB, $SQL);

    if($DB->type=="sybase") {
      $q->query($DB, "set rowcount 0");
    }
    elseif($DB->type=="postgresql"){
      $q->query($DB, "set QUERY_LIMIT TO '0'");
    }

    $rec=$thread_list->getrow();

    if(empty($rec["thread"]) && $action!=0){
      Header("Location: $forum_url/$list_page.$ext?f=$num$GetVars");
      exit();
    }

    if(isset($rec['thread'])){
      $max=$rec["thread"];
      $keepgoing=true;
      $x=0;
      while (is_array($rec)){
        $threads[]=$rec;
        $min=$rec["thread"];
        $rec=$thread_list->getrow();
      }
    }
    else{
      $threads="";
      $max=0;
      $min=0;
    }

    if($$phcollapse==0){
      $SQL = "Select id,parent,thread,subject,author,datestamp,userid from $ForumTableName where approved='Y' AND thread<=$max and thread>=$min order by thread desc, id asc";
    }
    else{
      $SQL = "Select id,thread,subject,author,datestamp,userid from $ForumTableName where approved='Y' AND thread = id AND thread<=$max AND thread>=$min order by thread desc";
    }

  }

  $headers=array();

  $msg_list = new query($DB, $SQL);

  $rec=$msg_list->getrow();
  while(is_array($rec)){
    if(!empty($rec["thread"])){
        $headers[]=$rec;
        if($ForumSecurity!=SEC_NONE && !empty($rec["userid"])) $ids[]=$rec["userid"];
    }
    $rec=$msg_list->getrow();
  }

  $rows=@count($headers);

  // Get the user info.  I curse PG for not having Left Joins.
  if(@is_array($ids)){
    $SQL="select id, name, email, signature from $pho_main"."_auth where id in (".implode(",", $ids).")";
    $q->query($DB, $SQL);
    $rec=$q->getrow();
    While(is_array($rec)){
      $users[$rec["id"]]=$rec;
      $rec=$q->getrow();
    }

    $SQL="select user_id from $pho_main"."_moderators where forum_id=$f and user_id in (".implode(",", $ids).")";
    $q->query($DB, $SQL);
    $rec=$q->getrow();
    While(is_array($rec)){
      $moderators[$rec["user_id"]]=true;
      $rec=$q->getrow();
    }

  }

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

    // Forum List
    if($ActiveForums>1)
      addnav($menu, $lForumList, "$forum_page.$ext?f=$ForumParent$GetVars");

    // Go To Top
    addnav($menu, $lGoToTop, "$list_page.$ext?f=$num$GetVars");

    // New Topic
    addnav($menu, $lStartTopic, "$post_page.$ext?f=$num$GetVars");

    // Collapse/View Threads
    if($$phcollapse==0){
      addnav($menu, $lCollapseThreads, "$list_page.$ext?f=$num&collapse=1$GetVars");
    }
    else{
      addnav($menu, $lViewThreads, "$list_page.$ext?f=$num&collapse=0$GetVars");
    }

    // Search
    addnav($menu, $lSearch, "$search_page.$ext?f=$num$GetVars");

    // Mark all read
    addnav($menu, $lMarkRead, "$list_page.$ext?f=$num$GetVars&r=1");

    // Log Out/Log In
    if($ForumSecurity){
      if(!empty($phorum_auth)){
        addnav($menu, $lLogOut, "login.$ext?logout=1&f=$f$GetVars");
        addnav($menu, $lMyProfile, "profile.$ext?f=$f&id=$phorum_user[id]$GetVars");
      }
      else{
        addnav($menu, $lLogIn, "login.$ext?f=$f$GetVars");
      }
    }

    $nav=getnav($menu);
    $TopLeftNav=$nav;
    $LowLeftNav=$nav;

    $menu=array();

    if ($ForumMultiLevel==2) {
      if($action!=0 && $min < $thread) {
        // Newer
        addnav($menu, $lNewerMessages, "$list_page.$ext?f=$num&t=$max&a=1$GetVars");
      }
    } elseif ($action!=0) {
      // Newer
      addnav($menu, $lNewerMessages, "$list_page.$ext?f=$num&t=$max&a=1$GetVars");
    }
    if($rows>=$ForumDisplay) {
      // Older
      addnav($menu, $lOlderMessages, "$list_page.$ext?f=$num&t=$min&a=2$GetVars");
    }
    $nav=getnav($menu);
    $TopRightNav=$nav;
    $LowRightNav=$nav;

  //////////////////////////
  // END NAVIGATION       //
  //////////////////////////

?>
<table width="<?php echo $ForumTableWidth; ?>" cellspacing="0" cellpadding="3" border="0">
<tr>
  <td width="60%" nowrap <?php echo bgcolor($ForumNavColor); ?>><?php echo $TopLeftNav; ?></td>
  <td align="right" width="40%" <?php echo bgcolor($ForumNavColor); ?>><div class=nav><?php echo $TopRightNav; ?></div></td>
</tr>
</table>
<?php
  if(!$ForumMultiLevel || $$phcollapse) {
    include "$include_path/threads.php";
  } else {
    include "$include_path/multi-threads.php";
  }
?>
<table width="<?php echo $ForumTableWidth; ?>" cellspacing="0" cellpadding="3" border="0">
<tr>
  <td width="60%" nowrap <?php echo bgcolor($ForumNavColor); ?>><?php echo $LowLeftNav; ?></td>
  <td align="right" width="40%" <?php echo bgcolor($ForumNavColor); ?>><div class=nav><?php echo $LowRightNav; ?></div></td>
</tr>
</table>
<?php
  if(file_exists("$include_path/footer_$ForumConfigSuffix.php")){
    include "$include_path/footer_$ForumConfigSuffix.php";
  }
  else{
    include "$include_path/footer.php";
  }
?>
