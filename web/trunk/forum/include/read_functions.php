<?php

  function format_body($body){
    global $ForumAllowHTML, $plugins;

    $body=str_replace("{phopen}", "", $body);
    $body=str_replace("{phclose}", "", $body);

    $body=eregi_replace("<(mailto:)([^ >\n\t]+)>", "{phopen}a href=\"\\1\\2\"{phclose}\\2{phopen}/a{phclose}", $body);
    $body=eregi_replace("<([http|news|ftp]+://[^ >\n\t]+)>", "{phopen}a href=\"\\1\" target=\"blank\"{phclose}\\1{phopen}/a{phclose}", $body);

    if($ForumAllowHTML!="Y" && substr($body, 0, 6)!="<HTML>"){
      $tags=explode("|", $ForumAllowHTML);
      while(list($key, $tag)=each($tags)){
        switch($tag){
            case "font":
            case "a":
            case "img":
                $tag="<(/*$tag( [^>]*)?)>";
                break;
            default:
                $tag="<(/*{$tag}[^[:alpha:]>]*)>";
                break;
        }
        $body=eregi_replace($tag, "{phopen}\\1{phclose}", $body);
      }
      $body=str_replace("<", "&lt;", $body);
      $body=str_replace(">", "&gt;", $body);
    }

    $body=str_replace("{phopen}", "<", $body);
    $body=str_replace("{phclose}\n", "{phclose}\n\n", $body);
    $body=str_replace("{phclose}", ">", $body);

    // exec all read plugins
    @reset($plugins["read_body"]);
    while(list($key,$val) = each($plugins["read_body"])) {
      $body = $val($body);
    }

    if(empty($ForumAllowHTML) && substr($body, 0, 6)!="<HTML>"){
      $body=nl2br($body);
    }
    else{
      $body=my_nl2br($body);
    }

    return $body;

  }

?>
