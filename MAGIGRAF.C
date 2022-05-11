/*Magigraph. Main module. Gary Godfrey          10-Oct-88*/

#include<stdio.h>
#include<graph.h>
#include<signal.h>
#include<stdlib.h>

#include"c:\tp\magic-c\magic.h"

extern void showbar(void);
extern int getmenu(void);       /*CONTROL OF MENUDRV*/
extern void filemenu(void);     /*CONTROL OF FILEOPTS*/
extern void drawmenu(void);     /*CONTROL OF DRAWOPTS*/
extern void prefmenu(void);     /*CONTROL OF PREFOPTS*/

/*MENU ARRAY INTIALIZERS*/
char menus[NOFMENUS][MENSTLEN]={"File","Draw","Options"};
char options[NOFMENUS][NOFOPTS][OPTSTLEN]={ {"Save","Load","Dos Shell","Quit"},
											{"Point","Line","Ellipse","Rad Ellipse","Clear","Refresh","Zoom"},
											{"Sensitivity"} };
/*BIT MAP FOR CROSS*/
unsigned int cross[32] =
	{
		0xffff,0xfc7f,0xfc7f,0xfc7f,0xfc7f,0xfc7f,0x8003,0x8103,
		0x8003,0xfc7f,0xfc7f,0xfc7f,0xfc7f,0xfc7f,0xffff,0xffff,
		0x0000,0x0000,0x0100,0x0100,0x0100,0x0100,0x0000,0x3c78,
		0x0000,0x0100,0x0100,0x0100,0x0100,0x0000,0x0000,0x0000
	};
/*BIT MAP FOR WAND, MASK IS CURRENTLY INCORRECT*/
unsigned int wand[32] =
	{
		0xcfef,0xe18f,0xe01f,0xe01f,0xf0fc,0xf0f0,0xc20f,0x864f,
		0xfc67,0xfe73,0xfef9,0xfffc,0xfffe,0xffff,0xffff,0xffff,
		0x0000,0x3010,0x1e70,0x1ba0,0x103c,0x080f,0x3ef0,0x7ab0,
		0x0398,0x018c,0x0106,0x0003,0x0001,0x0000,0x0000,0x0000
	};


int ono;        /*OPTION NUMBER*/
long curobj=0;  /*CURRENT OBJECT*/

/*prototypes*/
void main(void);
void initscreen(void);
void nulfunc(void);
void objlimit(void);

void main()
{
	initscreen();
	while(1)            /*INFINATE LOOP PROGRAM EXITS THROUGH FILEOPTS*/
	{
		if (mbutstat(1)==RIGHTBUT) /*1 for right but*/
		{
			switch (getmenu())      /*CALL TO getmenu RETURNS FROM ' WHEN RIGHT BUTTON HAS BEEN RELEASED*/
			{
				case 0:             /*ENTERS ONE OF THE THREE OTHER CONTROL STRUCTURES IN THE MODULES*/
						filemenu();
						break;
				case 1:
						drawmenu();
						break;
				case 2:
						prefmenu();
						break;
			}
		}
		ono=-1;
	}
}


void initscreen()
{
struct videoconfig config;
	_getvideoconfig(&config);
	if(config.adapter!=_EGA)    /*TEST FOR EGA*/
	{
		puts("Sorry but at the moment this program has only been tested\
				' on an EGA.\nI am not prepared to put my name to a program\
				' that has not been\nfully tested on your graphics card.");
		abort();
	}
	if(config.mode!=_ERESNOCOLOR) _setvideomode(_ERESNOCOLOR); /*SET MODE IF NOT ALREADY IN IT*/
	_rectangle(_GBORDER,SCRULX,SCRULY+15,SCRBRX,SCRBRY);
	minit();
#ifdef M_I86LM      /*CONDITIONAL COMPILATION USED TO DETECT THE MEMORY MODEL THE PROGRAM IS BEING COMPLIED IN*/
	mnewpointer(WANDXHOT,WANDYHOT,wand);    /*THE mnewpointer USES FP_OFF & FP_SEG WHICH ONLY WORK IN A LARGE MEMORY MODEL*/
#endif              /*END OF CONDITIONAL COMPILATION*/
	mshow();
	showbar();
}


void objlimit()     /*This function is called to check the remaining memory in
					the array prints a warning message and possibly gives
					options to delete some of the current file from memory or
					to save the first half of it to disk if needed*/
{
	if (curobj>=MAXIMUMOBJECTS)
	{
		printf("\n\nLook out, the end is nigh!\n\n");
	}
}
