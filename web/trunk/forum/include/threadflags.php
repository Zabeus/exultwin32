<?php

  function modify_threadflags($thread, $threadflags)
  {
    GLOBAL $PHORUM, $DB, $q;

    $SQL="UPDATE $PHORUM[ForumTableName] SET threadflags = '$threadflags' WHERE thread = '$thread'";

    $q->query($DB, $SQL);
  }
?>
