/**
  * @file    ft800.h
  * @brief   Библиотека графических контроллеров FT80x/FT81x
  *          Этот файл содержит макроопределения и объявления
  *          функций
  */
#ifndef _FT800_H_
#define _FT800_H_

#include <stdint.h>

#define MAX_HEIGHT  SCREEN_HEIGHT
#define MAX_WIDTH   SCREEN_WIDTH


/* FT800 Power Modes */
#define CMD_ACTIVE  0x00
#define CMD_STANDBY 0x41
#define CMD_SLEEP   0x42
#define CMD_PWRDOWN 0x50

/* FT800 Clock Switching */
#define CMD_CLKINT 0x48
#define CMD_CLKEXT 0x44
#define CMD_CLK48M 0x62
#define CMD_CLK36M 0x61

/* MISC */
#define CMD_CORERST 0x68


//#define FT800_VERSION "1.9.0"
#define ADC_DIFFERENTIAL     1UL
//#define ADC_SINGLE_ENDED     0UL
#define ADPCM_SAMPLES        2UL
#define ALWAYS               7UL
#define ARGB1555             0UL
#define ARGB2                5UL
#define ARGB4                6UL
#define BARGRAPH             11UL
#define BILINEAR             1UL
#define BITMAPS              1UL
#define BORDER               0UL

#define CMDBUF_SIZE          4096UL
#define CMD_APPEND           4294967070UL
#define CMD_BGCOLOR          4294967049UL
#define CMD_BITMAP_TRANSFORM 4294967073UL
#define CMD_BUTTON           4294967053UL
#define CMD_CALIBRATE        4294967061UL
#define CMD_CLOCK            4294967060UL
#define CMD_COLDSTART        4294967090UL
#define CMD_CRC              4294967043UL
#define CMD_DIAL             4294967085UL
#define CMD_DLSTART          4294967040UL
#define CMD_EXECUTE          4294967047UL
#define CMD_FGCOLOR          4294967050UL
#define CMD_GAUGE            4294967059UL
#define CMD_GETMATRIX        4294967091UL
#define CMD_GETPOINT         4294967048UL
#define CMD_GETPROPS         4294967077UL
#define CMD_GETPTR           4294967075UL
#define CMD_GRADCOLOR        4294967092UL
#define CMD_GRADIENT         4294967051UL
#define CMD_HAMMERAUX        4294967044UL
#define CMD_IDCT             4294967046UL
#define CMD_INFLATE          4294967074UL
#define CMD_INTERRUPT        4294967042UL
#define CMD_KEYS             4294967054UL
#define CMD_LOADIDENTITY     4294967078UL
#define CMD_LOADIMAGE        4294967076UL
#define CMD_LOGO             4294967089UL
#define CMD_MARCH            4294967045UL
#define CMD_MEMCPY           4294967069UL
#define CMD_MEMCRC           4294967064UL
#define CMD_MEMSET           4294967067UL
#define CMD_MEMWRITE         4294967066UL
#define CMD_MEMZERO          4294967068UL
#define CMD_NUMBER           4294967086UL
#define CMD_PROGRESS         4294967055UL
#define CMD_REGREAD          4294967065UL
#define CMD_ROMFONT          0xFFFFFF3F
#define CMD_ROTATE           4294967081UL
#define CMD_SCALE            4294967080UL
#define CMD_SCREENSAVER      4294967087UL
#define CMD_SCROLLBAR        4294967057UL
#define CMD_SETBASE          0xFFFFFF38
#define CMD_SETFONT          4294967083UL
#define CMD_SETMATRIX        4294967082UL
#define CMD_SKETCH           4294967088UL
#define CMD_SLIDER           4294967056UL
#define CMD_SNAPSHOT         4294967071UL
#define CMD_SPINNER          4294967062UL
#define CMD_STOP             4294967063UL
#define CMD_SWAP             4294967041UL
#define CMD_TEXT             4294967052UL
#define CMD_TOGGLE           4294967058UL
#define CMD_TOUCH_TRANSFORM  4294967072UL
#define CMD_TRACK            4294967084UL
#define CMD_TRANSLATE        4294967079UL

#define DECR                 4UL
#define DECR_WRAP            7UL
#define DLSWAP_DONE          0UL
#define DLSWAP_FRAME         2UL
#define DLSWAP_LINE          1UL
#define DST_ALPHA            3UL
#define EDGE_STRIP_A         7UL
#define EDGE_STRIP_B         8UL
#define EDGE_STRIP_L         6UL
#define EDGE_STRIP_R         5UL
#define EQUAL                5UL
#define GEQUAL               4UL
#define GREATER              3UL
#define INCR                 3UL
#define INCR_WRAP            6UL
#define INT_CMDEMPTY         32UL
#define INT_CMDFLAG          64UL
#define INT_CONVCOMPLETE     128UL
#define INT_PLAYBACK         16UL
#define INT_SOUND            8UL
#define INT_SWAP             1UL
#define INT_TAG              4UL
#define INT_TOUCH            2UL
#define INVERT               5UL

#define KEEP                 1UL
#define L1                   1UL
#define L4                   2UL
#define L8                   3UL
#define LEQUAL               2UL
#define LESS                 1UL
#define LINEAR_SAMPLES       0UL
#define POINTS               2UL
#define LINES                3UL
#define LINE_STRIP           4UL
#define NEAREST              0UL
#define NEVER                0UL
#define NOTEQUAL             6UL
#define ONE                  1UL
#define ONE_MINUS_DST_ALPHA  5UL
#define ONE_MINUS_SRC_ALPHA  4UL
#define OPT_CENTER           1536UL
#define OPT_CENTERX          512UL
#define OPT_CENTERY          1024UL
#define OPT_FLAT             256UL
#define OPT_MONO             1UL
#define OPT_NOBACK           4096UL
#define OPT_NODL             2UL
#define OPT_NOHANDS          49152UL
#define OPT_NOHM             16384UL
#define OPT_NOPOINTER        16384UL
#define OPT_NOSECS           32768UL
#define OPT_NOTICKS          8192UL
#define OPT_RIGHTX           2048UL
#define OPT_SIGNED           256UL
#define PALETTED             8UL
#define FTPOINTS             2UL
#define RECTS                9UL

#define REPEAT               1UL
#define REPLACE              2UL
#define RGB332               4UL
#define RGB565               7UL
#define SRC_ALPHA            2UL
#define TEXT8X8              9UL
#define TEXTVGA              10UL
#define TOUCHMODE_CONTINUOUS 3UL
#define TOUCHMODE_FRAME      2UL
#define TOUCHMODE_OFF        0UL
#define TOUCHMODE_ONESHOT    1UL
#define ULAW_SAMPLES         1UL
#define ZERO                 0UL

#define DEV_ID_LOC 0x102400



#define FT_GPU_NUMCHAR_PERFONT (128)
#define FT_GPU_FONT_TABLE_SIZE (148)


#ifdef FT80x

    /* Definitions used for FT800 co processor command buffer */
    #define FT_DL_SIZE           (8*1024)  //8KB Display List buffer size
    #define FT_CMD_FIFO_SIZE     (4*1024)  //4KB coprocessor Fifo size
    #define FT_CMD_SIZE          (4)       //4 byte per coprocessor command of EVE





    #define RAM_CMD              1081344UL
    #define RAM_DL               1048576UL
    #define RAM_G                0UL
    #define RAM_PAL              1056768UL
    #define RAM_REG              1057792UL



    #define REG_ANALOG           1058104UL
    #define REG_ANA_COMP         1058160UL
    #define REG_CLOCK            1057800UL
    #define REG_CMD_DL           1058028UL
    #define REG_CMD_READ         1058020UL
    #define REG_CMD_WRITE        1058024UL
    #define REG_CPURESET         1057820UL
    #define REG_CRC              1058152UL
    #define REG_CSPREAD          1057892UL
    #define REG_CYA0             1058000UL
    #define REG_CYA1             1058004UL
    #define REG_CYA_TOUCH        1058100UL
    #define REG_DATESTAMP        1058108UL
    #define REG_DITHER           1057884UL
    #define REG_DLSWAP           1057872UL
    #define REG_FRAMES           1057796UL
    #define REG_FREQUENCY        1057804UL
    #define REG_GPIO             1057936UL
    #define REG_GPIO_DIR         1057932UL
    #define REG_HCYCLE           1057832UL
    #define REG_HOFFSET          1057836UL
    #define REG_HSIZE            1057840UL
    #define REG_HSYNC0           1057844UL
    #define REG_HSYNC1           1057848UL
    //#define REG_ID               1057792UL
    #define REG_ID               0x102400
//        #define REG_ID               0xC0000

    #define REG_INT_EN           1057948UL
    #define REG_INT_FLAGS        1057944UL
    #define REG_INT_MASK         1057952UL
    #define REG_MACRO_0          1057992UL
    #define REG_MACRO_1          1057996UL
    #define REG_OUTBITS          1057880UL
    #define REG_PCLK             1057900UL
    #define REG_PCLK_POL         1057896UL
    #define REG_PLAY             1057928UL
    #define REG_PLAYBACK_FORMAT  1057972UL
    #define REG_PLAYBACK_FREQ    1057968UL
    #define REG_PLAYBACK_LENGTH  1057960UL
    #define REG_PLAYBACK_LOOP    1057976UL
    #define REG_PLAYBACK_PLAY    1057980UL
    #define REG_PLAYBACK_READPTR 1057964UL
    #define REG_PLAYBACK_START   1057956UL
    #define REG_PWM_DUTY         1057988UL
    #define REG_PWM_HZ           1057984UL
    #define REG_RENDERMODE       1057808UL
    #define REG_ROMSUB_SEL       1058016UL
    #define REG_ROTATE           1057876UL
    #define REG_SNAPSHOT         1057816UL
    #define REG_SNAPY            1057812UL
    #define REG_SOUND            1057924UL
    #define REG_SWIZZLE          1057888UL
    #define REG_TAG              1057912UL
    #define REG_TAG_X            1057904UL
    #define REG_TAG_Y            1057908UL
    #define REG_TAP_CRC          1057824UL
    #define REG_TAP_MASK         1057828UL
    #define REG_TOUCH_ADC_MODE   1058036UL
    #define REG_TOUCH_CHARGE     1058040UL
    #define REG_TOUCH_DIRECT_XY  1058164UL
    #define REG_TOUCH_DIRECT_Z1Z2 1058168UL
    #define REG_TOUCH_MODE       1058032UL
    #define REG_TOUCH_OVERSAMPLE 1058048UL
    #define REG_TOUCH_RAW_XY     1058056UL
    #define REG_TOUCH_RZ         1058060UL
    #define REG_TOUCH_RZTHRESH   1058052UL
    #define REG_TOUCH_SCREEN_XY  1058064UL
    #define REG_TOUCH_SETTLE     1058044UL
    #define REG_TOUCH_TAG        1058072UL
    #define REG_TOUCH_TAG_XY     1058068UL
    #define REG_TOUCH_TRANSFORM_A 1058076UL
    #define REG_TOUCH_TRANSFORM_B 1058080UL
    #define REG_TOUCH_TRANSFORM_C 1058084UL
    #define REG_TOUCH_TRANSFORM_D 1058088UL
    #define REG_TOUCH_TRANSFORM_E 1058092UL
    #define REG_TOUCH_TRANSFORM_F 1058096UL
    #define REG_TRACKER          1085440UL
    #define REG_TRIM             1058156UL
    #define REG_VCYCLE           1057852UL
    #define REG_VOFFSET          1057856UL
    #define REG_VOL_PB           1057916UL
    #define REG_VOL_SOUND        1057920UL
    #define REG_VSIZE            1057860UL
    #define REG_VSYNC0           1057864UL
    #define REG_VSYNC1           1057868UL


#elif defined FT81x
    #define RAM_G                0x000000
    #define RAM_DL               0x300000
    #define RAM_REG              0x302000     
    #define RAM_CMD              0x308000
    
//        #define RAM_PAL              1056768UL
    
    
//        #define REG_ANALOG           1058104UL
//        #define REG_ANA_COMP         1058160UL
//        #define REG_CLOCK            1057800UL
//        #define REG_CMD_DL           1058028UL
    #define REG_CMD_READ         0x3020F8
    #define REG_CMD_WRITE        0x3020FC
    #define REG_CPURESET         0x302020
//        #define REG_CRC              1058152UL
    #define REG_CSPREAD          0x302068
//        #define REG_CYA0             1058000UL
//        #define REG_CYA1             1058004UL
//        #define REG_CYA_TOUCH        1058100UL
//        #define REG_DATESTAMP        1058108UL
//        #define REG_DITHER           1057884UL
    #define REG_DLSWAP           0x302054
//        #define REG_FRAMES           1057796UL
//        #define REG_FREQUENCY        1057804UL
    #define REG_GPIO             0x302094
    #define REG_GPIO_DIR         0x302098
    #define REG_HCYCLE           0x30202C
    #define REG_HOFFSET          0x302030
    #define REG_HSIZE            0x302034
    #define REG_HSYNC0           0x302038
    #define REG_HSYNC1           0x30203C
    #define REG_ID               0x302000
//
//        #define REG_INT_EN           1057948UL
//        #define REG_INT_FLAGS        1057944UL
//        #define REG_INT_MASK         1057952UL
//        #define REG_MACRO_0          1057992UL
//        #define REG_MACRO_1          1057996UL
//        #define REG_OUTBITS          1057880UL
    #define REG_PCLK             0x302070
    #define REG_PCLK_POL         0x30206C
    #define REG_PLAY             0x30208C
//        #define REG_PLAYBACK_FORMAT  1057972UL
//        #define REG_PLAYBACK_FREQ    1057968UL
//        #define REG_PLAYBACK_LENGTH  1057960UL
//        #define REG_PLAYBACK_LOOP    1057976UL
//        #define REG_PLAYBACK_PLAY    1057980UL
//        #define REG_PLAYBACK_READPTR 1057964UL
//        #define REG_PLAYBACK_START   1057956UL
    #define REG_PWM_DUTY         0x3020D4
    #define REG_PWM_HZ           0x3020D0
//        #define REG_RENDERMODE       1057808UL
//        #define REG_ROMSUB_SEL       1058016UL
    #define REG_ROTATE           0x302058
//        #define REG_SNAPSHOT         1057816UL
//        #define REG_SNAPY            1057812UL
    #define REG_SOUND               0x302088
    #define REG_SWIZZLE             0x302064
    #define REG_TAG                 0x30207C
    #define REG_TAG_X               0x302074
    #define REG_TAG_Y               0x302078
    #define REG_TAP_CRC             0x302024
    #define REG_TAP_MASK            0x302028
    #define REG_TOUCH_ADC_MODE      0x302108
    #define REG_CTOUCH_EXTENDED     REG_TOUCH_ADC_MODE   

    #define REG_TOUCH_CHARGE        0x30210C
    #define REG_TOUCH_DIRECT_XY     0x30218C
    #define REG_CTOUCH_TOUCH2_XY    REG_TOUCH_DIRECT_XY
    #define REG_TOUCH_DIRECT_Z1Z2   0x302190
    #define REG_CTOUCH_TOUCH3_XY    REG_TOUCH_DIRECT_Z1Z2
    #define REG_TOUCH_MODE          0x302104
    #define REG_TOUCH_OVERSAMPLE    0x302114
    #define REG_TOUCH_RAW_XY        0x30211C
    #define REG_CTOUCH_TOUCH1_XY    REG_TOUCH_RAW_XY
    #define REG_TOUCH_RZ            0x302120
    #define REG_CTOUCH_TOUCH4_Y     REG_TOUCH_RZ
    #define REG_TOUCH_RZTHRESH      0x302118
    #define REG_TOUCH_SCREEN_XY     0x302124
    #define REG_CTOUCH_TOUCH0_XY    REG_TOUCH_SCREEN_XY
    #define REG_TOUCH_SETTLE        0x302110
    #define REG_TOUCH_TAG_XY        0x302128
    #define REG_TOUCH_TAG           0x30212C
    #define REG_TOUCH_TAG1_XY       0x302130
    #define REG_TOUCH_TAG1          0x302134
    #define REG_TOUCH_TAG2_XY       0x302138
    #define REG_TOUCH_TAG2          0x30213C
    #define REG_TOUCH_TAG3_XY       0x302140
    #define REG_TOUCH_TAG3          0x302144
    #define REG_TOUCH_TAG4_XY       0x302148
    #define REG_TOUCH_TAG4          0x30214C
    
    #define REG_TOUCH_TRANSFORM_A   0x302150
    #define REG_TOUCH_TRANSFORM_B   0x302154
    #define REG_TOUCH_TRANSFORM_C   0x302158
    #define REG_TOUCH_TRANSFORM_D   0x30215C
    #define REG_TOUCH_TRANSFORM_E   0x302160
    #define REG_TOUCH_TRANSFORM_F   0x302164
    #define REG_TRACKER             0x309000

    #define REG_TRIM                0x302180
    #define REG_VCYCLE              0x302040
    #define REG_VOFFSET             0x302044
//        #define REG_VOL_PB           1057916UL
    #define REG_VOL_SOUND           0x302084
    #define REG_VSIZE               0x302048
    #define REG_VSYNC0              0x30204C
    #define REG_VSYNC1              0x302050
    
#endif


#define VERTEX2F(x,y) ((1UL<<30)|(((x)&32767UL)<<15)|(((y)&32767UL)<<0))
#define VERTEX2II(x,y,handle,cell) ((2UL<<30)|(((x)&511UL)<<21)|(((y)&511UL)<<12)|(((handle)&31UL)<<7)|(((cell)&127UL)<<0))
#define VERTEX_TRANSLATE_X(x) ((43UL<<24)|((((x)<<4)&65535UL)<<0))
#define BITMAP_SOURCE(addr) ((1UL<<24)|(((addr)&1048575UL)<<0))
#define CLEAR_COLOR_RGB(red,green,blue) ((2UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define TAG(s) ((3UL<<24)|(((s)&255UL)<<0))
#define COLOR_RGB(red,green,blue) ((4UL<<24)|(((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))
#define BITMAP_HANDLE(handle) ((5UL<<24)|(((handle)&31UL)<<0))
#define CELL(cell) ((6UL<<24)|(((cell)&127UL)<<0))
#define BITMAP_LAYOUT(format,linestride,height) ((7UL<<24)|(((format)&31UL)<<19)|(((linestride)&1023UL)<<9)|(((height)&511UL)<<0))
#define BITMAP_SIZE(filter,wrapx,wrapy,width,height) ((8UL<<24)|(((filter)&1UL)<<20)|(((wrapx)&1UL)<<19)|(((wrapy)&1UL)<<18)|(((width)&511UL)<<9)|(((height)&511UL)<<0))
#define ALPHA_FUNC(func,ref) ((9UL<<24)|(((func)&7UL)<<8)|(((ref)&255UL)<<0))
#define STENCIL_FUNC(func,ref,mask) ((10UL<<24)|(((func)&7UL)<<16)|(((ref)&255UL)<<8)|(((mask)&255UL)<<0))
#define BLEND_FUNC(src,dst) ((11UL<<24)|(((src)&7UL)<<3)|(((dst)&7UL)<<0))
#define STENCIL_OP(sfail,spass) ((12UL<<24)|(((sfail)&7UL)<<3)|(((spass)&7UL)<<0))
#define POINT_SIZE(size) ((13UL<<24)|(((size)&8191UL)<<0))
#define LINE_WIDTH(width) ((14UL<<24)|(((width)&4095UL)<<0))
#define CLEAR_COLOR_A(alpha) ((15UL<<24)|(((alpha)&255UL)<<0))
#define COLOR_A(alpha) ((16UL<<24)|(((alpha)&255UL)<<0))
#define CLEAR_STENCIL(s) ((17UL<<24)|(((s)&255UL)<<0))
#define CLEAR_TAG(s) ((18UL<<24)|(((s)&255UL)<<0))
#define STENCIL_MASK(mask) ((19UL<<24)|(((mask)&255UL)<<0))
#define TAG_MASK(mask) ((20UL<<24)|(((mask)&1UL)<<0))
#define BITMAP_TRANSFORM_A(a) ((21UL<<24)|(((a)&131071UL)<<0))
#define BITMAP_TRANSFORM_B(b) ((22UL<<24)|(((b)&131071UL)<<0))
#define BITMAP_TRANSFORM_C(c) ((23UL<<24)|(((c)&16777215UL)<<0))
#define BITMAP_TRANSFORM_D(d) ((24UL<<24)|(((d)&131071UL)<<0))
#define BITMAP_TRANSFORM_E(e) ((25UL<<24)|(((e)&131071UL)<<0))
#define BITMAP_TRANSFORM_F(f) ((26UL<<24)|(((f)&16777215UL)<<0))
#define SCISSOR_XY(x,y) ((27UL<<24)|(((x)&511UL)<<9)|(((y)&511UL)<<0))
#define SCISSOR_SIZE(width,height) ((28UL<<24)|(((width)&1023UL)<<10)|(((height)&1023UL)<<0))
#define CALL(dest) ((29UL<<24)|(((dest)&65535UL)<<0))
#define JUMP(dest) ((30UL<<24)|(((dest)&65535UL)<<0))
#define BEGIN(prim) ((31UL<<24)|(((prim)&15UL)<<0))
#define COLOR_MASK(r,g,b,a) ((32UL<<24)|(((r)&1UL)<<3)|(((g)&1UL)<<2)|(((b)&1UL)<<1)|(((a)&1UL)<<0))
#define CLEAR(c,s,t) ((38UL<<24)|(((c)&1UL)<<2)|(((s)&1UL)<<1)|(((t)&1UL)<<0))
#define END() ((33UL<<24))
#define SAVE_CONTEXT() ((34UL<<24))
#define RESTORE_CONTEXT() ((35UL<<24))
#define RETURN() ((36UL<<24))
#define MACRO(m) ((37UL<<24)|(((m)&1UL)<<0))
#define DISPLAY() ((0UL<<24))


/* FT800 font table structure */
/* Font table address in ROM can be found by reading the address from 0xFFFFC location. */
/* 16 font tables are present at the address read from location 0xFFFFC */
typedef struct FT_Gpu_Fonts
{
    /* All the values are in bytes */
    /* Width of each character font from 0 to 127 */
    uint8_t	FontWidth[FT_GPU_NUMCHAR_PERFONT];
    /* Bitmap format of font wrt bitmap formats supported by FT800 - L1, L4, L8 */
    unsigned int	FontBitmapFormat;
    /* Font line stride in FT800 ROM */
    unsigned int	FontLineStride;
    /* Font width in pixels */
    unsigned int	FontWidthInPixels;
    /* Font height in pixels */
    unsigned int	FontHeightInPixels;
    /* Pointer to font graphics raw data */
    unsigned int	PointerToFontGraphicsData;
} FT_Gpu_Fonts_t;


#ifdef __cplusplus
extern "C" {
#endif
    __weak uint8_t SPI_send(uint8_t);
    __weak void FT_spi_select(void);
    __weak void FT_spi_deselect(void);
    
    /* FT800 FUNCTIONS *****************************************************************/
    void HOST_CMD_ACTIVE(void);			/* send host command activate (wake-up command */
    void HOST_CMD_WRITE(uint8_t CMD);	/* send host command */

    void HOST_MEM_READ_STR(uint32_t addr, uint8_t *pnt, uint8_t len);	/* read len bytes of data from memory */
    void HOST_MEM_WR_STR(uint32_t addr, uint8_t *pnt, uint32_t len);		/* write len bytes of data into memory */

    void HOST_MEM_WR8(uint32_t addr, uint8_t data);		/* write  8bit (1byte)  data to memory */
    void HOST_MEM_WR16(uint32_t addr, uint32_t data);	/* write 16bit (2bytes) data to memory */
    void HOST_MEM_WR32(uint32_t addr, uint32_t data);	/* write 32bit (4bytes) data to memory */
    uint8_t HOST_MEM_RD8(uint32_t addr);				/* read  8bit  (1byte)  data from memory */
    uint32_t HOST_MEM_RD16(uint32_t addr);				/* read  16bit (2bytes) data from memory */
    uint32_t HOST_MEM_RD32(uint32_t addr);				/* read  32bit (4bytes) data from memory */

    /*** CO-PROCESSOR ******************************************************************/
    uint8_t cmd_ready(void);				/* check if co-processor is ready */
    uint8_t cmd(uint32_t data);				/* command function (tries to execute command max. 255 times) */
    uint8_t cmd_execute(uint32_t data);		/* execute function (returns 0: when failed to execute command, ie. co-p. is busy) */

    void cmd_track(int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag);										/* set touch engine for tracking */
    void cmd_spinner(int16_t x, int16_t y, uint16_t style, uint16_t scale);											/* draw spinner */
    void cmd_toogle(int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const uint8_t* s); 
    void cmd_slider(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range);	/* draw slider */

    void cmd_stop(void);   
    
    void cmd_setfont(uint32_t font, uint32_t ptr);
    void cmd_romfont(uint32_t font, uint32_t romslot);
    void cmd_text_var(int16_t x, int16_t y, int16_t font, uint16_t options, char* str, uint8_t length);
    
    void cmd_text(int16_t x, int16_t y, int16_t font, uint16_t options, const uint8_t* str);							/* draw text */
    void cmd_number(int16_t x, int16_t y, int16_t font, uint16_t options, uint32_t number);
    void cmd_setbase(uint32_t base);
    void cmd_button(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str);	/* draw button */
    void cmd_keys(int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char* str);		/* draw keyboard */
    void cmd_progress( int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range );
    
    void cmd_memzero(uint32_t ptr, uint32_t num);	/* write zero to a block of memory */

    void cmd_fgcolor(uint32_t c);			/* set widget foreground color */
    void cmd_bgcolor(uint32_t c);			/* set widget background color */
    void cmd_gradcolor(uint32_t c);			/* set 3d button highlight color */
    void cmd_gradient(int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1);	/* draw gradient */

    void cmd_loadidentity(void);				/* set current matrix to the identity matrix */
    void cmd_setmatrix(void);					/* write current matrix to the display list */
    void cmd_inflate(uint32_t ptr, uint8_t *image_addr, uint32_t filesize);

    void cmd_rotate(int32_t angle);				/* apply rotation to the current matrix */
    void cmd_translate(int32_t tx, int32_t ty);	/* apply translation to the current matrix */
    void cmd_scale(float sx, float sy );
    
    uint8_t initFT800(void);
    void clrscr(void);

    
    void vStartFrame(void);
    void vStopFrame(void);
    
    //Воробей 17.07.2018

    //суперудобная функция нарисовать линию
    void cmd_line(uint16_t x1, uint16_t y1,//координаты первой точки
              uint16_t x2, uint16_t y2,//координаты второй точки
              uint16_t width);//толщина
        
#ifdef __cplusplus
}
#endif        
        
#endif 
