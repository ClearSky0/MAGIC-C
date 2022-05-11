/*magic.h*/

/*THESE DECLARATIONS SPECIFY THE SIZE AND CONTENTS OF THE MENUS*/

#define NOFMENUS 3
#define NOFOPTS 10
#define MENSTLEN 8
#define OPTSTLEN 12

struct titlepos {                               /*holds the graphic coords*/
					int start,end;              /*of the start and begining of*/
				} menuname[NOFMENUS];           /*menu titles*/

struct optbox {                                 /*holds ul and br coords of opt*/
			  int x,y;                          /*boxes*/
			  } optul[NOFOPTS],optbr[NOFOPTS];


#define ULBARX 0
#define ULBARY 0
#define BRBARX 639
#define BRBARY 13                  /*coords of whole menu bar*/
#define PEND "Pen=Down"
#define PENU "Pen=Up  "
#define NOPEN -1
#define PENOFF 0
#define PENON 1
#define SCRULX 0
#define SCRULY 0
#define SCRBRX 639
#define SCRBRY 349
#define SCRCX 320
#define SCRCY 175
#define LEFTBUT  1
#define RIGHTBUT 2
#define BOTHBUT  3
#define WANDXHOT 2
#define WANDYHOT 1
#define CROSSXHOT 7
#define CROSSYHOT 7
#define MAXIMUMOBJECTS 1500
#define POINT 0
#define LINE 1
#define ELLIPSE 2
#define XORON outp(0x3ce,3); outp(0x3cf,24)   /*SETS THE XOR FUNCTION OF THE GRAPHICS CHIP*/
#define DONS 0
#define MINE 1


/*DEFINES FOR LINEF*/
#define TRUE    -1
#define FALSE   0
#define abs(x)   (((x)<0) ? -(x) : (x))
#define sign(x) ((x)>0 ? 1 :((x)==0 ? 0 :(-1)))
#define ENABLE  0x0F
#define INDEXREG    0x3CE
#define VALREG  0x3CF
#define OUTINDEX(index,val) {outp(INDEXREG,index);outp(VALREG,val);}
#define EGABASE 0xA0000000L
#define WIDTH   80L
#define XMAX    639
#define YMAX    349
#define XMIN    0
#define YMIN    0
#define XORIT 0X80
#define ORIT  0x00


/*declarations for use with the mouse.*/

extern void minit(void);
extern void mshow(void);
extern void mhide(void);
extern void mposet(unsigned int,unsigned int);
extern void mdomain(unsigned int,unsigned int,unsigned int,unsigned int);
extern void mnewpointer(char,char,int far *);
extern void mratio(unsigned char,unsigned char);
extern unsigned int mbutstat(char);
extern unsigned int mrelease(char);
extern unsigned int mstatus(void);


/*structure for graphics objects*/

struct PICTUREOBJECT
{
	int objectype;
	int brightstate;
	union
	{
		struct
		{
			double x;
			double y;
		} pixel;
		struct
		{
			double x1;
			double y1;
			double x2;
			double y2;
		} line;
		struct
		{
			double x1;
			double y1;
			double x2;
			double y2;
		} ellipse;
	} oftype;
} picobject[MAXIMUMOBJECTS];
