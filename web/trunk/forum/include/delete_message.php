<?php

    function delete_messages($ids)
    {
        GLOBAL $PHORUM, $DB, $q;

	if(!is_array($ids)) $id_array=explode(",", $ids);

	// get all message-ids involved
        $SQL="Select id from $PHORUM[ForumTableName] where id in ($ids)";
        $q->query($DB, $SQL);

        while(list($key, $id)=each($id_array)){
            $lists[]=_get_message_tree($id);
        }

        $lists=implode(",", $lists);
        $arr=explode(",", $lists);

	// get all involved threads
        $SQL="SELECT DISTINCT(thread) from $PHORUM[ForumTableName] where id in ($lists)";
        $q->query($DB, $SQL);
        while($rec=$q->getrow()){
                $threads[]=$rec['thread'];
        }
        $threads=implode(",",$threads);

	// delete headers
        $SQL="Delete from $PHORUM[ForumTableName] where id in ($lists)";
        $q->query($DB, $SQL);

	// delete bodies
        $SQL="Delete from $PHORUM[ForumTableName]_bodies where id in ($lists)";
        $q->query($DB, $SQL);

	// delete attachments
        $SQL="Select message_id,id,filename from $PHORUM[ForumTableName]_attachments where message_id in ($lists)";

        $q->query($DB, $SQL);

        while($rec=$q->getrow()){
            $filename="$PHORUM[AttachmentDir]/$PHORUM[ForumTableName]/$rec[message_id]_$rec[id]".strtolower(strrchr($rec["filename"], "."));
            unlink($filename);
        }

	// delete attachments from attachments-table
        $SQL="Delete from $PHORUM[ForumTableName]_attachments where message_id in ($lists)";
        $q->query($DB, $SQL);

	// reset the modifystamp
        $SQL="SELECT thread,max(datestamp) as datestamp from $PHORUM[ForumTableName] where thread in ($threads) group by thread";
	$q->query($DB, $SQL);

	$q2= new query($DB);

        while($rec=$q->getrow()){
	        list($date,$time) = explode(" ", $rec["datestamp"]);
	        list($year,$month,$day) = explode("-", $date);
	        list($hour,$minute,$second) = explode(":", $time);
	        $tstamp = mktime($hour,$minute,$second,$month,$day,$year);
	        $SQL="update $PHORUM[ForumTableName] set modifystamp=$tstamp where thread=$rec[thread]";
	        $q2->query($DB, $SQL);	
        }

    }

    function _get_message_tree($id)
    {
        global $PHORUM, $DB;
	$q = new query($DB);
        $SQL="Select id from $PHORUM[ForumTableName] where parent=$id";
        $q->query($DB, $SQL);
        $tree="$id";
        while($rec=$q->getrow()){
            $tree.=","._get_message_tree($rec["id"]);
        }
        return $tree;
    }

?>
