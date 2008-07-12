#!/bin/sh

echo "Updating from SVN..."
svn update

echo "Fixing permissions"
chgrp -R exult .
chmod -f -R g+rw,a+r .

