<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
<!--
<meta name="Phorum Version" content="<?PHP echo $phorumver; ?>">
<meta name="Phorum DB" content="<?PHP echo $DB->type; ?>">
<meta name="PHP Version" content="<?PHP echo phpversion(); ?>">
-->
<title>phorum - <?PHP if(isset($ForumName)) echo $ForumName; ?><?PHP echo initvar("title"); ?></title>
<link rel="STYLESHEET" type="text/css" href="<?PHP echo $include_path; ?>/phorum.css">
</head>
<body bgcolor="#FFFFFF" TEXT="#333366" LINK="#666699" ALINK="#ffcc33" VLINK="#669966" background="../images/back.gif">
<div align="center">
 <table border="0" cellpadding="0" cellspacing="0" width="100%">
  <tr>
   <td align="center"><img src="../images/exult_logo.gif" width="181" height="127" alt="Exult Logo"></td>
  </tr>
  <tr>
   <td>&nbsp;</td>
  </tr>
  <tr>
   <td align="center"><b>
     <a href="../../index.php">Home</a> |
     <a href="../../docs.php">Documentation</a> |
     <a href="../../download.php">Download</a> |
     <a href="../../faq.php">FAQ</a> |
     <a href="../../screenshots.php">Screen Shots</a>
   </b></td>
  </tr>
  <tr>
   <td align="center"><b>
     <a href="../../dev.php">Development</a> |
     <a href="index.php">Discussion</a> |
     <a href="../../about.php">About Us</a> |
     <a href="../../history.php">History</a> |
    <a href="../../links.php">Links</a>
   </b></td>
  </tr>
  <tr>
   <td>&nbsp;</td>
  </tr>
 </table>
 <table border="0" cellpadding="0" cellspacing="0" width="90%">
  <tr>
   <td><font class=PhorumForumTitle><b><?PHP echo $ForumName; ?></b></font></td>  </tr>
  <tr>
   <td>Before posting, make sure you've read the <a href="../../faq.php">FAQ</a>
       and searched the message board for previous discussions. When reporting
       problems/bugs, please include details about your setup (Exult version,
       OS, sound and video cards).
   </td>
  </tr>
 </table>
</div>
<div align="center">
<br>
