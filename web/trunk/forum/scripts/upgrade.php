<xmp>
<?php
// Please read the instructions in docs/updgrade.txt before using this file.

// Please note that this script only works for upgrading from 3.1 or newer

  chdir("../");
  include "common.php";

  echo "Altering table $pho_main\n";
  flush();
  $SQL="ALTER TABLE $pho_main change id id int UNSIGNED DEFAULT '0' NOT NULL AUTO_INCREMENT";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main change parent parent int UNSIGNED DEFAULT '0' NOT NULL AUTO_INCREMENT";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main change display display int UNSIGNED DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main change check_dup check_dup smallint unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main change multi_level multi_level smallint(5) unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main change collapse collapse smallint(5) unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main change flat flat smallint(5) unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD allow_uploads char(1) DEFAULT 'N' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD email_list char(50) DEFAULT '' NOT NULL after mod_pass";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD email_return char(50) DEFAULT '' NOT NULL after email_list";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD email_tag char(50) DEFAULT '' NOT NULL after email_return";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD config_suffix char(50) DEFAULT '' NOT NULL after description";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD upload_types char(100) DEFAULT '' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD upload_size int unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD max_uploads int unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);
  $SQL="ALTER TABLE $pho_main ADD security int unsigned DEFAULT '0' NOT NULL";
  $q->query($DB, $SQL);

  $SQL="Select id, name, table_name from $pho_main WHERE folder = '0'";
  $query = new query($DB, $SQL);

  $rec=$query->getrow();

  while(is_array($rec)){
    echo "Altering tables for $rec[name]\n";
    flush();
    $SQL="ALTER TABLE $rec[table_name]_bodies CHANGE id id int unsigned DEFAULT '0' NOT NULL auto_increment";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name]_bodies CHANGE thread thread int unsigned DEFAULT '0' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] CHANGE id id int unsigned DEFAULT '0' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] CHANGE thread thread int unsigned DEFAULT '0' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] CHANGE parent parent int unsigned DEFAULT '0' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] CHANGE subject subject char(255) DEFAULT '' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] CHANGE email email char(200) DEFAULT '' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] ADD attachment char(64) DEFAULT '' NOT NULL AFTER email";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] ADD msgid char(100) DEFAULT '' NOT NULL, ADD KEY msgid (msgid)";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] ADD modifystamp int(10) unsigned DEFAULT '0' NOT NULL";
    $q->query($DB, $SQL);
    $SQL="ALTER TABLE $rec[table_name] ADD KEY modifystamp (modifystamp)";
    $q->query($DB, $SQL);
    $rec=$query->getrow();
  }
?>
</xmp>