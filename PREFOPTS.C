/*Prefopts.c*/

#include<malloc.h>
#include<graph.h>
#include<stdio.h>
#include<stdlib.h>
#include<bios.h>
#include<conio.h>

#include"c:\tp\magic-c\magic.h"

union REGS regsout;

void prefmenu(void);
void mrat(void);

extern ono;

void prefmenu()     /*CONTROL OF PREFOPTS*/
{
	switch(ono)
	{
		case 0:
				mrat();
				break;
	}
}

void mrat()         /*FUNCTION TO DISPLAY AN INTERACTIVE IF SOMEWHAT ABSTRACT METHOD OF ALTERING MOUSE SENSITIVITY*/
{
int ulx,uly,brx,bry,x,y,oldx,oldy,xcent,ycent;
char *winbuf;
	ulx=219;        /*CO-ORDS OF WINDOW*/
	uly=69;
	brx=ulx+200;
	bry=uly+200;
	ycent=uly+ ((bry-uly) >>1);     /*CALCULATE CENTER OF WINDOW*/
	xcent=ulx+ ((brx-ulx) >>1);
	mhide();
	winbuf=(char *)malloc((unsigned int)_imagesize(ulx,uly,brx,bry));   /*ALLOCATE MEMORY FOR WINDOW BUFFER*/
	if (winbuf==NULL)
	{       /*IF UNSUCCESFUL*/
		puts("Error in allocating window buffer");
		abort();
	}
	_getimage(ulx,uly,brx,bry,winbuf);  /*STORE WHAT IS UNDERNEATH THE WINDOW*/
	_putimage(ulx,uly,winbuf,_GXOR);    /*REMOVE IT FROM SCREEN BY XOR*/
	_rectangle(_GBORDER,ulx+1,uly+1,brx-1,bry-1);       /*Yes its the old problem again, PROBABLE BUG IN MICROSOFT C V1.0*/
	_rectangle(_GBORDER,ulx+1,uly+1,brx-1,bry-1);
	_settextposition((uly/14)+3,(ulx/8)+15);    /*CALC TEXT POSITION IN RELATION TO GRAPHICS CO-ORDS*/
	printf("Low");
	_settextposition((bry/14),(ulx/8)+15);
	printf("High");
	_settextposition((ycent/14),(ulx/8)+2);
	printf("Low");
	_settextposition((ycent/14),(brx/8)-6);
	printf("High");
	_moveto(ulx+5,ycent);   /*DRAWS HORIZONTAL AND VERTICAL LINES*/
	_lineto(brx-5,ycent);
	_moveto(xcent,uly+5);
	_lineto(xcent,bry-5);
	oldx=xcent;             /*SET INITIAL POSITION TO CENTER*/
	oldy=ycent;
	mposet(xcent,ycent);    /*PLACES THE MOUSE AT THE CENTER OF THE WINDOW*/
	mdomain(xcent-90,ycent-90,xcent+90,ycent+90);    /*LIMIT THE MOUSE*/
	mratio(1,1);            /*RESET THE MOUSE RATIO SINCE THE MOUSE IS USED TO ALTER THE RATIO*/
	while(mstatus()!=RIGHTBUT)  /*REPEAT UNTIL THE RIGTH BUT*/
	{
		x=regsout.x.cx;         /*NEW POSITION OF MOUSE*/
		y=regsout.x.dx;
		if(oldx!=x || oldy!=y)  /*IF THE MOUSE HAS MOVED*/
		{
			XORON;
			_ellipse(_GBORDER,oldx-3,oldy-3,oldx+3,oldy+3);     /*REMOVE OLD ELLIPSE*/
			XORON;
			_ellipse(_GBORDER,x-3,y-3,x+3,y+3);                 /*DRAW NEW ELLIPSE*/
			oldx=x;     /*NEW BECOMES OLD*/
			oldy=y;
		}
	mratio(180/(x-219)+1,180/(y-69)+1);     /*WHEN THE RIGHT BUTTON IS PRESSED CHANGE THE RATIO*/
	}
	_putimage(ulx,uly,winbuf,_GPSET);       /*RESTORE WHAT WAS UNDER THE WINDOW*/
	free(winbuf);                           /*FREE THE BUFFER*/
	mdomain(SCRULX,SCRULY,SCRBRX,SCRBRY);
	mshow();
}
