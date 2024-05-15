#define __AVR_ATmega328__
#define F_CPU 15000000
#include <avr/io.h>
#include <util/delay.h>
#include "ili9341.h"
#include "gfx.h"
#include <avr/pgmspace.h>
#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SPI_DC_PIN PB1
#define SPI_SS_PIN PB2
#define SPI_MOSI_PIN PB3
#define SPI_RST_PIN PB4
#define SPI_SCK_PIN PB5

#define W_PIN PC0
#define A_PIN PC1
#define S_PIN PC2
#define D_PIN PC3
#define SPACE_PIN PC4
#define BUTTON_PORT PORTC

void SPI_MasterInit(void) {
    SPI_DDR |= (1 << SPI_SS_PIN) | (1 << SPI_MOSI_PIN) | (1 << SPI_DC_PIN) | (1 << SPI_RST_PIN) | (1 << SPI_SCK_PIN);
    SPI_PORT |= (1 << SPI_SS_PIN);  // Set SS pin high (inactive)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

inline void SPI_MasterTransmit(const uint8_t data) {
    SPDR = data;
//    PORTB ^= 1;
    while (!(SPSR & (1 << SPIF)));
 //   PORTB ^= 1;
}

inline void ILI9341_WriteData8(const uint8_t data){
    SPI_PORT |= (1 << SPI_DC_PIN);
   
    SPI_MasterTransmit(data);
}
inline void ILI9341_WriteData16(const uint16_t data){
    SPI_PORT |= (1 << SPI_DC_PIN);

    SPI_MasterTransmit(data   >> 8);
    SPI_MasterTransmit(data & 0xff);
}
inline void ILI9341_WriteData8_array(const uint8_t* data, const uint8_t n){
    SPI_PORT |= (1 << SPI_DC_PIN);
    
    uint8_t i=0;
    for (i=0;i<n;i++){
        SPI_MasterTransmit(data[i]);
    }
}
inline void ILI9341_WriteData8_array_lpad(const uint8_t* data, const uint8_t n){
    SPI_PORT |= (1 << SPI_DC_PIN);
   
    uint8_t i=0;
    for (i=0;i<n;i++){
        SPI_MasterTransmit(0);
        SPI_MasterTransmit(data[i]);
    }
   
}
inline void ILI9341_WriteData8_array_rpad(const uint8_t* data, const uint8_t n){
    SPI_PORT |= (1 << SPI_DC_PIN);
   
    int i;
    for (i=0;i<n;i++){
        SPI_MasterTransmit(data[i]);
        SPI_MasterTransmit(0);
    }
   
}
inline void ILI9341_WriteData8_array_double(const uint8_t* data, const uint8_t n){
    SPI_PORT |= (1 << SPI_DC_PIN);
    
    int i;
    for (i=0;i<n;i++){
        SPI_MasterTransmit(data[i]);
        SPI_MasterTransmit(data[i]);
    }
   
}


inline void ILI9341_WriteData16_array(const uint16_t* data, const uint8_t n){
    SPI_PORT |= (1 << SPI_DC_PIN);
   
    int i;
    for (i=0;i<n;i++){
        SPI_MasterTransmit(data[i]>>8);
        SPI_MasterTransmit(data[i]&0xff);
    }
   
}




inline void ILI9341_WriteCommand(uint8_t cmd) {
    SPI_PORT &= ~(1 << SPI_DC_PIN); // set transmission type to COMMAND
    SPI_MasterTransmit(cmd);
    
}


void ILI9341_Init(void) {
    static const uint8_t initcmd[] = {
      0xEF, 3, 0x03, 0x80, 0x02,
      0xCF, 3, 0x00, 0xC1, 0x30,
      0xED, 4, 0x64, 0x03, 0x12, 0x81,
      0xE8, 3, 0x85, 0x00, 0x78,
      0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
      0xF7, 1, 0x20,
      0xEA, 2, 0x00, 0x00,
      ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
      ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
      ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
      ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
      ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
      ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
      ILI9341_PIXFMT  , 1, 0x55,
      ILI9341_FRMCTR1 , 2, 0x00, 0x18,
      ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
      0xF2, 1, 0x00,                         // 3Gamma Function Disable
      ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
      ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
      ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
      ILI9341_SLPOUT  , 0x80,                // Exit Sleep
      ILI9341_DISPON  , 0x80,                // Display on
      0x00                                   // End of list
    };
    SPI_PORT &= ~(1 << SPI_SS_PIN);  // Set SS pin low (active)
    _delay_ms(300);
    SPI_PORT |= (1 << SPI_RST_PIN);  // Set SS pin high (inactive)
    _delay_ms(100);


    uint8_t cmd, x, numArgs;
    const uint8_t *addr = initcmd;
  

    while ((cmd = *(addr++)) > 0) {
        x = *(addr++);
        numArgs = x & 0x7F;
        SPI_PORT &= ~(1 << SPI_SS_PIN);  // Set SS pin low (active)
                                         //
        ILI9341_WriteCommand(cmd);
        ILI9341_WriteData8_array(addr,numArgs);
        
        SPI_PORT |= (1 << SPI_SS_PIN);  // Set SS pin high (inactive)
        addr += numArgs;
        if (x & 0x80)
          _delay_ms(150);
    }
}
uint16_t colors[]={
ILI9341_BLACK,
ILI9341_NAVY,
ILI9341_DARKGREEN,
ILI9341_DARKCYAN,
ILI9341_MAROON,
ILI9341_PURPLE,
ILI9341_OLIVE,
ILI9341_LIGHTGREY,
ILI9341_DARKGREY,
ILI9341_BLUE,
ILI9341_GREEN,
ILI9341_CYAN,
ILI9341_RED,
ILI9341_MAGENTA,
ILI9341_YELLOW,
ILI9341_WHITE,
ILI9341_ORANGE,
ILI9341_GREENYELLOW,
ILI9341_PINK,
    };
uint8_t textcol  = 0;

uint16_t palette[16]={
   ILI9341_BLACK, 
   ILI9341_RED, 
   ILI9341_BLUE, 
   ILI9341_BLACK, 
   
   ILI9341_WHITE, 
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   ILI9341_BLACK, 
   ILI9341_BLACK, 
};

uint8_t map[15*20]={
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,

    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
    1,0,1,1,1,1,1,1,1,1,1,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,

    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
    1,0,1,1,1,1,1,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,

    1,0,0,1,1,0,0,1,0,0,0,0,0,0,1,
    1,1,1,1,1,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

};
void ILI9341_drawMap() {
    SPI_PORT &= ~(1 << SPI_SS_PIN);  // Set SS pin low (active)
    uint8_t x,y;
    for (x=0;x<15;x++){
        ILI9341_WriteCommand(0x2A);  // Set column address
        ILI9341_WriteData16(x*16);
        ILI9341_WriteData16(x*16+15);
        
        ILI9341_WriteCommand(0x2B);  // Set row address
        ILI9341_WriteData16(0);
        ILI9341_WriteData16(320);
    for (y=0;y<20;y++){
        
        uint8_t i;
        for (i=0;i<16*2;i++){
            int8_t j;
            for (j=28; j>=0; j=j-4){
                ILI9341_WriteData16(
                        ~palette[ (sprites[i+32*map[x+15*y] ] >> j) & 0xf ] 
                );
            }
        }
    }
    }
    SPI_PORT |= (1 << SPI_SS_PIN);  // Set SS pin low (active)
}
void ILI9341_fillblack() {
    SPI_PORT &= ~(1 << SPI_SS_PIN);  // Set SS pin low (active)
    ILI9341_WriteCommand(0x2A);  // Set column address
    ILI9341_WriteData16(0);
    ILI9341_WriteData16(240);
    
    ILI9341_WriteCommand(0x2B);  // Set row address
    ILI9341_WriteData16(0);
    ILI9341_WriteData16(320);
    uint32_t i;
    SPI_PORT |= (1 << SPI_DC_PIN);
    for (i=0;i<320l*240l*2l;i++){
        SPDR = 0;
        while (!(SPSR & (1 << SPIF)));
    }
    SPI_PORT |= (1 << SPI_SS_PIN);  // Set SS pin low (active)
}
void ILI9341_drawMap2() {
    uint16_t data[64];
    uint16_t *data_end=data;
    SPI_PORT &= ~(1 << SPI_SS_PIN);  // Set SS pin low (active)
    uint16_t x,y;
    ILI9341_WriteCommand(0x2A);  // Set column address
    ILI9341_WriteData16(0);
    ILI9341_WriteData16(240);
    
    ILI9341_WriteCommand(0x2B);  // Set row address
    ILI9341_WriteData16(0);
    ILI9341_WriteData16(320);
    for (y=0;y<320;y++){
    for (x=0;x<15;x++){
        uint32_t spritedata=sprites[  map[x+(y/16)*15] *2*16 + y%16 * 2];
        uint32_t spritedata2=sprites[ map[x+(y/16)*15] *2*16 + y%16 * 2 +1];
          
        
        ILI9341_WriteData16(palette[spritedata>>28  &0xf]);
        ILI9341_WriteData16(palette[spritedata>>24  &0xf]);
        ILI9341_WriteData16(palette[spritedata>>20  &0xf]);
        ILI9341_WriteData16(palette[spritedata>>16  &0xf]);
        ILI9341_WriteData16(palette[spritedata>>12  &0xf]);
        ILI9341_WriteData16(palette[spritedata>> 8  &0xf]);
        ILI9341_WriteData16(palette[spritedata>> 4  &0xf]);
        ILI9341_WriteData16(palette[spritedata>> 0  &0xf]);
        ILI9341_WriteData16(palette[spritedata2>>28 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>>24 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>>20 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>>16 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>>12 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>> 8 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>> 4 &0xf]);
        ILI9341_WriteData16(palette[spritedata2>> 0 &0xf]);
//        SPDR = data;
 //       while (!(SPSR & (1 << SPIF)));
        
    }
    }
    SPI_PORT |= (1 << SPI_SS_PIN);  // Set SS pin low (active)
}

void ILI9341_drawPlayer(uint16_t x, uint16_t y, uint16_t oldx, uint16_t oldy) {
    uint16_t minx = x<oldx?x:oldx;
    uint16_t miny = y<oldy?y:oldy;
    
    uint16_t maxx = x>oldx?x+16:oldx+16;
    uint16_t maxy = y>oldy?y+16:oldy+16;


    ILI9341_WriteCommand(0x2A);  // Set column address
    ILI9341_WriteData16(minx);
    ILI9341_WriteData16(maxx-1);
    
    ILI9341_WriteCommand(0x2B);  // Set row address
    ILI9341_WriteData16(miny);
    ILI9341_WriteData16(maxy-1);
    
    int8_t xi;
    int8_t yi;
    for (yi=0;yi<maxy-miny;yi++){
        for (xi=0;xi<maxx-minx;xi++){

            int8_t spritex =xi-(x-minx);
            int8_t spritey =yi-(y-miny);
            uint8_t col;
            if (spritex <0 || spritex >= 16 || spritey <0 || spritey >= 16)col=2;
            else{
                col = (sprites[spritey*2+(spritex/8)+32*2 ] >> (28-4*(spritex%8)) ) & 0xf ;
                if (!col) col=2;  
            }
            ILI9341_WriteData16(palette[col]);
        }
    }
    
}
void ILI9341_erasePlayer(uint16_t x, uint16_t y) {
    ILI9341_WriteCommand(0x2A);  // Set column address
    ILI9341_WriteData16(x);
    ILI9341_WriteData16(x+15);
    
    ILI9341_WriteCommand(0x2B);  // Set row address
    ILI9341_WriteData16(y);
    ILI9341_WriteData16(y+15);
    
    uint8_t i;
    for (i=1;i;i++){
        SPDR = ILI9341_BLUE>>8;
        while (!(SPSR & (1 << SPIF)));
        SPDR = ILI9341_BLUE&0xff;
        while (!(SPSR & (1 << SPIF)));
    }
    
}
int main(void) {
    SPI_MasterInit();
    ILI9341_Init();
    PORTC=0xFF;
    DDRC=0;
    PORTC=0xFF;
    
    uint8_t arg = 7;
    
    // Draw a pixel at (100, 50) with red color (RGB565)
    uint16_t x= 30;
    uint16_t y= 30;
    ILI9341_drawMap();
    ILI9341_fillblack();
    ILI9341_drawMap2();
    int8_t vel_x;
    int8_t vel_y;
    uint8_t x_tile=x/16;
    uint8_t y_tile=y/16;
    while (1) {
        uint8_t buttonstate = PINC;
        vel_x=0;
        uint8_t onground=map[x/16+(y/16+1)*15] ;
        onground |= map[(x+15)/16+(y/16+1)*15] ;
        if (~buttonstate & (1 << W_PIN) ){
//            vel_y;
        }
        if (~buttonstate & (1 << S_PIN) ){
 //           y++;
        }
        if (~buttonstate & (1 << A_PIN) ){
            vel_x=-1;
        }
        if (~buttonstate & (1 << D_PIN) ){
            vel_x=1;
        }
        if (~buttonstate & (1 << SPACE_PIN)){
            if (map[x/16+(y/16+1)*15]) {
                vel_y=-8;
                onground=0;
            }
        }
        if (map[x/16+1+(y/16)*15] && vel_x == 1 ) vel_x=0;
        if (map[(x-1)/16+(y/16)*15] && vel_x == -1 ) vel_x=0;
        
        if (map[x/16+((y-1)/16)*15] && vel_y < 0 ) vel_y=0;
        if (map[(x+15)/16+((y-1)/16)*15] && vel_y < 0 ) vel_y=0;
        
        if (!onground) vel_y++;
        else {
            vel_y=0;
            y-=y%16;
        }
        if (vel_y>16)vel_y=16;

        uint16_t oldx=x;
        uint16_t oldy=y;
        x+=vel_x;
        y+=vel_y;
        if (map[x/16+(y/16+1)*15]) {
            y-=y%16;
        }
        if (map[x/16+(y/16)*15]) {
            y+=16-(y%16);
        }
        
        if(x!=oldx || y!=oldy) {
            SPI_PORT &= ~(1 << SPI_SS_PIN);  // Set SS pin low (active)
            ILI9341_drawPlayer(x,y,oldx,oldy);
            SPI_PORT |= (1 << SPI_SS_PIN);  // Set SS pin high (inactive)
        }
        _delay_ms(10);

        // Main program loop
    }

    return 0;
}
