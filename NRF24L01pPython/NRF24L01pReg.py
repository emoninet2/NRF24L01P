from enum import Enum


NRF24L01P_SPI_CMD_RD_REG            = 0x00
NRF24L01P_SPI_CMD_WR_REG            =0x20
NRF24L01P_SPI_CMD_RD_RX_PAYLOAD     = 0x61
NRF24L01P_SPI_CMD_WR_TX_PAYLOAD     = 0xa0
NRF24L01P_SPI_CMD_FLUSH_TX          = 0xe1
NRF24L01P_SPI_CMD_FLUSH_RX          = 0xe2
NRF24L01P_SPI_CMD_REUSE_TX_PL       = 0xe3
NRF24L01P_SPI_CMD_R_RX_PL_WID       = 0x60
NRF24L01P_SPI_CMD_W_ACK_PAYLOAD     = 0xa8
NRF24L01P_SPI_CMD_W_TX_PYLD_NO_ACK  = 0xb0
NRF24L01P_SPI_CMD_NOP               = 0xff




NRF24L01P_REG_CONFIG                = 0x0 
NRF24L01P_REG_EN_AA                 = 0x1 
NRF24L01P_REG_EN_RXADDR             = 0x2 
NRF24L01P_REG_SETUP_AW              = 0x3 
NRF24L01P_REG_SETUP_RETR            = 0x4 
NRF24L01P_REG_RF_CH                 = 0x5 
NRF24L01P_REG_RF_SETUP              = 0x6 
NRF24L01P_REG_STATUS                = 0x7 
NRF24L01P_REG_OBSERVE_TX            = 0x8 
NRF24L01P_REG_RPD                   = 0x9 
NRF24L01P_REG_RX_ADDR_P0            = 0xa 
NRF24L01P_REG_RX_ADDR_P1            = 0xb 
NRF24L01P_REG_RX_ADDR_P2            = 0xc 
NRF24L01P_REG_RX_ADDR_P3            = 0xd 
NRF24L01P_REG_RX_ADDR_P4            = 0xe 
NRF24L01P_REG_RX_ADDR_P5            = 0xf 
NRF24L01P_REG_TX_ADDR               = 0x10 
NRF24L01P_REG_RX_PW_P0              = 0x11 
NRF24L01P_REG_RX_PW_P1              = 0x12 
NRF24L01P_REG_RX_PW_P2              = 0x13 
NRF24L01P_REG_RX_PW_P3              = 0x14 
NRF24L01P_REG_RX_PW_P4              = 0x15 
NRF24L01P_REG_RX_PW_P5              = 0x16 
NRF24L01P_REG_FIFO_STATUS           = 0x17 
NRF24L01P_REG_DYNPD                 = 0x1c 
NRF24L01P_REG_FEATURE               = 0x1d 
NRF24L01P_REG_ADDRESS_MASK          = 0x1f 




NRF24L01P_CONFIG_PRIM_RX        = (1<<0)
NRF24L01P_CONFIG_PWR_UP         = (1<<1)
NRF24L01P_CONFIG_CRC0           = (1<<2)
NRF24L01P_CONFIG_EN_CRC         = (1<<3)
NRF24L01P_CONFIG_MASK_MAX_RT    = (1<<4)
NRF24L01P_CONFIG_MASK_TX_DS     = (1<<5)
NRF24L01P_CONFIG_MASK_RX_DR     = (1<<6)
NRF24L01P_CONFIG_CRC_MASK       = (NRF24L01P_CONFIG_EN_CRC | NRF24L01P_CONFIG_CRC0)

NRF24L01P_RF_SETUP_RF_PWR_MASK         = (0x3<<1)
NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT      = (1 << 3)
NRF24L01P_RF_SETUP_RF_DR_LOW_BIT       = (1 << 5)
NRF24L01P_RF_SETUP_RF_DR_MASK          = (NRF24L01P_RF_SETUP_RF_DR_LOW_BIT | NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT)
NRF24L01P_RF_SETUP_PLL_LOCK            = (1<<4)
NRF24L01P_RF_CONT_WAVE                 = (1<<7)


NRF24L01P_STATUS_TX_FULL       = (1<<0)
NRF24L01P_STATUS_RX_P_NO       = (0x7<<1)
NRF24L01P_STATUS_MAX_RT        = (1<<4)
NRF24L01P_STATUS_TX_DS         = (1<<5)
NRF24L01P_STATUS_RX_DR         = (1<<6)

NRF24L01P_OBSERVE_TX_ARC_CNT_BP			=	0
NRF24L01P_OBSERVE_TX_ARC_CNT_MASK		=	0x0F
NRF24L01P_OBSERVE_TX_PLOS_CNT_BP		=	4
NRF24L01P_OBSERVE_TX_PLOS_CNT_MASK		=	0xF0



NRF24L01P_FIFO_STATUS_RX_EMPTY		=	(1<<0)
NRF24L01P_FIFO_STATUS_RX_FULL		=	(1<<1)
NRF24L01P_FIFO_STATUS_TX_EMPTY		=	(1<<4)
NRF24L01P_FIFO_STATUS_TX_FULL		=	(1<<5)
NRF24L01P_FIFO_STATUS_RX_REUSE		=	(1<<6)



NRF24L01_FEATURE_EN_DPL		=	(1<<2)
NRF24L01_FEATURE_EN_ACK_PAY	=	(1<<1)
NRF24L01_FEATURE_EN_DYN_ACK	=	(1<<0)


NRF24L01P_RX_PW_Px_MASK      	=   0x3F


NRF24L01P_TIMING_PowerOnReset_ms =   100   # // 100mS
NRF24L01P_TIMING_Tundef2pd_us    = 100000  # // 100mS
NRF24L01P_TIMING_Tstby2a_us      =    130  # // 130uS
NRF24L01P_TIMING_Thce_us         =     10  # //  10uS
NRF24L01P_TIMING_Tpd2stby_us     =   4500  # // 4.5mS worst case
NRF24L01P_TIMING_Tpece2csn_us    =      4  # //   4uS




CONFIG_CRC_NONE      =  (0)
CONFIG_CRC_8BIT      =  (NRF24L01P_CONFIG_EN_CRC)
CONFIG_CRC_16BIT     =  (NRF24L01P_CONFIG_EN_CRC|NRF24L01P_CONFIG_CRC0)



SETUP_AW_AW_3BYTE   =  (0x1<<0) #/**< 3 bytes address width */
SETUP_AW_AW_4BYTE   =  (0x2<<0) #/**< 4 bytes address width */
SETUP_AW_AW_5BYTE   =  (0x3<<0) #/**< 5 bytes address width */



RF_SETUP_RF_PWR_0DBM        =  (0x3<<1)
RF_SETUP_RF_PWR_MINUS_6DBM  =  (0x2<<1)
RF_SETUP_RF_PWR_MINUS_12DBM =  (0x1<<1)
RF_SETUP_RF_PWR_MINUS_18DBM =  (0x0<<1)



RF_SETUP_RF_DR_250KBPS    =    (NRF24L01P_RF_SETUP_RF_DR_LOW_BIT)
RF_SETUP_RF_DR_1MBPS      =    (0)
RF_SETUP_RF_DR_2MBPS      =    (NRF24L01P_RF_SETUP_RF_DR_HIGH_BIT)


PIPE_P0       =    0
PIPE_P1       =    1
PIPE_P2       =    2
PIPE_P3       =    3
PIPE_P4       =    4
PIPE_P5       =    5
