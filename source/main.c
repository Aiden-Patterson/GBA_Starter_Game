//--------------------------By:3DSage-------------------------------------------
//               https://www.youtube.com/c/3DSage
//------------------------------------------------------------------------------

//---#defines---
#include "gba.h"
#include "Intellisense.h"
//---Math functions---
#include <math.h> 

//---Load textures---
#include "textures/ball.c"
#include "textures/end.c"
#include "textures/title.c"
#include "textures/right_bumper.c"
#include "textures/left_bumper.c"
#include "textures/TitleScreen.c"

//---Global variables---
#define  GBA_SW 160                                        //actual gba screen width
#define  GBA_SH 128
#define  SW     120                                        //game screen width
#define  SH      80                                        //game screen height
#define RGB(r,g,b) ((r)+((g)<<5)+((b)<<10))                //15 bit, 0-31, 5bit=r, 5bit=g, 5bit=b 
int lastFr=0,FPS=0;                                        //for frames per second
int gameState=0;                                           //game state, title, game, ending
int endScreenTimer=0;                                      //time in second to hold on the ending

typedef struct                                             //player
{
 int x,y;                                                  //position
 const u16* map;                                           //point to current map
}Player;
Player R;													//R is the Right Paddle
Player L;                                       //L is the left paddle

typedef struct                                             //ball
{
 float x,y;                                                  //position
 int frame;                                                //4 frames to animate
 int xdir;                                                  //direction the ball is travelling in the x direction. 1 for right, 0 for left
 int ydir;                                                 //direction the ball is moving in the y direction. 0 for up, 1 for down
 float ychange;                                             //how much the y direction changes. affects the angle
}Ball;
Ball B;														//B is the ball

void clearBackground()
{int x,y;
 	for(x=0;x<SW;x++)
 	{                                                         //rgb values 0-31
  		for(y=0;y<SH;y++)
  			{ VRAM[y*GBA_SW+x]=RGB(5,5,5);}       //clear all 120x80 pixels
 	}
}

void buttons()                                             //buttons to press
{
 if(KEY_RIGHT){ }  
 if(KEY_LEFT ){ }  
 if(KEY_UP )  {L.y-=4; if(L.y<    0){ L.y=    0;}}  //move right paddle up
 if(KEY_DOWN ){L.y+=4; if(L.y>SH-16){ L.y=SH-16;}}  //move right paddle down
 if(KEY_A ){R.y-=4; if(R.y<    0){ R.y=    0;} }    //move left paddle up
 if(KEY_B ){ R.y+=4; if(R.y>SH-16){ R.y=SH-16;}}     //move left paddle down
 if(KEY_L){ } 
 if(KEY_R){ } 
 if(KEY_ST){if(gameState == 1){gameState = 0;}} 
 if(KEY_SL){ } 
}

IN_IWRAM void drawImage(int w,int h, int xo,int yo, const u16* map, int to)
{int x,y,c;          //image w/h, position offset,  texture name, texture offset
 for(x=0;x<w;x++) 
 {                
  	for(y=0;y<h;y++)
  	{
		c=map[(y+to*h)*w+x]; 
	 	if(c>0)
	 		{ VRAM[(y+yo)*GBA_SW+x+xo]=c;}
	} 
 }
}

void gameUpdates()
{
 //ball animate image
 B.frame+=1; 
 if(B.frame>3)
 	{ B.frame=0;}
 //move the ball.
if(B.xdir)
{B.x += 4;}
else{B.x+= -4;}
if(B.ydir)
{B.y += B.ychange;}
else{B.y += -B.ychange;}
 //ball collision detection, reverse the x direction of the ball and update the y direction
if((B.x<L.x+16 && B.x+4 > L.x+12 && B.y<=L.y+15 && B.y+4>=L.y) || (B.x+4>=R.x && B.x+4 < R.x + 4 && B.y+4>=R.y && B.y<=R.y+16))
 	{ 
      playSong(3, 1);
      //reverse x direction
      if(B.xdir == 0)
         {B.xdir = 1;}
      else{B.xdir = 0;}

      //modify y direction
      if(B.ydir == 0) // moving up
      {
         if(B.x < SW << 2) // hit the left paddle moving up
         {
            B.ychange = 4 * ((L.y + 8)/B.y);
         } 
         else{B.ychange = 4 * ((R.y + 8)/B.y);} // hit the right paddle moving up
      }
      else                                    // moving down
      {
         if(B.x < SW << 2) // hit the left paddle moving down
         {
            B.ychange = 4 * (B.y/(L.y+8));
         } 
         else{B.ychange = 4 * (B.y/(R.y+8));} // hit the right paddle moving down
      }
   }
   //If the ball goes behind the paddles, reset to center of the screen
   if(B.x<L.x || B.x + 4 > R.x + 16)
   {
       B.x = SW >> 1;
       B.y = SH >> 1;
   }
   //If the ball hits the top or bottom, invert the y direction
   if(B.y <= 0 || B.y + 4 >= SH)
      {
         if(B.ydir == 0)
         {B.ydir = 1;}
         else{B.ydir = 0;}
      }
}

//---MUSIC----------------------------------------------------------------------
typedef struct
{
 u16* song;
 int tic;
 int spd;
 int size;
 int onOff;
}Music; Music MusicList[5];

u16 notes[] = 
{
   44, 157,  263, 363,  458,  547, 631,  711, 786,  856, 923,  986,//C2,C2#, D2,D2#, E2, F2,F2#, G2,G2#, A2,A2#, B2 
 1046,1102, 1155,1205, 1253, 1297,1340, 1379,1417, 1452,1486, 1517,//C3,C3#, D3,D3#, E3, F3,F3#, G3,G3#, A3,A3#, B3 	
 1547,1575, 1602,1627, 1650, 1673,1694, 1714,1732, 1750,1767, 1783,//C4,C4#, D4,D4#, E4, F4,F4#, G4,G4#, A4,A4#, B4
 1798,1812, 1825,1837, 1849, 1860,1871, 1881,1890, 1899,1907, 1915,//C5,C5#, D5,D5#, E5, F5,F5#, G5,G5#, A5,A5#, B5
 1923,1930, 1936,1943, 1949, 1954,1959, 1964,1969, 1974,1978, 1982,//C6,C6#, D6,D6#, E6, F6,F6#, G6,G6#, A6,A6#, B6
 1985,1989, 1992,1995, 1998, 2001,2004, 2006,2009, 2011,2013 ,2015,//C7,C7#, D7,D7#, E7, F7,F7#, G7,G7#, A7,A7#, B7
};

u16 song_1[]={ 10,0,10, 0, 3,2,3,2, 7,22,0,8, 9,24,0,12};          //title song
u16 song_2[]={ 2,2,0,0, 3,7,5,0, 4,0,4,0, 4,4,6,10};               //game  song
u16 song_3[]={ 60,58,56,54, 52,50,48,46, 44,42,40,38, 36,34,32,30};//end   song
u16 sound[] ={ 14, 15};                                            //sound effect
u16 scale[] = {0, 0, 2, 4, 5, 7, 9, 11, 12, 12, 12};

void playSong(int songNum, int loop)
{
 if(FPS%MusicList[songNum].spd==0 && MusicList[songNum].onOff==1) 
 {
  int note=MusicList[songNum].song[MusicList[songNum].tic];
  if(note>0)
  	{ PlayNote(notes[note],64);}
  MusicList[songNum].tic+=1; 
  if(MusicList[songNum].tic>MusicList[songNum].size)
  	{ 
		MusicList[songNum].tic=0; 
		if(loop==0)
			{ MusicList[songNum].onOff=0;}
	}
 }
}//-----------------------------------------------------------------------------

void init()
{
 L.x=0;        L.y=SH >> 1; L.map=left_bumper_Map;                   //init right bumper
 R.x=SW - 16;  R.y=SH >> 1; R.map=right_bumper_Map;                  //init left bumper
 B.x= SW >> 1; B.y=SH >> 1; B.xdir=0; B.frame=0; B.ychange = 2.0;      //init ball
 endScreenTimer=0;                                                   //clear timer
 //init music
 MusicList[0].song=song_1; MusicList[0].spd=4; MusicList[0].tic=0; MusicList[0].size=15; MusicList[0].onOff=1;
 MusicList[1].song=song_2; MusicList[1].spd=2; MusicList[1].tic=0; MusicList[1].size=15; MusicList[1].onOff=1;
 MusicList[2].song=song_3; MusicList[2].spd=1; MusicList[2].tic=0; MusicList[2].size=15; MusicList[2].onOff=1;
 MusicList[3].song=sound;  MusicList[3].spd=1; MusicList[3].tic=0;  MusicList[3].size=1; MusicList[3].onOff=1;
 MusicList[4].song=scale;  MusicList[4].spd=1; MusicList[4].tic=0; MusicList[4].size=10; MusicList[4].onOff=1;
}

int main()
{int x,y; 
 //Init mode 5------------------------------------------------------------------
 *(u16*)0x4000000 = 0x405;                                           //mode 5 background 2
 *(u16*)0x400010A = 0x82;                                            //enable timer for fps
 *(u16*)0x400010E = 0x84;                                            //cnt timer overflow

 //scale small mode 5 screen to full screen-------------------------------------
 REG_BG2PA=256/2;                                                    //256=normal 128=scale 
 REG_BG2PD=256/2;                                                    //256=normal 128=scale 
  
 init();                                                             //init game variables

 while(1) 
 { 
  if(REG_TM2D>>12!=lastFr)                                           //draw 15 frames a second
  {  
   if(gameState==0)  //title screen--------------------------------------------- 
   {  
    playSong(4,1);                                                  //play title song
    drawImage(120,80, 0,0, TitleScreen_Map, 0);                     //draw title screen
    if(KEY_STATE != 0x03FF && !KEY_ST){ init(); gameState=1;}       //any button but start pressed
   } 

   if(gameState==1)  //play the game-------------------------------------------- 
   {
    clearBackground();                                               //clear the background
    //playSong(1,1);                                                   //play game song
    gameUpdates();                                                   //animate and collision
    buttons();                                                       //Buttons pressed  
    drawImage(16,16, L.x,L.y, L.map, 0);                             //draw left bumper
    drawImage(16,16, R.x,R.y, R.map, 0);                             //draw right bumper
    drawImage( 8, 6, B.x,B.y, ball_Map, B.frame);                     //draw ball                   
   } 

   if(gameState==2)  //end screen-----------------------------------------------
   { 
    playSong(2,0);                                                   //play end song once
    drawImage(120,80, 0,0, end_Map, 0);                              //draw end screen
    endScreenTimer+=1; if(endScreenTimer>15*3){ gameState=0;}  	//hold for 3 seconds
   }  	

   //frames per second---------------------------------------------------------- 
   //VRAM[15]=0; VRAM[FPS]=RGB(31,31,0);                       	 //draw fps 
   FPS+=1; if(lastFr>REG_TM2D>>12){ FPS=0;}                   	//increase frame
   lastFr=REG_TM2D>>12;                                     	//reset counter

   //swap buffers---------------------------------------------------------------
   while(*Scanline<160){}	                                  	//wait all scanlines 
   if  ( DISPCNT&SHOW_BACK)										      //use the back buffer
   	{ 
		DISPCNT &= ~SHOW_BACK; 
		VRAM=(u16*)BACK_BUFFER;
	}      
   else															           //use the front buffer
   { 
	DISPCNT |=  SHOW_BACK; 
	VRAM=(u16*)FRONT_BUFFER;
	}        
  }
 }
}

