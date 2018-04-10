all:
	g++ -D_DEBUG main.cpp -ospew.exe
	g++ -D_DEBUG AssertTest.cpp -oat.exe


CWD = ../$(shell echo `pwd` | sed 's/.*\///')
tgz:
	./tarballer.sh ${CWD};mv ${CWD}.tgz ./; rm Exclude

