/**
 * @file LTC6804Cmd.h
 * @brief LTC6804命令宏定义
 */
#ifndef LTC6804CMD_H
#define LTC6804CMD_H

#include "stm32f10x.h"

/*!*******************************************************************************************************************
 \brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands
|command	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] | 
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
*/

#define WRCFG 0x0001
#define RDCFG 0x0002
#define RDCVA 0x0004
#define RDCVB 0x0006
#define RDCVC 0x0008
#define RDCVD 0x000A
#define RDAUXA 0x000C
#define RDAUXB 0x000E
#define RDSTATA 0x0010
#define RDSTATB 0x0012
#define ADCV(md, dcp, ch) (0x0260 | ((md & 0x03) << 7) | ((dcp & 0x01) <<4) | (ch & 0x07))
#define ADAX(md, dcp, chg) (0x0460 | ((md & 0x03) << 7) | ((dcp & 0x01) <<4) | (chg & 0x07))
#define CLRCELL 0x0711
#define CLRAUX 0x0712

#endif
