/*fileopts.c*/

#include<graph.h>
#include<process.h>
#include<stdio.h>
#include<bios.h>
#include<malloc.h>
#include<conio.h>
#include<fcntl.h>
#include<dos.h>
#include<signal.h>
#include<io.h>
#include<string.h>

#include"c:\tp\magic-c\magic.h"

void filemenu(void);
void gotodos(void);
void leaveprog(void);
void savescreen(void);
void restorescreen(void);
void loadnewfont(void);
void readfont(void);
void save(void);
void load(void);
char *filesell(void);


extern void setbp(unsigned int,unsigned int);

extern void refresh(void);

extern ono;
extern wand;
extern curobj;
char *screenbuf;
char *filespec="picture.mgp";
unsigned int bp,es;
unsigned char far *font;

void filemenu()     /*CONTROL STRUCTURE OF FILEOPTS*/
{
	switch(ono)
	{
		case 0:
				save();
				break;
		case 1:
				load();
				break;
		case 2:
				gotodos();
				break;
		case 3:
				leaveprog();
				break;
	}
}


void leaveprog()    /*QUIT RESTORE SCREEN AND STUFF*/
{
	_setvideomode(_DEFAULTMODE);
	mhide();
	signal(SIGINT,SIG_DFL);     /*restore control-c, NOT HAPPY WITH THIS*/
	free(font);
	puts("Goodbye!");
	exit(0);
}

void gotodos()  /*DOS SHELL*/
{
	mhide();
	savescreen();
	loadnewfont();

#ifndef M_I86LM     /*IF NOT LARGE WARN ABOUT LACK OF SCREEN SAVING ROUTINE*/
	puts("\n\nThere is a screen saving routine, but it only works outside the\nintegrated environment.");
#endif

	puts("\n\nNow entering DOS type EXIT to return.\n\n");  /*THIS IS PRINTED IN THE NEW FONT*/
	spawnl(P_WAIT,"C:\\COMMAND.COM","C:\\COMMAND.COM",NULL);    /*SPAWN OF COMMMAND.COM FROM A:*/
	restorescreen();
#ifdef M_I86LM
	mnewpointer(WANDXHOT,WANDYHOT,&wand);   /*RESTORE POINTER TO WAND*/
#endif
	mshow();
}

void savescreen()   /*SAVE WHOLE SCREEN ONLY WORKS IN LARGE MEMORY MODEL*/
{
	screenbuf=(char *)malloc((unsigned int)_imagesize(SCRULX,SCRULY,SCRBRX,SCRBRY));
	if (screenbuf==(char *)NULL)
	{
		puts("\n\nError in allocating whole screen buffer!");

#ifndef M_I86LM
		puts("\n\nI am not Large!");    /*EXPLAINS WHY IF THE PROGRAM IS NOT LARGE*/
#endif

		while(!kbhit());
	}
	else _getimage(SCRULX,SCRULY,SCRBRX,SCRBRY,screenbuf);
	_setvideomode(_DEFAULTMODE);
}

void restorescreen()    /*PUTS WHOLE SCREEN BACK AGAIN*/
{
	_setvideomode(_ERESNOCOLOR);
	_putimage(SCRULX,SCRULY,screenbuf,_GPSET);
	free((char *)screenbuf);
}

void loadnewfont()  /*LOADS NEW EGA TEXT FONT INTO MEMORY*/
{
static char done=0;
#ifdef M_I86LM      /*ONLY MAKES AN ATTEMPT IF THE PROGRAM IS LARGE*/
	if(!done)   /*ONLY READS THE FONT IN ONCE*/
	{
		readfont();
		done=1;
	}
	es=FP_SEG(font);    /*ONLY WORKS IF LARGE*/
	bp=FP_OFF(font);
	setbp(bp,es);       /*EXTERNAL FUNCTION WRITTEN IN MACHINE LANGUAGE TO ALLOW ACCESS TO BP REGISTER*/
#endif
}

void readfont()         /*READS FONT IN TO font ARRAY FROM HARD DISK*/
{
long len;
int numread;
FILE *stream;
	if ((stream=fopen("c:\\ega\\courier.fnt","rb"))!=NULL)  /*ATTEMPTS TO OPEN FONT*/
	{
		len=filelength(fileno(stream));                             /*ASCERTAIN LENGTH OF FILE*/
		font=(unsigned char *)malloc((size_t)len*sizeof(char));     /*ALLOCATE MEMORY FOR FONT*/
		numread=(int)fread((char *)font,1,(size_t)len,stream);      /*READS THE FONT*/
		if(len!=numread) puts("\n\nError in reading font");         /*CHECK RESULT OF READ*/
		fclose(stream);                                             /*CLOSES THE FILE*/
	}
	else
	{
		puts("Error in opening file.");     /*DISPLAYS AN ERROR AND EXIT THE PROGRAM IF UNABLE TO OPEN*/
		abort();
	}
}

void save()     /*SAVES THE COMPLETE STRUCTURE CONTAINING ALL GRAPHICS OBJECTS UP TO DATE*/
{
FILE *stream;
int num;
	stream=fopen(filespec,"wb");    /*OPENS FILE*/
	if(stream!=NULL)                /*CHECK IT*/
	{
		/*IF ITS OPEN WRITE A HEADER CONTAINING THE LENGTH OF THE FILE IN OBJECTS AND ALL THE EXISTING OBJECTS*/
		num=fwrite(&curobj,sizeof(long),1,stream);
		num=fwrite(picobject,sizeof(struct PICTUREOBJECT),curobj,stream);
		fclose(stream);             /*CLOSE THE FILE*/
	}
	else printf("\nerror in opening file for output");  /*DISPLAY AN ERROR SAYING THE THE FILE CANNOT BE OPENED*/
}

void load()     /*LOADS WHOLE STRUCTURE OF OBJECTS*/
{
FILE *stream;
int num;
	curobj=-1;  /*NO CURRENT OBJECTS*/
	refresh();  /*CLEARS THE SCREEN SINCE THERE ARE NO OBJECTS*/
	filesell();
	stream=fopen(filespec,"rb");    /*OPEN FOR READING IN BINARY*/
	if(stream!=NULL)    /*IF ITS OPEN*/
	{
		num=fread(&curobj,sizeof(long),1,stream);   /*READ THE HEADER*/
		num=fread(picobject,sizeof(struct PICTUREOBJECT),curobj,stream);    /*READ THE NUMBER OF OBJECTS SPECIFIED BY THE HEADER*/
		fclose(stream);     /*CLOSE THE FILE*/
	}
	else
	{
		printf("\nerror in opening file for input");    /*ERROR THAT FILE COULD NOT BE OPENED*/
	}
	refresh();  /*DISPLAY THE NEWLY LOADED OBJECTS*/
}

char *filesell()
{
char filenames[20][13];
struct find_t info;
int c=1,d;
	_dos_findfirst("*.mgp",_A_NORMAL,&info);
	strcpy(filenames[0],info.name);
	while(c<21 && _dos_findnext(&info)==0)
	{
		strcpy(filenames[c],info.name);
		c++;
	}
	for(d=0;d<c;d++)
	{
		_settextposition(d+3,5);
		printf("%s",filenames[d]);
	}
	while(!mstatus());
}
