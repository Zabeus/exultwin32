<?php check_security(); ?>
<?php
  if (!empty($new_AttachmentDir) && !file_exists($new_AttachmentDir)) {
    if(!@mkdir($new_AttachmentDir, 0777)){
      QueMessage("The directory you entered '$new_AttachmentDir' could not be created.  Attachments have been disabled until the problem is resolved.");
      $new_AllowAttachments=0;
      $page="attachments";
    }
  }
  elseif($new_AllowAttachments && empty($new_AttachmentDir)){
    QueMessage("You did not provide a directory for attachments.  Attachments have been disabled.");
    $new_AllowAttachments=0;
    $page="attachments";
  }
  else{
    $AttachmentDir = $new_AttachmentDir;
    $AllowAttachments = $new_AllowAttachments;
    $AttachmentSizeLimit = $new_AttachmentSizeLimit;
    $AttachmentFileTypes = $new_AttachmentFileTypes;
    $MaximumNumberAttachments=(int)$new_MaximumNumberAttachments;
    writefile();
    QueMessage("The Attachment settings have been updated.");
  }
?>