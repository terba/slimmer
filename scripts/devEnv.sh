#!/bin/sh

sudo chmod o+r /dev/input/event2
~/compile/logitechmediaserver-7.8.1-1442495869/slimserver.pl --daemon --debug
~/bin/squeezelite -z -s localhost
#luit -encoding ISO-8859-2
/usr/sbin/LCDd -c ~/svn/slimmer/scripts/LCDd.dev.conf 2> /dev/null
