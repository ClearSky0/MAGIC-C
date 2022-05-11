/*drawopts.c*/

#include<dos.h>
#include<graph.h>
#include<malloc.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#include"c:\tp\magic-c\magic.h"

extern ono;
extern curobj;
extern cross;
extern wand;

extern void objlimit(void);

union REGS regsout;

void drawmenu(void);
void dots(void);
void savemenubar(void);
void printonbar(char *,char);
void replacemenubar(void);
void lines(void);
void limitmouse(void);
void unlimitmouse(void);
void ellipses(char);
void clear(void);
void refresh(void);
void zoom(void);
void linef(int,int,int,int,int,unsigned int,int,int);
void points(int,int,int);

char *barbuf;
double scale=1.0;

void drawmenu()         /*CONTROL STRUCTURE OF DRAWOPTS*/
{
	switch(ono)
	{
		case 0:
				limitmouse();
				dots();
				unlimitmouse();
				break;
		case 1:
				limitmouse();
				lines();
				unlimitmouse();
				break;
		case 2:
				limitmouse();
				ellipses(MINE);
				unlimitmouse();
				break;
		case 3:
				limitmouse();
				ellipses(DONS);
				unlimitmouse();
				break;
		case 4:
				clear();
				break;
		case 5:
				refresh();
				break;
		case 6:
				zoom();
				break;
	}
}

void limitmouse()   /*REDUCES THE ACTIVE AREA OF THE MOUSE*/
{
	mdomain(SCRULX+2,SCRULY+17,SCRBRX-2,SCRBRY-2);
}

void unlimitmouse() /*RESTORES ACTIVE AREA OF MOUSE TO WHOLE SCREEN*/
{
	mdomain(SCRULX,SCRULY,SCRBRX,SCRBRY);
}

void savemenubar()
{
	barbuf=(char *)malloc((unsigned int)_imagesize(ULBARX,ULBARY,BRBARX,BRBARY));
	if (barbuf==(char *)NULL)
	{
		puts("\nThere has been an error in the menu buffer");
		abort();
	}
	mhide();
	_getimage(ULBARX,ULBARY,BRBARX,BRBARY,barbuf);
	_putimage(ULBARX,ULBARY,barbuf,_GXOR);
	mshow();
}

void replacemenubar()
{
	mhide();
	_putimage(ULBARX,ULBARY,barbuf,_GPSET);
	mshow();
	free(barbuf);
}

void printonbar(char *str,char penst)
{
	_settextposition(0,4);
	mhide();
	printf("%s",str);                       /*PRINT REQUESTED STRING*/
	if(penst!=NOPEN)
	{
		_settextposition(1,25);
		printf("%s",(penst ? PEND : PENU));     /*PEN STATE*/
		_settextposition(1,65);
		printf("Next=%d",curobj+1);             /*NEXT OBJECT*/
	}
	_rectangle(_GBORDER,ULBARX,ULBARY,BRBARX,BRBARY);
	mshow();
}

/*linef.c
	SECTION OF CODE TAKEN FROM ADVANCED GRAPHICS IN C, JOHNSON*/

void linef(x1,y1,x2,y2,color,style,orxor,first_on)
int x1,y1,x2,y2,color,orxor,first_on;
unsigned style;
{
int ix,iy,i,inc,x,y,dx,dy,plot,plotx,ploty;
unsigned style_mask;
style=(style==0) ? 0xFFFF : style;
style_mask=style;
OUTINDEX(1,ENABLE);
if (orxor==XORIT) OUTINDEX(3,0x18);
dx = x2 - x1;
dy =y2 -y1;
ix=abs(dx);
iy=abs(dy);
inc = max(ix,iy);
plotx=x1;
ploty=y1;
x=y=0;
if (first_on) points(plotx,ploty,color);
for (i=0;i<=inc;++i)
	{
	if (style_mask==0) style_mask=style;
	x += ix;
	y += iy;
	plot=FALSE;
	if (x>inc)
		{
		plot=TRUE;
		x -= inc;
		plotx += sign(dx);
		}
	if (y>inc)
		{
		plot=TRUE;
		y -= inc;
		ploty += sign(dy);
		}
	if (plot && style_mask & 0x0001) points(plotx,ploty,color);
	style_mask >>= 1;
	}
OUTINDEX(0,0);
OUTINDEX(1,0);
if (orxor==XORIT) OUTINDEX(3,0);
OUTINDEX(8,TRUE);
}

void points(x,y,color)
int x,y,color;
{
#ifdef M_I86LM
	unsigned char mask=0x80,exist_color;
	char far *base;
	if (x<XMIN || x>XMAX || y<YMIN || y>YMAX) return;
	base=(char far *)(EGABASE+((long)y * WIDTH+((long)x / 8L)));
	mask >>=x % 8;
	exist_color= *base;
	OUTINDEX(0,color);
	OUTINDEX(8,mask);
	*base &= TRUE;
#endif
}

void dots()         /*PLOTS A SINGLE POINT EVERY TIME THE LEFT BUTTON IS PRESSED
						UNTIL THE RIGHT BUTTON IS PRESSED*/
{
int x,y;
	savemenubar();
	printonbar("Point Active",PENOFF);
#ifdef M_I86LM
	mnewpointer(CROSSXHOT,CROSSYHOT,&cross);
#endif
	while(mbutstat(1)!=RIGHTBUT)
	{
		if(mbutstat(0)==LEFTBUT)
		{
			picobject[curobj].objectype=POINT;
			/*SET THE STORED CO-ORDS TO +- THE CENTER OF THE SCREEN*/
			picobject[curobj].oftype.pixel.x=((x=regsout.x.cx)-SCRCX)/scale;
			picobject[curobj].oftype.pixel.y=((y=regsout.x.dx)-SCRCY)/scale;
			printonbar("Point Active",PENON);
			mhide();
			_setpixel(x,y);   /*SHOW THE PIXEL*/
			mshow();
			curobj++;               /*INCREASE CURRENT OBJECT BY ONE*/
			printonbar("Point Active",PENOFF);
			while(mrelease(0));     /*WAIT UNTIL LEFTBUT IS RELEASED THEN CONTINUE*/
			objlimit();             /*CHECK TO SEE IF THE LIMIT HAS BEEN REACHED*/
		}
	}
	replacemenubar();
#ifdef M_I86LM
	mnewpointer(WANDXHOT,WANDYHOT,&wand);
#endif
}

void lines()    /*DRAWS LINES FROM WHERE LEFTBUT WAS PRESSED TO WHERE IT WAS
					RELEASED, INTERACTIVELY UNTIL RIGHTBUT*/
{
int x,y,oldx=0,oldy=0,newx=0,newy=0;
	savemenubar();
	printonbar("Line Active",PENOFF);
#ifdef M_I86LM
	mnewpointer(CROSSXHOT,CROSSYHOT,&cross);
#endif
	while(mbutstat(1)!=RIGHTBUT)
	{
		if(mbutstat(0)==LEFTBUT)
		{
			x=newx=oldx=regsout.x.cx;   /*SET ALL TO CURRENT MOUSE POSITION*/
			y=newy=oldy=regsout.x.dx;
			_moveto(newx,newy);
			mhide();
			printonbar("Line Active",PENON);
			while(mstatus()==LEFTBUT)
			{
				newx=regsout.x.cx;      /*GET CURRENT MOUSE POSITION*/
				newy=regsout.x.dx;
				if(newx!=oldx || newy!=oldy)    /*IF THE MOUSE HAS MOVED*/
				{
					if(y==oldy)     /*IF THE LINE IS HORIZONTAL USE LINEF INSTEAD OF INBUILT C FUNCTION
										DUE TO SUSPECTED BUG IN MICROSOFT C V1.0*/
					{
						linef(x,y,oldx,oldy,7,255,0x80,-1); /*LINEF HAS A BUILT IN XOR FUNCTION*/
					}
					else            /*USE NORMAL LINE*/
					{
						XORON;      /*MACRO USED TO OUTPUT DIRECTLY TO THE VIDEO CHIP, TO SET XOR ON*/
						_moveto(x,y);
						_lineto(oldx,oldy); /*REMOVE OLD LINE*/
					}
					if(y==newy)
					{
						linef(x,y,newx,newy,7,255,0x80,-1); /*IF LINE IS HORIZONTAL*/
					}
					if(y!=newy)
					{
						XORON;              /*IF LINE IS NOT HORIZONTAL*/
						_moveto(x,y);
						_lineto(newx,newy); /*DRAW NEW LINE*/
					}
					oldx=newx;
					oldy=newy;
				}
			}
			printonbar("Line Active",PENOFF);
			_moveto(x,y);
			_lineto(newx,newy); /*DRAW LINE IN RESTING POSITION WITHOUT XOR*/
			picobject[curobj].objectype=LINE;
			picobject[curobj].oftype.line.x1=(x-SCRCX)/scale;    /*STORES CO-ORDS IN RELATION TO THE CENTER OF THE SCREEN*/
			picobject[curobj].oftype.line.y1=(y-SCRCY)/scale;
			picobject[curobj].oftype.line.x2=(newx-SCRCX)/scale;
			picobject[curobj].oftype.line.y2=(newy-SCRCY)/scale;
			curobj++;
			mshow();
			objlimit();
		}
	}
	replacemenubar();
#ifdef M_I86LM
	mnewpointer(WANDXHOT,WANDYHOT,&wand);
#endif
}

void ellipses(char type)     /*DRAWS AN ELLIPSES INTERACTIVELY UNTIL RIGHT BUT IS PRESSED*/
{
int x,y,newx,oldx,newy,oldy,fox,foy,sox,soy,fnx,fny,snx,sny;
#ifdef M_I86LM
	mnewpointer(CROSSXHOT,CROSSYHOT,&cross);    /*CHANGE POINTER FROM WAND TO CROSS*/
#endif
	savemenubar();
	printonbar("Ellipse Active",PENOFF);
	_setcliprgn(SCRULX+2,SCRULY+17,SCRBRX-2,SCRBRY-2);
	while(mbutstat(1)!=RIGHTBUT)
	{
		if(mbutstat(0)==LEFTBUT)
		{
			x=oldx=regsout.x.cx;
			y=oldy=regsout.x.dx;
			if(type==DONS)
			{
				fox=x-oldx;
				foy=y-oldy;
			}
			mhide();
			_setcliprgn(SCRULX,SCRULY,SCRBRX,SCRBRY);
			printonbar("Ellipse Active",PENON);
			_setcliprgn(SCRULX+2,SCRULY+17,SCRBRX-2,SCRBRY-2);
			while(mstatus()==LEFTBUT)
			{
				newx=regsout.x.cx;
				newy=regsout.x.dx;
				if(type==DONS)
				{
					snx=x+newx;
					sny=y+newy;
					sox=snx;
					soy=sny;
				}
				if(oldx!=newx || oldy!=newy)    /*IF MOUSE HAS MOVED. REMOVE OLD ELLIPSE AND PLACE A NEW ONE*/
				{
					if(type==DONS)
					{
						fox=x-oldx;
						foy=y-oldy;
						fnx=x-newx;
						fny=y-newy;
					}
					else
					{
						fox=fnx=x;
						foy=fny=y;
						sox=oldx;
						soy=oldy;
						snx=newx;
						sny=newy;
					}
					XORON;
					_ellipse(_GBORDER,fox,foy,sox,soy);
					XORON;
					_ellipse(_GBORDER,fnx,fny,snx,sny);
				}
				oldx=newx;
				oldy=newy;
			}
		_setcliprgn(SCRULX,SCRULY,SCRBRX,SCRBRY);
		printonbar("Ellipse Active",PENOFF);
		_setcliprgn(SCRULX+2,SCRULY+17,SCRBRX-2,SCRBRY-2);
		_ellipse(_GBORDER,x,y,newx,newy);   /*DRAW ELLIPSE IN FINAL POSITION WITHOUT XOR*/
		mshow();
		picobject[curobj].objectype=ELLIPSE;
		picobject[curobj].oftype.ellipse.x1=(fox-SCRCX)/scale;
		picobject[curobj].oftype.ellipse.y1=(foy-SCRCY)/scale;
		picobject[curobj].oftype.ellipse.x2=(snx-SCRCX)/scale;
		picobject[curobj].oftype.ellipse.y2=(sny-SCRCY)/scale;
		curobj++;
		objlimit();
		}
	}
	_setcliprgn(SCRULX,SCRULY,SCRBRX,SCRBRY);
	replacemenubar();
#ifdef M_I86LM
	mnewpointer(WANDXHOT,WANDYHOT,&wand);   /*CHANGE POINTER BACK TO A WAND*/
#endif
}

void clear()    /*CLEARS THE SCREEN WITHOUT REMOVING THE OBJECTS FROM MEMORY*/
{
	curobj=-1;
	refresh();
	curobj=0;
}

void refresh()  /*REDRAWS ALL THAT HAS BEEN DRAWN UPTO THE CURRENT OBJECT*/
{
long lastobj;
	lastobj=curobj-1;
	mhide();
	_setcliprgn(SCRULX+2,SCRULY+17,SCRBRX-2,SCRBRY-2);
	_clearscreen(_GVIEWPORT);
	_setlogorg(SCRCX,SCRCY);
	for(curobj=0;curobj<=lastobj;curobj++)
	{
		switch(picobject[curobj].objectype)
		{
			case POINT:
					_setpixel(picobject[curobj].oftype.pixel.x*scale,picobject[curobj].oftype.pixel.y*scale);
					break;
			case LINE:
					_moveto(picobject[curobj].oftype.line.x1*scale,picobject[curobj].oftype.line.y1*scale);
					_lineto(picobject[curobj].oftype.line.x2*scale,picobject[curobj].oftype.line.y2*scale);
					break;
			case ELLIPSE:
					_ellipse(_GBORDER,picobject[curobj].oftype.ellipse.x1*scale,picobject[curobj].oftype.ellipse.y1*scale\
									 ,picobject[curobj].oftype.ellipse.x2*scale,picobject[curobj].oftype.ellipse.y2*scale);
					break;
		}
	}
	_setcliprgn(SCRULX,SCRULY,SCRBRX,SCRBRY);
	_setlogorg(SCRULX,SCRULY);
	mshow();
}

void zoom()
{
int len;
char *str;
double stepsize=0.01;
int acc=1;
	savemenubar();
	gcvt(scale,8,str);
	strcat(str,"=Zoom Scale    ");
	printonbar(str,NOPEN);
	do
	{
		while(mstatus()==LEFTBUT && scale-stepsize>0.01)
		{
			scale-=stepsize;
			acc++;
			gcvt(scale,8,str);
			strcat(str,"=Zoom Scale    ");
			printonbar(str,NOPEN);
			if(acc>15)
			{
				acc=1;
				stepsize=stepsize*2;
			}
		}
		while(mstatus()==RIGHTBUT && scale+stepsize<100)
		{
			scale+=stepsize;
			acc++;
			gcvt(scale,8,str);
			strcat(str,"=Zoom Scale    ");
			printonbar(str,NOPEN);
			if(acc>15)
			{
				acc=1;
				stepsize=stepsize*2;
			}
		}
		acc=1;
		stepsize=0.01;
		if(mstatus()==BOTHBUT) refresh();
		while(!mstatus());
	} while(regsout.x.dx>BRBARY);
	replacemenubar();
}
