#ifndef CDROM_H
#define CDROM_H

#include <mmio.h>

#define COM_DELAY 0x1F801020
#define DATA_SECTOR_SIZE 2048

typedef enum cd_reg : uintptr_t {
    CD_REG0 = 0x1F801800,
    CD_REG1 = 0x1F801801,
    CD_REG2 = 0x1F801802,
    CD_REG3 = 0x1F801803
} CD_REGS;

typedef enum cd_reg0_w_xb : uint8_t {
    CD_R0_BANK0 = 0x00,
    CD_R0_BANK1 = 0x01,
    CD_R0_BANK2 = 0x02,
    CD_R0_BANK3 = 0x03,
} CD_ADDRESS;

typedef enum cd_reg0_r_xb : uint8_t{
    CD_ADPCM_BUSY = 0x04,
    CD_PRM_EMPTY = 0x08,
    CD_PRM_WRDY = 0x10,
    CD_RES_RRDY = 0x20,
    CD_DATA_REQ = 0x40,
    CD_BUSY = 0x80
} CD_HSTS;

typedef enum cd_reg1_w_b0 : uint8_t {
    CD_CMD_NOP = 0x01,
    CD_CMD_SETLOC = 0x02,
    CD_CMD_PLAY = 0x03,
    CD_CMD_FWD = 0x04,
    CD_CMD_REV = 0x05,
    CD_CMD_READ_N = 0x06,
    CD_CMD_STANDBY = 0x07,
    CD_CMD_STOP = 0x08,
    CD_CMD_PAUSE = 0x09,
    CD_CMD_INIT = 0x0A,
    CD_CMD_MUTE = 0x0B,
    CD_CMD_DEMUTE = 0x0C,
    CD_CMD_SETF = 0x0D,
    CD_CMD_SETMODE = 0x0E,
    CD_CMD_GETPARAM = 0x0F,
    CD_CMD_GETLOC_L = 0x10,
    CD_CMD_GETLOC_P = 0x11,
    CD_CMD_SETSESSION = 0x12,
    CD_CMD_GET_T_NUM = 0x13,
    CD_CMD_GET_T_DAT = 0x14,
    CD_CMD_SEEK_L = 0x15,
    CD_CMD_SEEK_P = 0x16,
    CD_CMD_TEST = 0x19,
    CD_CMD_GET_ID = 0x1A,
    CD_CMD_READ_S = 0x1B,
    CD_CMD_RESET = 0x1C
} CD_COMMAND;

typedef enum cd_reg3_w_b0 : uint8_t {
    CD_HCHPCTL_INIT = 0x00,
    CD_BFWR = 0x40,
    CD_BFRD = 0x80
} CD_HCHPCTL;

typedef enum cd_reg3_r_b1b3 : uint8_t {
    CD_INTERRUPT_RMASK = 0x07,
    CD_IRQ_S_NOIRQ = 0x0,
    CD_IRQ_S_DATA_RDY = 0x1,
    CD_IRQ_S_CMD_FIN = 0x2,
    CD_IRQ_S_ACK = 0x3,
    CD_IRQ_S_EOD = 0x4,
    CD_IRQ_S_ERR = 0x5    
} CD_HINTSTS;

typedef enum cd_reg2_w_b1 : uint8_t {
    CD_DISABLE_IRQ = 0x00,
    CD_ENABLE_IRQ = 0x1F,
    CD_INTSTS0 = 0x01,
    CD_INTSTS1 = 0x02,
    CD_XINTSTS = 0x03
} CD_HINTMSK;

typedef enum cd_reg3_w_b1 : uint8_t {
    CD_ACK_IRQ = 0x1F,
    CD_CLS_P_FIFO = 0x40,
    CD_RST_DEC = 0x80
} CD_HCLRCTL;

typedef enum cd_reg2_w_b0 : uint8_t {
    CD_ARG_CDDA = 0x01,
    CD_ARG_AUTOPAUSE = 0x02,
    CD_ARG_REPORT = 0x04,
    CD_ARG_SF = 0x08,
    CD_ARG_SIZE2 = 0x10,
    CD_ARG_SIZE = 0x20,
    CD_ARG_ADPCM = 0x40,
    CD_ARG_D_SPEED = 0x80
} CD_ARGUMENT;

typedef enum cd_stat_mask : uint8_t {
    CD_STAT_ERR = 0x01,
    CD_STAT_STANDBY = 0x02,
    CD_STAT_SEEK_ERR = 0x04,
    CD_STAT_SHELL_OPEN = 0x08,
    CD_STAT_DATA_READING = 0x10,
    CD_STAT_SEEKING = 0x20
} CD_STATUS_MASK;


typedef enum cd_setmode : uint8_t {
    CD_MODE_NORMAL_SPEED = 0x00,
    CD_MODE_DOUBLE_SPEED = 0x80,
    CD_MODE_ADPCM_OFF    = 0x00,
    CD_MODE_ADPCM_ON     = 0x40,
    CD_MODE_SIZE_2048    = 0x00,
    CD_MODE_SIZE_2340    = 0x20,
    CD_MODE_SIZE_2328    = 0x10,
    CD_MODE_SF_OFF       = 0x00,
    CD_MODE_SF_ON        = 0x08,
    CD_MODE_REPORT_OFF   = 0x00,
    CD_MODE_REPORT_ON    = 0x04,
    CD_MODE_AUTOPAUSE_OFF = 0x00,
    CD_MODE_AUTOPAUSE_ON  = 0x02,
    CD_MODE_CDDA_OFF     = 0x00,
    CD_MODE_CDDA_ON      = 0x01,
} CD_MODE;

#define CDROM_RDY !(_MMIO8(CD_REG0)&CD_BUSY)

#define CDROM_SYNC while (!CDROM_RDY){ __asm__ volatile("");}

void cdrom_irq(void);

void cdrom_init(void);

bool cdrom_issue_cmd(CD_COMMAND cmd, uint8_t argc, CD_ARGUMENT *argv, CD_HINTSTS expected_response, bool sync);

extern uint8_t cdrom_command_response[16];

extern bool cdrom_up;

extern volatile CD_HINTSTS cdrom_current_status;

#define CDROM_ERR (cdrom_current_status == CD_IRQ_S_ERR)



#endif