#!/bin/sh
# edit regex below for files to exclude...

if [ $# -ne 1 ]; then
   echo 1>&2 Usage: $0 dirname
   echo "error: need to specify directory to archive"
   echo ""
   echo "   makes a .tgz file that unpacks one directory "
   echo "   called mydir (with all the files/dirs inside)"
   exit 127
fi

if [ -e "$1.tgz" ]; then
	echo "file exists, hit a key to continue"
	read pressakey
fi

find $1 -print | egrep '/,|%$|~$|\.suo$|\.tgz$|\.swp$|\.svn|\.ncb$|Utils\.h$|log\.txt$|\.exe$|\.lib$|\.pdf$|Exclude$|Debug$|Release$|/core$|\.o$|\.obj$' > Exclude
tar zcvfX "$1.tgz" Exclude "$1"


# exclude
#find project ! -type d -print | egrep '/,|%$|~$|\.old$|SCCS|/core$|\.o$|\.orig$' > Exclude
#tar cvfX project.tar Exclude project

#include
#find project -type f -print | egrep '(\.[ch]|[Mm]akefile)$' > Include
#tar cvf project.tar -I Include

