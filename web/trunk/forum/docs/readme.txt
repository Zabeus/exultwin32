Phorum
http://www.phorum.org

If you are upgrading an existing Phorum, see one of the upgrade*.txt files.

**********************************************
*  READ security.txt!!!!!!!!!!!!!!           *
**********************************************

Contents:

0. PRE-Installation
1. Installation
2. Creating a forum.
3. Features
4. Support
5. Add-on Scripts
6. Upgrading

****************************
*** 0. PRE-Installation  ***
****************************

    Read security.txt after you complete the installation.

 1. Make sure you have PHP (www.php.net) installed.  We recommend version 3.0.12
    or higher.  Phorum has been known to work with 3.0.6, but we have also seen
    problems with this version.  Phorum 3.2.x was tested with version 4.0.3.

 2. Make sure you have one of the supported databases installed.  At the time
    this was written the ones tested by the Phorum team are:
    MySQL version 3.22.x or higher. (www.tcx.se)
    PostgreSQL 6.4.1 or higher (www.postgresql.org)

    There are also database modules for other databases.  These are not tested
    by the Phorum team.

    Sybase SQL Server
    Microsoft SQL Server

 3. If you do not have a database already, create a database.  In MySQL you
    use the "mysqladmin" program like this:

    mysqladmin -uuser -ppaswword create mydb

 4. Be sure that you have a user for that database which has the rights to
    select, insert, update, delete, create, alter and drop.  A MySQL Grant
    statment for this user would look like this:

    GRANT
      select, insert, update, create, alter, delete, drop
    ON
      database.*
    TO
      user@localhost
    IDENTIFIED BY
      'password';


************************
*** 1. Installation  ***
************************

    Read security.txt after you complete the installation.

******* There is a script in the scripts dir called install.sh.  If you are on
******* a Unix system please use this script to set up Phorum.  This script
******* make many of the following steps unnecessary.

 1. Edit common.php.

    a) Change $inf_path to the dir where you will put forums.php.  See
       security.txt for more on why you should move it.

    b) Change $include_path to the dir where you will put the included
       files.  There is no real risk in leaving these where they are.
       If you are uncomfortable about people possibly reading code on
       your server you may want to move them.  Simply changing the dir
       name to something unique would do.

    c) Change $admindir to the dir where you will put the admin files.  See
       security.txt for more on why you should move it and how to protect it.
       If you move the admin directory to somewhere other than one level down
       from the current location, see security.txt for details on how you must
       change the admin/index.php to reflect these changes.

    e) Set the name ($admin_page) of the main admin script (index.php by default).

    f) Configure Phorum to interface with your DBMS.

       This means uncommenting the line in common.php that corresponds to your
       database system.  MySQL is selected by default.  The variable that needs
       to be changed is $db_file

       NOTE FOR POSTGRESQL USERS:  If you are using PostgreSQL 6.5 or newer use
       the postgresql65.php file.  Others use the postgresql.php file.

    g) If you want to use a different base tablename that "forums" change the
       value of $pho_main to reflect this.

 2. Give write permissions to the webserver on the admin/forums dir.

     > cd admin
     > chmod 707 forums

    This is only secure if you are on a dedicated server.  If you are on a
    shared server, see security.txt for more detail on securing your files.

 3. Give write permissions to the webserver on the configuration files.

     > cd [inf_path]
     > chmod 707 forums.php
     > chmod 706 forums.bak.php

    This is only secure if you are on a dedicated server.  If you are on a
    shared server, see security.txt for more detail on securing your files.

 4. Move the files into your web tree.  Be sure you move forums.php and
    forums.php.bak to the location you designated in common.php for
    $inf_path.  By default these files are in the include dir.

    NOTE: If this is a first time install you will need to rename the following:
      forums.php-dist => forums.php
      forums.bak.php-dist => forums.bak.php
      header.php-dist => header.php
      footer.php-dist => footer.php

    In most cases, changes to your existing forums.php can be included by
    upgrading the admin files and selecting "Rebuild INF file".

 5. Secure forums.php and forums.php.bak according to your setup as laid out
    in security.txt.

 6. Goto the admin from a web browser.  This is admin/index.php by default.

 7. Now select 'Main'.  Then, select 'Change Password'.  Fill in your new
    password and hit Update.

 8. Select 'Phorum Setup' and then 'Database Settings' from the menu.  Enter the
    database information you are asked for and hit update.  The admin will still
    report that there is no database connection.  This should go away on the
    next screen.

 9. Select 'File/Path Settings'.  If you plan on changing any of the file names
    or extensions of the files do it now.  Also, the forum url should be filled
    in for you.  Do not change it unless the URL you are currently accessing the
    admin from is different from the publicly accessed Phorum installation. Hit
    update.

10. Now select 'Global Settings'.  Once there you must fill in the default
    email and select a language file.  Hit update.

    Phorum is now ready to be used.

*************************************
*** 2. Creating a forum or folder ***
*************************************

 1. Select 'New Forum' or 'New Folder'.

 2. Fill out this form completely and hit 'Update'.  If you are unsure of
    some information, consult your systems admin.

    Field                  Description
    ------------------------------------------------------------------------
    Name                   This is the name of the Forum that user will see.  This
                           is a required field

    Description            This will appear under the forum name and will tell
                           users about the subject matter of the forum.  This is a
                           required field

    Config Suffix          This is used to allow seperate header, footer,
                           censor and bad* files.  See Other Features for more.

    Folder                 This is the folder under which this forum will be
                           listed.  If no folders are created the only options is
                           Top Level which means the forum will appear on the first
                           forum listing page.

    Language               Select the language for the forum.  Additional language files
                           can be found at the Phorum web site.

    Table Name             This is the table name that will store the messages
                           for this forum.  This is a required field, and if it
                           is a new table, it must not collide with any existing
                           table names

    Table already exists   Check this if the table name already exists for this
                           forum.

    Moderator Password     This must be given even if there is no moderation.
                           When filled into the email box of the post form, the
                           moderators post will be bolded and HTML will not be
                           stripped.  This is a required field.

    Security               There are currently 4 security options for a forum.
                           1. Public - anyone can read and post and no login is allowed.
                           2. Login Optional - people can login and their nick will
                                               be reserved.
                           3. Login To Post - People must have a login to post.
                           4. Login To Read - The highest level, any access to the forum
                                              requires a login.




    Table Width            This value sets the table width of all tables with
                           exception of the post table.

    Colors:

        Table Header Color           Color in hex value of the table headers.

        Table Header Font Color      Color in hex value of the table header font.

        Main Table Body Color        Color in hex value of the main table bodies.

        Main Table Body Font Color   Color in hex value of the main table fonts.

        Alt. Table Body Color        Color in hex value of the alt table bodies.

        Alt. Table Body Font Color   Color in hex value of the main table font.

        Navigation Background Color  Color in hex value of the navigation background

        Navigation Font Color        Color in hex value of the navigation font.



    Duplicate Posts        Phorum can eliminate some duplicate posts.  This does
                           require and extra couple of queries.

    Messages Per Page      This numer determines the minimum messages that are
                           shown on a page.  Phorum stops displaying messages
                           when this number is met and the last thread is
                           completely displayed.

    Thread Type            There are 3 options:
                           1. Float to Top - Threads are moved to the top of the
                                             listing when replies are posted.
                           2. Multi-level - Creates a threaded discussion.
                           3. Single Level - all replies are posted to the end
                                              of their thread.

    Thread Display         You can choose for the default view of the forum message
                           list to be collapsed (only the first message is shown) or
                           expanded ( the entire thread is shown).  It is recommended
                           that collapsed be used when using a Thread Type of
                           Float To Top as threads can become long.

    Read Messages          Thread can be read in two ways.
                           1. Entire Thread At Once - All messages appear on one page
                           2. One At A Time - Each message is alone with nav to other
                                              messages in the thread.

    HTML                   These check boxes allow the use of HTML in messages.  This is
                           not a great idea but hey, it is your forum.  Checking allow all
                           will disable all HTML filtering and any posted HTML will be sent
                           to the browser.  Otherwise, check the ones you want to allow.


    Moderation             Phorum has 3 moderation options.
                           1. None - all messages are available as soon as they are posted.
                           2. All Messages Before Posted - messages must be approved before
                                                           they are available.
                           3. All Messages After Posted - messages are available after
                                                          posted but the moderator is sent
                                                          and email for possible removal.

    Moderator
    Host Replacement       When posts are made in Phorum, the IP address or HOST name of
                           the poster is logged and partially displayed on the read page.
                           Moderators may not want that to happen to them.  Perhaps several
                           moderators want to appear as one.  This field allows you to provide
                           text to be put in place of that IP address on the read page.
                           Example: "site.com staff"

    Moderator Email        This is the email address where moderation emails are sent if
                           a moderation other than none is selected.

    Allow attachments?     Phorum can allow users to "attach" files to their posts.

    Attachment Size Limit  This is the maximum size in K that one attachment can be.

    Allowed File Types     These are allow file extensions (gif, jpg, etc...) that can be
                           attached to a post.  The list is setup in the Attachment Settings
                           from the main admin menu.

    Maximum #
    of Attachments         Phorum will allow more than one attachments.  This number is
                           capped at the limit set in Attachment Settings.


    Mailing List Address   If you would like to have all your Phorum posts sent out to a
                           mailing list or any address for that matter you can enter it
                           here.

    Mailing List Return    This address will appear as the From and Reply-To for messages
                           sent to the above address.  See the file script/phorummail on
                           how you can setup a two way communication between Phourm and a
                           mailing list.

    Mailing List Tag       This is an optional piece of text that will be prepended to
                           the email subject of the above emails.
                           Example: [myforum]


Follow these steps to add more forums or folders.

************************
*** 3. Features      ***
************************

New Features in 3.3.x:

 1. User login.
 2. Moved TimeZone config from language file to config file (phorum-dist) and made
    it updateble via admin (global config).
 3. Complete use of CSS for fonts.  It was half and half before.

New Features in 3.2.x:

 1. Improved Phorummail. It is finally what it should be. Allows for proper
    message threading in email clients and avoids loops entirely.
 2. Improved install and upgrade scripts.  Unix users with access to the
    console no longer have to edit any files manually for a standard install.
 3. Added support for SYBASE and MSSQL.
 4. Added file attachment capabilities.  This is fairly basic support.  There
    are no restrictions of file size or type.
 6. Added optional multi-forum search (advsearch.php).  This file needs to be
    renamed to search.php if you want to let your users use it by default.  Either
    that or provide a link to it in the search.php page.
 7. Added plugin prototype.  Simple text-replacement plugin included.  This
    can be used for supporting things like emoticons (smilies) in messages,
    text shortcuts, etc.
 8. Added recent-admin code in the admin section.  No longer have to search for
    recent posts.

New Features in 3.1.x:

 1. New and improved admin.  Moderators do not have access to other forums or
    to main settings.  The UI is much improved.  More options on forums.  Admin
    code is more modular allowing easier modifications.  Options like the
    database server and language setting is a readable text value not a file
    name.

 2. The introduction of Folders.  Now not only can you have multiple forums, you
    can have folders that contain forums.  These can also contain more folders,
    and so on.  With version 3.1, folders can have separate config files such as
    unique headers/footers, etc...

 3. All messages in a thread can be viewed on one page. (flat mode)

 4. Forum URL's are shorter.

 5. Added latest reply date in collapsed mode.

 6. New search features allow for conditional searching, date ranges, and
    limiting the fields that are searched.

 7. New moderation features.  Now messages can be held until approved.  All
    messages will be mailed to the moderator for approval.  Simple urls will be
    provided in the email for the mdoerator to approve, delete, or edit the
    message.

 8. There can now be seperate bad* files and censor files for each forum.  See
    Other Features, #1 for instruction on creating these.

Other Features:

 1. Multiple forums on one engine.  You can create different header, footer,
    censor, and bad* files for different forums.  Simply fill in 'Config Suffix'
    when creating your forums with a string and then create files named
    header_suffix.inc where suffix is the string you gave the admin when
    creating the forum.

 2. Database independence.  Phorum currently supports MySQL and PostgreSQL.
    To support your databse, convert abstract.php to work with your db.  To
    change the db engine used by Phorum refer to step 1.2 above.

 3. Multi-leveled threading.  This is enabled by default, but can be set off if
    desired on a forum by forum basis.  Select the forum you want to enable it
    for, then select 'Edit Properties'.  Change Thread Type from Multiple Levels
    to Single Level

 4. Emailing readers when replies are posted to a thread.

 5. Limited HTML/URL linking.  The following text decoration tags are allowed
    in all posts: <b>,<u>,<i>,<ul>,<ol>,<li>.  Plus any URL surrounded by <>
    will be linked.  Full HTML use is allowed and can be enabled in the admin
    on a forum by forum basis.  Select the forum you want to enable it for,
    then select 'Edit Properties'.  Check the box under 'allow HTML' and hit
    'Update'.

 6. Take Phorum up and down from the admin.  This is great for maintenence.
    Select 'Down Phorum' from the main menu, enter the master password and hit
    'Login'.  All requests to the Phorum will now be sent to down.php.  To
    bring Phorum back up, select 'Up Phorum' from the main menu, enter the
    master password and hit 'Login'.

 7. Moderator Privileges.  When the master password or the forums moderator
    password is entered in place of the email address, the message is given
    moderator priveleges.  This includes: the message will be bolded in the
    message list; the host/ip will be replaced with the string specified by
    the moderator under 'when moderator post, replace host with' in the admin
    section for that forum. The post will also be given full HTML privileges.

 8. index, list, read, post, down, and violation file names as well as the
    extension are defined as variables.  This can be changed in the
    'Master Settings' section of the admin.
    NOTE: THIS DOES NOT CHANGE THE FILE NAMES.  YOU MUST DO THIS MANUALLY.

 9. User Banning.  You can ban users by email, name, or their IP/HOST.  See
    bad_*.inc for details of use.  These can be created for each forum.  See
    number 1 for details.

10. Allows for censored posting.  There are instructions in censor.php as to how
    to use it.  By default the f word and sh-t are censored for example.

11. Allows for disallowing service to a given author, email or domain.
    Entries in bad_names.inc, bad_hosts.inc and bad_emails.inc will be
    checked for when a message is posted.  If any are present, the user will
    be forwarded to violation.php.  There are instructions in those files
    as to how to use them.

12. Localized text (Multi-lingual).  Additional language files are available at
    http://www.phorum.org/local.php

13. Ability to quote text from original message.  This is not available if
    viewing the messages in "Flat View".

14. Colors, table sizes configurable in Admin

15. Control number of messages displayed in main list.

16. Edit a post or delete entire threads or individual messages.

17. Search Engine (needs improvement)

18. Collapsible threads.  This can be defaulted on or off in the forum admin
    section.

************************
*** 4. Support       ***
************************

Mailing Lists
------------------------------

You can send all support questions to phorum@phorum.org.
This list is the support mailing list. It is for general support, questions
and answers. If you are familiar with the PHP list then this is much like
that. To subscribe to this list, send and empty email to:

    phorum-subscribe@phorum.org

There are also two other lists for Phorum. The first is the announcements
mailing list. This is a moderated list and no spam will be sent through it. If
you would like to receive mailings about new releases or bug fixes, send an
empty email to:

    phorum-announce-subscribe@phorum.org

The last is the developers list. It is for discussion of development issues.
This is a new list. To subscribe send an empty email to:

    phorum-dev-subscribe@phorum.org


Phorum Mailing List Archive
------------------------------

The good people at Progressive Computer Concepts, Inc have started archiving
the phorum mailing lists. The URL's are:

Phorum Support List:

http://marc.theaimsgroup.com/?l=phorum&r=1&w=2

Phorum Developers List:

http://marc.theaimsgroup.com/?l=phorum-dev&r=1&w=2

Phorum Announcement List:

http://marc.theaimsgroup.com/?l=phorum-announce&r=1&w=2

Online Support Phorum
Once the technology is developed for communicating with Phorum via email,
there will be a support Phorum tied into the mailing list above. We have found
that better support is received via email.

*************************
*** 5. Add-on Scripts ***
*************************

There are several scripts in the scripts dir.  Instructions are included in each
file.

************************
*** 6. Upgrading     ***
************************

Upgrading from Phorum 1.x.

There is no direct path from 1.x to 3.1.  If it appears to be in demand, it may
be developed.

Upgrading from Phorum 3.x

Read docs/upgrade.txt for instructions.
