#ifndef _BD_DEFINITIONS_H
#define _BD_DEFINITIONS_H

///////////////////BD_ID/////////////////////

#define BD_BDKG_07_ID                   0x00
#define BD_BDKG_06_ID                   0x01
#define BD_BDKG_05_ID                   0x02
#define BD_BDPA_01_ID                   0x05
#define BD_BDPB_01_ID                   0x06
#define BD_BDKG_03_ID                   0x07
#define BD_BDKG_10_ID                   0x08
#define BD_BDPA_02_ID                   0x09
#define BD_BDKG_11_ID                   0x0B
#define BD_BDKG_12_ID                   0x0C
#define BD_BDKG_13_ID                   0x0D
#define BD_BDKG_14_ID                   0x0E
#define BD_BDKG_15_ID                   0x0F
#define BD_BDKR_01_ID                   0x10
#define BD_BDKG_11_M_ID                 0x13
#define BD_BDPB_02_ID                   0x14
#define BD_BDKG_01_ID                   0x20
#define BD_BDKG_30_ID                   0x21
#define BD_BDKN_03_ID                   0x24
#define BD_BDKG_04_ID                   0x25
#define BD_BDKN_01_ID                   0x28
#define BD_BDKN_05_ID                   0x2A
#define BD_BDKG_11_ID                   0x0B
#define BD_BDKG_17_ID                   0x2B
#define BD_BDKG_22_ID                   0x2D
#define BD_BDKG_23_ID                   0x2E
#define BD_BDKG_24_ID                   0x37
#define BD_BDKG_32_ID                   0x48
#define BD_BDKG_35_ID                   0x1F
#define BD_BDKG_224_ID                  0x19
#define BD_BDKG_230_ID                  0x1A
#define BD_BDKG_201_M_ID                0x3B
#define BD_BDKG_203_M_ID                0x3C
#define BD_BDKG_205_M_ID                0x39
#define BD_BDKG_211_M_ID                0x3A


/////////////////Commands////////////////////

#define BD_READ_BIN_SIGN                0x02
#define BD_READ_CON_REG                 0x03
#define BD_READ_DATA_REG                0x04
#define BD_CON_SIGN                     0x05
#define BD_WRITE_CON_REG                0x06
#define BD_READ_STATE_WORD              0x07
#define BD_DIAGNOSTIC                   0x08
#define BD_WRITE_CONREGS                0x10
#define BD_READ_ID                      0x11

/////////////Controll signals////////////////

#define BD_AUTO_CHANGE_MAES_RANGE       0x01
#define BD_NEW_CONV_CICLE               0X22
#define BD_RESET_DOSE                   0x23
#define BD_SERCH_MODE                   0x24
#define BD_CHOOSE_BASE_COUNT            0x25
#define BD_FONE_SUB_MOD                 0x26
#define BD_CHOOSE_BASE_DOSE_RATE        0x27

/////////Diagnostic sub-commands/////////////

#define BD_RETURN_DATA                  0x00
#define BD_INIT                         0x01
#define BD_REED_DIAG_REGS               0x02

#endif	//#ifndef _BD_DEFINITIONS_H