<xmp>
<?php
// Please note that this script only works for upgrading from 3.1.x or newer.
// For earlier versions in the 3.0 tree, use upgrade30.php.  There is currently
// no upgrade path for 1.x versions.

  chdir("../");
  include "common.php";

  echo "Altering table $pho_main\n";
  flush();
  $SQL="ALTER TABLE $pho_main ADD allow_uploads char(1) DEFAULT 'N' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD email_list char(50) DEFAULT '' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD email_return char(50) DEFAULT '' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD email_tag char(50) DEFAULT '' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD config_suffix char(50) DEFAULT '' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD upload_types varchar(100) DEFAULT 'N' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD upload_size int4 DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD max_uploads int4 DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD security int4 DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);

  $SQL="Select id, name, table_name from $pho_main WHERE folder = '0'";
  $query = new query($DB, $SQL);

  $rec=$query->getrow();

  while(is_array($rec)){
    echo "Altering tables for $rec[name]\n";
    flush();
    $SQL="ALTER TABLE $rec[table_name] ADD attachment char(64) DEFAULT '' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] ADD msgid char(100) DEFAULT '' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] ADD modifystamp INT4 unsigned DEFAULT '0' NOT NULL";
    $q->query($DB, $SQL);
    $sSQL="CREATE INDEX ".$rec[table_name]."_msgid on ".$rec[table_name]."(msgid)";
    $q->query($DB, $SQL);
    $sSQL="CREATE INDEX ".$rec[table_name]."_modifystamp on $rec[table_name](modifystamp)";
    $q->query($DB, $sSQL);
    $rec=$query->getrow();
  }
?>
</xmp>