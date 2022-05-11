#
# Program: Magigraf
#

.c.obj:
	cl -c  -W3 -Ze -Zid -Zr -AM $*.c

magigraf.obj : magigraf.c

menudrv.obj : menudrv.c

mousedrv.obj : mousedrv.c

fileopts.obj : fileopts.c

drawopts.obj : drawopts.c

prefopts.obj : prefopts.c

Magigraf.exe : magigraf.obj menudrv.obj mousedrv.obj fileopts.obj  \
		drawopts.obj prefopts.obj 
	del Magigraf.lnk
	echo magigraf.obj+ >>Magigraf.lnk
	echo menudrv.obj+ >>Magigraf.lnk
	echo mousedrv.obj+ >>Magigraf.lnk
	echo fileopts.obj+ >>Magigraf.lnk
	echo drawopts.obj+ >>Magigraf.lnk
	echo prefopts.obj  >>Magigraf.lnk
	echo Magigraf.exe >>Magigraf.lnk
	echo Magigraf.map >>Magigraf.lnk
	link @Magigraf.lnk /NOI $(LDFLAGS);
