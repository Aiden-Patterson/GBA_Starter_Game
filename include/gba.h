

//---rename unsigned short and long---
typedef unsigned short 	u16;
typedef unsigned long  	u32; 
typedef unsigned int 	s32;
typedef unsigned char	u8;



//---Video buffer---
u16    *VRAM; 
#define FRONT_BUFFER   	(u16*)0x6000000 
#define BACK_BUFFER    	(u16*)0x600A000
#define DISPCNT        	*(u32*)0x4000000
#define SHOW_BACK   	0x10 
volatile u16* Scanline=(volatile u16*)0x4000006;
//---Scale mode 5 screen---
#define REG_BG2PA *(volatile unsigned short *)0x4000020
#define REG_BG2PD *(volatile unsigned short *)0x4000026
//---Buttons---
#define KEY_STATE      (*(volatile u16*)0x4000130) 
#define KEY_A          !(KEY_STATE &   1)
#define KEY_B          !(KEY_STATE &   2)
#define KEY_SL         !(KEY_STATE &   4)
#define KEY_ST         !(KEY_STATE &   8)
#define KEY_RIGHT      !(KEY_STATE &  16)
#define KEY_LEFT       !(KEY_STATE &  32)
#define KEY_UP         !(KEY_STATE &  64)
#define KEY_DOWN       !(KEY_STATE & 128)
#define KEY_R          !(KEY_STATE & 256)
#define KEY_L          !(KEY_STATE & 512)
//---Timer for FPS---
#define REG_TM2D       *(volatile u16*)0x4000108
//--Fast Iwram and Ewram
#define IN_IWRAM       __attribute__ ((section (".iwram")))
#define IN_EWRAM       __attribute__ ((section (".ewram")))

#define REG_SOUNDCNT_X  *(volatile u16*)0x4000084 //Extended sound control
#define REG_SOUNDCNT_L  *(volatile u16*)0x4000080 //DMG sound control
#define REG_SOUNDCNT_H  *(volatile u16*)0x4000082 //Direct sound control
#define REG_SOUND3CNT_L *(volatile u16*)0x4000070
#define SOUND3BANK32    0x0000	                  //Use two banks of 32 steps each
#define SOUND3SETBANK1  0x0040
#define REG_WAVE_RAM0   *(volatile u32*)0x4000090 
#define REG_WAVE_RAM1   *(volatile u32*)0x4000094 
#define REG_WAVE_RAM2   *(volatile u32*)0x4000098 
#define REG_WAVE_RAM3   *(volatile u32*)0x400009C 
#define SOUND3SETBANK0  0x0000	                  //Bank to play 0 or 1 (non set bank is written to)
#define SOUND3PLAY	    0x0080	                  //Output sound
#define REG_SOUND3CNT_H *(volatile u16*)0x4000072 
#define SOUND3OUTPUT1   0x2000	                  //Output unmodified
#define REG_SOUND3CNT_X *(volatile u16*)0x4000074 
#define SOUND3INIT	    0x8000	                  //Makes the sound restart
#define SOUND3PLAYONCE  0x4000	                  //Play sound once

void PlayNote( u16 frequency, unsigned char length ){

	unsigned char adjustedLength = 0xFF - length;

	REG_SOUNDCNT_X = 0x80;
	REG_SOUNDCNT_L=0xFF77;

	REG_SOUNDCNT_H = 2;

	REG_SOUND3CNT_L = SOUND3BANK32 | SOUND3SETBANK1;
	REG_WAVE_RAM0=0x10325476;
	REG_WAVE_RAM1=0x98badcfe;
	REG_WAVE_RAM2=0x10325476;
	REG_WAVE_RAM3=0x98badcfe;
	REG_SOUND3CNT_L = SOUND3BANK32 | SOUND3SETBANK0;

	REG_SOUND3CNT_L |= SOUND3PLAY;

	REG_SOUND3CNT_H = SOUND3OUTPUT1 | adjustedLength;
	REG_SOUND3CNT_X=SOUND3INIT|SOUND3PLAYONCE| frequency ;
}
