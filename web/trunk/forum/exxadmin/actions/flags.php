<?php

    require "$include_path/threadflags.php";

    modify_threadflags($mythread, $flags);

    QueMessage("Flags of thread $mythread have been updated<br>");

?>
