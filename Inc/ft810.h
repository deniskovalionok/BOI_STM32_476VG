
#ifndef FT810_h
#define FT810_h
   
/*****************************************************************************/
/************************* I N C L U D E S ***********************************/
/*****************************************************************************/

/*****************************************************************************/
/********************** D E C L A R A T I O N S ******************************/
/*****************************************************************************/


// FT800 Chip Commands - use with cmdWrite
#define FT800_ACTIVE					0x00			// Initializes FT800
#define FT800_STANDBY					0x41			// Place FT800 in Standby (clk running)
#define FT800_SLEEP						0x42			// Place FT800 in Sleep (clk off)
#define FT800_PWRDOWN					0x50			// Place FT800 in Power Down (core off)
#define FT800_CLKEXT					0x44			// Select external clock source
#define FT800_CLK48M					0x62			// Select 48MHz PLL
#define FT800_CLK36M					0x61			// Select 36MHz PLL
#define FT800_CORERST					0x68			// Reset core - all registers default

// FT800 Memory Commands - use with ft800memWritexx and ft800memReadxx
#define MEM_WRITE							0x80			// FT800 Host Memory Write 
#define MEM_READ							0x00			// FT800 Host Memory Read

#endif  //FT810_h