/*MOUSEDRV.C
For more detail see PROGRAM NOW Jan/Feb 1988 p58
refined by GAG 1-9-88*/

#include <dos.h>
#define MOUSE 0x33	/*mouse interrupt number*/

union REGS regsin,regsout;
struct SREGS sregs;

void minit(void)    /*FUNC 0, INITIALIZE. RETURNS NOTHING*/
{
	regsin.x.ax=0;
	int86(MOUSE,&regsin,&regsout);
}


void mshow(void)    /*FUNC 1, SHOW POINTER. RETURNS NOTHING*/
{
	regsin.x.ax=1;
	int86(MOUSE,&regsin,&regsout);
}


void mhide(void)    /*FUNC 2, HIDE POINTER.
					OUTPUT NOTHING. mshow AND mhide ARE BOTH ACCUMULATIVE*/
{
	regsin.x.ax=2;
	int86(MOUSE,&regsin,&regsout);
}


unsigned int mstatus(void)  /*FUNC 3, GET MOUSE CO-ORD AND BUTTON STATUS.
							OUTPUT regsout.x.bx=BUTTON STATUS(BIT 0=LEFT, BIT 1=RIGHT)
											  cx=POINTER X POS
											  dx=POINTER Y POS*/
{
	regsin.x.ax=3;
	int86(MOUSE,&regsin,&regsout);
	return(regsout.x.bx);
}


void mposet(unsigned int x,unsigned int y)  /*FUNC 4, SET THE POSITION OF MOUSE POINTER.
											INPUT regsin.x.cx=NEW X POS
														   dx=NEW Y POS
											OUTPUT NOTHING.*/
{
	regsin.x.ax=4;
	regsin.x.cx=x;
	regsin.x.dx=y;
	int86(MOUSE,&regsin,&regsout);
}


unsigned int mbutstat(char butno)   /*FUNC 5, GET STATUS OF PARTICULAR BUTTON
									INPUT regsin.x.bx=BUTTON OF INTEREST(0=LEFT,1=RIGHT)
									OUTPUT regsout.x.ax=BUTTON STATUS
													 bx=NUMBER OF PRESSES
													 cx=X POS AT LAST PRESS
													 dx=Y POS AT LAST PRESS*/
{
	regsin.x.ax=5;
	regsin.x.bx=butno;
	int86(MOUSE,&regsin,&regsout);
	return(regsout.x.ax);
}


unsigned int mrelease(char butno)   /*FUNC 6, GET RELEASE STATUS OF PARTICULAR BUTTON
									INPUT regsin.x.bx=BUTTON OF INTEREST(0=LEFT,1=RIGHT)
									OUTPUT regsout.x.ax=BUTTON STATUS
													 bx=NUMBER OF RELEASES?
													 cx=X POS AT LAST RELEASE
													 dx=Y POS AT LAST RELEASE*/
{
	regsin.x.ax=6;
	regsin.x.bx=butno;
	int86(MOUSE,&regsin,&regsout);
	return(regsout.x.ax);
}


void mdomain(unsigned int minx, unsigned int miny,unsigned int maxx, unsigned int maxy)
													/*FUNC 7, SET THE DOMAIN OF THE MOUSE
													1st INPUT regsin.x.cx=MINIMUM X POS
																	   dx=MAXIMUM X POS
													2nd INPUT regsin.x.cx=MINIMUM Y POS
																	   dx=MAXIMUM Y POS
													OUTPUT NOTHING*/
{
	regsin.x.ax=7;
	regsin.x.cx=minx;
	regsin.x.dx=maxx;
	int86(MOUSE,&regsin,&regsout);
	regsin.x.ax=8;
	regsin.x.cx=miny;
	regsin.x.dx=maxy;
	int86(MOUSE,&regsin,&regsout);
}


void mnewpointer(char xhot,char yhot,int far *ptr)  /*FUNC 9, REDEFINE GRAPHICS CURSOR
													NB. TO USE THIS FUNC COMPILE WITH A LARGE MEMORY MODEL
													INPUT regsin.x.bx=MOUSE X HOT SPOT
																   cx=MOUSE Y HOT SPOT
																   dx=OFFSET TO CURSOR GRAPHIC ARRAY
																   es=SEGMENT POINTER TO CURSOR GRAPHIC ARRAY
													OUTPUT NOTHING*/

{
	regsin.x.ax=9;
	regsin.x.bx=xhot;
	regsin.x.cx=yhot;
	regsin.x.dx=FP_OFF(ptr);
	sregs.es=FP_SEG(ptr);
	int86(MOUSE,&regsin,&regsout);
}


void mratio(unsigned char xrat,unsigned char yrat)  /*FUNC 15, SET MOVEMENT RATIOS OF MOUSE
													INPUT regsin.x.cx=X MICKEYS/PIXEL
																   dx=Y MICKEYS/PIXEL
													OUTPUT NOTHING*/
{
	regsin.x.ax=15;
	regsin.x.cx=xrat;
	regsin.x.dx=yrat;
	int86(MOUSE,&regsin,&regsout);
}
