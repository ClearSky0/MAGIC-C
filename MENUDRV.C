/*Menudrv V4 GAG, 9-sept-88*/

#include<stdio.h>
#include<graph.h>
#include<malloc.h>
#include<dos.h>
#include<conio.h>
#include<process.h>
#include<time.h>

#include"c:\tp\magic-c\magic.h"

extern char menus[NOFMENUS][MENSTLEN];              /*string arrays for menus*/
extern char options[NOFMENUS][NOFOPTS][OPTSTLEN];   /*string arrays for opts*/

struct windowbox {                      /*ul and br coords of menu box*/
				 int x,y;
				 } windul,windbr;

/*END OF MENU SPECS*/

int mno=-1,ono=-1;                              /*mno=MENU NUMBER ono=OPT NUMBER*/
char far *menutitlebuf;     /*screen grabing for menu bar work*/
char far *oldoptbuf;        /*two screen buffers for option bars*/
char far *newoptbuf;
char far *restbuf;          /*restore buffer for replacing the contents of space occupied by menu*/
union REGS regsout;         /*gives access to mouse controll*/

/*prototypes*/
void main(void);
void showbar(void);
int menuchoice(void);
int optionchoice(int);
void showmenu(int,int);
int nofoption(int);
void killmenu(int);
void inversopt(int);
int uninversopt(int);
int getmenu(void);

extern int strcspn(char *,char *);

/*end of prototypes*/


int getmenu()               /*main function with menu program, returns menu No*/
{
	while(mstatus()==RIGHTBUT)
	{
			mno=menuchoice();
			ono=optionchoice(mno);
	}
	return(mno);
}


void showbar()  /*displays the menu bar with titles as defined in menus array*/
{
int curpos=3;   /*current screen position*/
int curmen=0;   /*current menu*/
	mhide();
	while(curmen<NOFMENUS)
	{
		if (curpos>70)              /*safety net, to stop menus going off screen*/
		{
			puts("\n\nToo many menus.");
			abort();
		}
		_settextposition(1,curpos);
		menuname[curmen].start=((curpos-1)*8)-1;                                /*two lines to work out the*/
		menuname[curmen].end=(((curpos-1)*8)-1)+(8*strcspn(menus[curmen]," ")); /*coords of the titles as printed*/
		printf("%s",menus[curmen]);
		curpos+=(strcspn(menus[curmen]," ")+2);
		curmen++;
	}
	if(menutitlebuf!=NULL) free((char *)menutitlebuf);
	menutitlebuf=(char far *)malloc((unsigned int)_imagesize(ULBARX,ULBARY,BRBARX,BRBARY));
	if (menutitlebuf==(char far *)NULL)
	{
		puts("\n\nUnable to alocate memory for buffer");
		abort();
	}
	_getimage(ULBARX,ULBARY,BRBARX,BRBARY,menutitlebuf);
	_putimage(ULBARX,ULBARY,menutitlebuf,_GPRESET);       /*inverts the bar*/
	mshow();
}


int menuchoice(void)    /*checks to see if the mouse right but has been clicked in the bar and within an option*/
						/*returns the menu choosen, -1 if no menu*/
{
int curmen;
int retval=-1;
	if (regsout.x.dx<12)
	{
		for (curmen=0;curmen<NOFMENUS;curmen++)     /*scan all menu positions*/
		{
			if (regsout.x.cx>menuname[curmen].start-2 && regsout.x.cx<menuname[curmen].end)
				retval=curmen;
		}
			return(retval);
	}
	else return(retval);
}


int optionchoice(int nofmenu)   /*scans the complete menu, and returns the option choosen, -1 if none*/
{
int ono=0;
int curopt,curopt2;
	if (nofmenu==-1) return(0);        /*check to see that a valid menu has been chosen*/
/*CALCULATES SIZE OF MENU BOX AND SIZE OF INDIVIDUAL OPTIONS FOR CURRENT MENU*/
	windul.x=menuname[nofmenu].start-5;
	windul.y=12;
	windbr.x=+windul.x+(OPTSTLEN+2)*8;
	for(curopt=NOFOPTS-1;curopt>=0;curopt--)
	{
		if(options[nofmenu][curopt][0]!=NULL)
		{
			windbr.y=4+windul.y+((curopt+1)*14);
			break;
		}
	}                               /*MENU SIZE*/
	for(curopt2=0;curopt2<NOFOPTS;curopt2++)
	{
		optul[curopt2].x=windul.x+2;
		optul[curopt2].y=14*(curopt2+1);
		optbr[curopt2].x=windbr.x-2;
		optbr[curopt2].y=14*(curopt2+2);
	}                               /*OPTION LOCATIONS*/
/*END OF CALCULATION*/
	showmenu(nofmenu,curopt+1);
	ono=nofoption(nofmenu);
	killmenu(nofmenu);
	return(ono);
}


void showmenu(int nofmenu,int lastopt)  /*DISPLAYS MENU nofmenu*/
{
int curopt;
	mhide();
	if (menutitlebuf!=NULL) free((char *)menutitlebuf);
	menutitlebuf=(char far *)malloc((unsigned int)_imagesize(menuname[nofmenu].start,ULBARY,menuname[nofmenu].end,BRBARY));
	if (menutitlebuf==(char far *)NULL)
	{
		puts("\n\nUnable to alocate memory for buffer");
		abort();
	}
	_getimage(menuname[nofmenu].start,ULBARY,menuname[nofmenu].end,BRBARY,menutitlebuf);
	_putimage(menuname[nofmenu].start,ULBARY,menutitlebuf,_GPRESET);     /*INVERTS THE MENU NAME ON MENU BAR*/
	if (restbuf!=NULL) free((char *)restbuf);
	restbuf=(char far *)malloc((unsigned int)_imagesize(windul.x-1,windul.y-1,windbr.x+1,windbr.y+1));
	if (restbuf==(char far *)NULL)
	{
		puts("\n\nUnable to alocate memory for buffer");
		abort();
	}
	_getimage(windul.x-1,windul.y-1,windbr.x+1,windbr.y+1,restbuf);
	_putimage(windul.x-1,windul.y-1,restbuf,_GXOR);     /*INVERTS THE WHOLE MENU PLUS 1 IN ALL DIRECTIONS*/
	_rectangle(_GBORDER,windul.x,windul.y,windbr.x,windbr.y);
	_rectangle(_GBORDER,windul.x,windul.y,windbr.x,windbr.y);
	for (curopt=0;curopt<lastopt;curopt++)              /*PRINTS ALL OPTIONS*/
	{
		_settextposition(2+curopt,2+(menuname[nofmenu].start)/8);
		printf("%s",options[nofmenu][curopt]);
	}
	mshow();
}


void killmenu(int nofmenu)      /*REMOVES MENU FROM SCREEN RESTORING ALL THAT WAS UNDERNEATH*/
{
int curopt;
	mhide();
	_putimage(windul.x-1,windul.y-1,restbuf,_GPSET);                /*REMOVES MENU AND RESTORES*/
	_putimage(menuname[nofmenu].start,ULBARY,menutitlebuf,_GPSET);   /*REINVERT MENU NAME*/
	mshow();
}


int nofoption(int nofmenu)      /*SCANS MOUSE UNTIL RIGHT BUT RELEASED
								INVERTS OPTION THE MOUSE IS OVER AND RETURNS OPTION WHERE BUTTON WAS RELEASED*/
{
int oldopt=-1,newopt=-1,curopt=0,retval=-1;
time_t fin,now;             /*USED ONLY TO PAUSE WHILE OPTION CHOOSEN FLASHES*/
	mdomain(windul.x,SCRULY,windbr.x-3,windbr.y-3);      /*STOPS THE MOUSE LEAVING THE MENU CURRENTLY ACTIVE*/
	while(mrelease(1)==RIGHTBUT)
	{
		mstatus();
		if (regsout.x.dx<12&&newopt!=-1) newopt=-1;
		if (regsout.x.dx>=optul[curopt].y&&regsout.x.dx<optbr[curopt].y&&newopt!=curopt)
		{
			inversopt(curopt);
			newopt=curopt;
		}
		if (oldopt!=newopt)
		{
			uninversopt(oldopt);
			oldopt=newopt;
		}
		curopt++;
		if (curopt>=NOFOPTS) curopt=0;
	}
/*WHILE THE BUTTON IS HELD DOWN, THE PIECE OF CODE ABOVE CHECKS TO SEE
	WHICH OPTION THE MOUSE IS OVER AND INVERTS THE CURRENT ONE*/

/*ONCE RELEASED THIS CODE AGAIN SCANS THE LOCATION OF THE MOUSE AND SETS THE
	RETURN VALUE TO THE OPTION WHERE THE BUTTON WAS RELEASED*/
	for (curopt=0;curopt<NOFOPTS;curopt++)
	{
		if (regsout.x.dx>=optul[curopt].y&&regsout.x.dx<optbr[curopt].y)
		{
		retval=curopt;
		break;
		}
		else retval=-1;
	}

/*AS LONG AS THE BUTTON WAS NOT RELEASED ON THE MENU BAR OR BOTH BUTTONS WHERE
PRESSED, THE CODE BELOW FLASHES THE SELECTED OPTION*/
	if (retval!=-1&&mstatus()!=BOTHBUT)
	{
		mhide();
		if (oldoptbuf!=NULL) free(oldoptbuf);
		oldoptbuf=(char far *)malloc((unsigned int)_imagesize(optul[retval].x,optul[retval].y,optbr[retval].x,optbr[retval].y));
		if (oldoptbuf==NULL) puts ("unable to malloc in flash option");
		_getimage(optul[retval].x,optul[retval].y,optbr[retval].x,optbr[retval].y,oldoptbuf);
		for(curopt=0;curopt<4;curopt++)
		{
			fin=time(&fin);
			fin+=(time_t)0.1;
			while(difftime(fin,time(&now))>0.0);
			_putimage(optul[retval].x,optul[retval].y,oldoptbuf,_GXOR);
		}
		mshow();
	}
	mdomain(SCRULX,SCRULY,SCRBRX,SCRBRY);        /*RESTORES THE MOUSE DOMAIN TO WHOLE SCREEN*/
	return(retval);
}


void inversopt(int optn)    /*INVERT ACTIVE OPTION*/
{
	if (newoptbuf!=NULL) free(newoptbuf);
	newoptbuf=(char far *)malloc((unsigned int)_imagesize(optul[optn].x,optul[optn].y,optbr[optn].x,optbr[optn].y));
	if (newoptbuf==NULL) puts ("unable to malloc in inversopt");
	mhide();
	_getimage(optul[optn].x,optul[optn].y,optbr[optn].x,optbr[optn].y,newoptbuf);
	_putimage(optul[optn].x,optul[optn].y,newoptbuf,_GPRESET);
	mshow();
}


int uninversopt(int optn)   /*RESTORE UNACTIVE OPTION*/
{
	if (optn==-1) return(0);
	if (oldoptbuf!=NULL) free(oldoptbuf);
	oldoptbuf=(char far *)malloc((unsigned int)_imagesize(optul[optn].x,optul[optn].y,optbr[optn].x,optbr[optn].y));
	if (oldoptbuf==NULL) puts ("unable to malloc in inversopt");
	mhide();
	_getimage(optul[optn].x,optul[optn].y,optbr[optn].x,optbr[optn].y,oldoptbuf);
	_putimage(optul[optn].x,optul[optn].y,oldoptbuf,_GPRESET);
	mshow();
}
