#!/bin/sh
cd autoRunWhenBootUp
cp bootupServer /etc/init.d/bootupServer
cp bootup.sh /usr/bin/bootup.sh
cp ../output/UsbAutoMounted /usr/bin/UsbAutoMounted

chmod 755 /etc/init.d/bootupServer
chmod 755 /usr/bin/bootup.sh
chmod 755 /usr/bin/UsbAutoMounted
update-rc.d bootupServer defaults
