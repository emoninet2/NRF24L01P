import sys
sys.path.insert(0, 'port')
from enum import Enum
from NRF24L01pPort import NRF24L01pPort
from NRF24L01pReg import *


class NRF24L01pHAL:
	def __init__(self):
		self.NRF24L01pPort = NRF24L01pPort()
	def read_Registers(self, addr, len):
		temp = [addr]
		for i in range (0,len):
			temp.append(0)
		return 	self.NRF24L01pPort.SPI_transceive(temp )[1:]
	def write_Registers(self, addr, data, len):
		temp = [(NRF24L01P_SPI_CMD_WR_REG | addr )] + data
		self.NRF24L01pPort.SPI_transceive(temp)
	def read_rx_payload(self, data, len):
		temp = [NRF24L01P_SPI_CMD_RD_RX_PAYLOAD] + data
		self.NRF24L01pPort.SPI_transceive(temp)[1:]
	def write_tx_payload(self, data, len):
		temp = [NRF24L01P_SPI_CMD_WR_TX_PAYLOAD] + data
		self.NRF24L01pPort.SPI_transceive(temp)
	def flush_tx(self):
		temp = [NRF24L01P_SPI_CMD_FLUSH_TX]
		self.NRF24L01pPort.SPI_transceive(temp)
	def flush_rx(self):
		temp = [NRF24L01P_SPI_CMD_FLUSH_RX]
		self.NRF24L01pPort.SPI_transceive(temp)
	def reuse_tx_payload(self):
		temp = [NRF24L01P_SPI_CMD_REUSE_TX_PL]
		self.NRF24L01pPort.SPI_transceive(temp)
	def read_rx_payload_width(self):
		temp = [NRF24L01P_SPI_CMD_R_RX_PL_WID, 0]
		return self.NRF24L01pPort.SPI_transceive(temp)[1:]
	def write_ack_payload(self, pipe, data, len):
		temp = [NRF24L01P_SPI_CMD_W_ACK_PAYLOAD | pipe] + data
		self.NRF24L01pPort.SPI_transceive(temp)
	def write_tx_payload_noack(self, pipe, data, len):
		temp = [NRF24L01P_SPI_CMD_W_TX_PYLD_NO_ACK] + data
		self.NRF24L01pPort.SPI_transceive(temp)
	def get_status(self):
		temp = [NRF24L01P_SPI_CMD_NOP,0]
		return self.NRF24L01pPort.SPI_transceive(temp)[1:]


	def power_up(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		if(enable == 1):
			temp[0] |= NRF24L01P_CONFIG_PWR_UP;
		else:
			temp[0] &= ~NRF24L01P_CONFIG_PWR_UP;
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);
	def rx_mode(self):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		temp[0] |= NRF24L01P_CONFIG_PRIM_RX;
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);
	def tx_mode(self):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		temp &= ~NRF24L01P_CONFIG_PRIM_RX;
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);
	def set_CRC(self, crc):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		temp[0] &= ~(NRF24L01P_CONFIG_CRC_MASK)
		temp[0] |= crc
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);
	def enable_dataReady_interrupt(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		if(enable == 0):
			temp[0] &= ~(NRF24L01P_CONFIG_MASK_RX_DR)
		else :
			temp[0] |= (NRF24L01P_CONFIG_MASK_RX_DR)
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);
	def enable_dataSent_interrupt(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		if(enable == 0):
			temp[0] &= ~(NRF24L01P_CONFIG_MASK_TX_DS)
		else :
			temp[0] |= (NRF24L01P_CONFIG_MASK_TX_DS)
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);
	def enable_maxRetry_interrupt(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_CONFIG,1)
		if(enable == 0):
			temp[0] &= ~(NRF24L01P_CONFIG_MASK_MAX_RT)
		else :
			temp[0] |= (NRF24L01P_CONFIG_MASK_MAX_RT)
		self.write_Registers(NRF24L01P_REG_CONFIG,temp,1);

	def enable_auto_ack(self, pipe, enable):
		temp = self.read_Registers(NRF24L01P_REG_EN_AA,1)
		if(enable == 0):
			temp[0] &= ~(1<<pipe)
		else :
			temp[0] |= (1<<pipe)
		self.write_Registers(NRF24L01P_REG_EN_AA,temp,1)

	def enable_rx_on_pipe(self, pipe, enable):
		temp = self.read_Registers(NRF24L01P_REG_EN_RXADDR,1)
		if(enable == 0):
			temp[0] &= ~(1<<pipe)
		else :
			temp[0] |= (1<<pipe)
		self.write_Registers(NRF24L01P_REG_EN_RXADDR,temp,1)
	def set_address_width(self, width):
		if((width >=3) and (width <=5) ):
			self.write_Registers(NRF24L01P_REG_SETUP_AW,[width-2],1)
	def get_address_width(self):
		return self.read_Registers(NRF24L01P_REG_SETUP_AW,1)[0] + 2
	def set_auto_retransmission_count(self, count):
		if (( count >=0) and ( count <= 15) ):
			temp = self.read_Registers(NRF24L01P_REG_SETUP_RETR,1)
			temp[0] &= ~ 0x0F;
			temp[0] |= (count<<0);
			self.write_Registers(NRF24L01P_REG_SETUP_RETR,temp,1);
	def get_auto_retransmission_count(self):
		temp = self.read_Registers(NRF24L01P_REG_SETUP_RETR,1)
		return (temp[0] & 0x0F); 
	def set_auto_retransmission_delay(self, us):
		times250us = int(us/250)
		#times250us = us/250.0 - (us%250.0/250.0)
		if((times250us >=0)and(times250us <= 15)):
			temp = self.read_Registers(NRF24L01P_REG_SETUP_RETR,1)
			temp[0] &= ~ 0xF0;
			temp[0] |= (times250us<<4);
			self.write_Registers(NRF24L01P_REG_SETUP_RETR,temp,1);
	def get_auto_retransmission_delay(self):
		temp = self.read_Registers(NRF24L01P_REG_SETUP_RETR,1)
		return ((temp[0] & 0xF0)>>4) * 250;
	def set_frequency(self,frequency):
		if((frequency>=2400) and (frequency<=2525 )):
			self.write_Registers(NRF24L01P_REG_RF_CH, [frequency-2400], 1);
	def get_frequency(self):
		freq = self.read_Registers(NRF24L01P_REG_RF_CH,1)
		return freq[0] + 2400
	def set_DataRate(self, DataRateKbps):
		temp = self.read_Registers(NRF24L01P_REG_RF_SETUP,1)
		temp[0]  &= ~NRF24L01P_RF_SETUP_RF_DR_MASK
		if(DataRateKbps == 250):
			temp[0]  |= RF_SETUP_RF_DR_250KBPS
		elif(DataRateKbps == 1024):
			temp[0]  |= RF_SETUP_RF_DR_1MBPS
		elif(DataRateKbps == 2048):
			temp[0]  |= RF_SETUP_RF_DR_2MBPS
		else:
			temp[0]  |= RF_SETUP_RF_DR_2MBPS
		self.write_Registers(NRF24L01P_REG_RF_SETUP,temp,1)
	def get_DataRate(self):
		temp = self.read_Registers(NRF24L01P_REG_RF_SETUP,1)
		temp[0] &= NRF24L01P_RF_SETUP_RF_DR_MASK

		if(temp[0] == RF_SETUP_RF_DR_250KBPS):
			DataRate = 250
		elif(temp[0]  == RF_SETUP_RF_DR_1MBPS):
			DataRate = 1024
		elif(temp[0]  == RF_SETUP_RF_DR_2MBPS):
			DataRate = 2048
		return DataRate
	def set_RF_Power(self, power):
		temp = self.read_Registers(NRF24L01P_REG_RF_SETUP,1)
		temp[0] &= ~NRF24L01P_RF_SETUP_RF_PWR_MASK;
		if(power == 0):
			temp[0] |= RF_SETUP_RF_PWR_0DBM
		elif (power == -6):
			temp[0] |= RF_SETUP_RF_PWR_MINUS_6DBM
		elif (power == -12):
			temp[0] |= RF_SETUP_RF_PWR_MINUS_12DBM
		elif (power == -18):
			temp[0] |= RF_SETUP_RF_PWR_MINUS_18DBM
		else:
			temp[0] |= RF_SETUP_RF_PWR_0DBM #default
		self.write_Registers(NRF24L01P_REG_RF_SETUP,temp,1)
	def get_RF_Power(self):
		temp = self.read_Registers(NRF24L01P_REG_RF_SETUP,1)
		temp[0] &= NRF24L01P_RF_SETUP_RF_PWR_MASK;
		if(temp[0] == RF_SETUP_RF_PWR_0DBM):
			return 0
		elif (temp[0] == RF_SETUP_RF_PWR_MINUS_6DBM):
			return -6
		elif (temp[0] == RF_SETUP_RF_PWR_MINUS_12DBM):
			return -12
		elif (temp[0] == RF_SETUP_RF_PWR_MINUS_18DBM):
			return -18
	
	def enable_pll_lock(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_RF_SETUP,1)
		if(enable == 0):
			temp[0] &= ~NRF24L01P_RF_SETUP_PLL_LOCK;
		else:
			temp[0] |= NRF24L01P_RF_SETUP_PLL_LOCK;
		self.write_Registers(NRF24L01P_REG_RF_SETUP,temp,1)
	def enable_cont_wave(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_RF_SETUP,1)
		if(enable == 0):
			temp[0] &= ~NRF24L01P_RF_CONT_WAVE;
		else:
			temp[0] |= NRF24L01P_RF_CONT_WAVE;
		self.write_Registers(NRF24L01P_REG_RF_SETUP,temp,1)
	def get_tx_fifo_full_flag(self):
		if not(self.get_status() and NRF24L01P_STATUS_TX_FULL):
			return 0
		else:
			return 1
	def get_max_retry_flag(self):
		if not(self.get_status() and NRF24L01P_STATUS_MAX_RT):
			return 0
		else:
			return 1
	def get_tx_fifo_full_flag(self):
		if not(self.get_status() and NRF24L01P_STATUS_MAX_RT):
			return 0
		else:
			return 1	
	def get_data_sent_flag(self):
		if not(self.get_status() and NRF24L01P_STATUS_TX_DS):
			return 0
		else:
			return 1
	def get_data_ready_flag(self):
		if not(self.get_status() and NRF24L01P_STATUS_RX_DR):
			return 0
		else:
			return 1	
	def clear_tx_max_retry_flag(self):
		temp = [NRF24L01P_STATUS_MAX_RT]
		self.write_Registers(NRF24L01P_REG_STATUS,temp,1)
	def clear_data_sent_flag(self):
		temp = [NRF24L01P_STATUS_TX_DS]
		self.write_Registers(NRF24L01P_REG_STATUS,temp,1)
	def clear_data_ready_flag(self):
		temp = [NRF24L01P_STATUS_RX_DR]
		c
	def get_rx_payload_pipe(self):
		return (((get_status()&_NRF24L01P_STATUS_RX_P_NO)>>1)&0x07)

	def get_arg_count(self):
		temp = self.read_Registers(NRF24L01P_REG_OBSERVE_TX,1)
		return ((temp&NRF24L01P_OBSERVE_TX_ARC_CNT_MASK)>>NRF24L01P_OBSERVE_TX_ARC_CNT_BP)
	def get_plos_count(self):
		temp = self.read_Registers(NRF24L01P_REG_OBSERVE_TX,1)
		return ((temp&_NRF24L01P_OBSERVE_TX_PLOS_CNT_MASK)>>_NRF24L01P_OBSERVE_TX_PLOS_CNT_BP)
	def clear_plos_count(self):
		temp = self.read_Registers(NRF24L01P_REG_RF_CH,1)
		self.write_Register(NRF24L01P_REG_RF_CH, temp, 1)
	def get_rpd(self):
		return self.read_Registers(NRF24L01P_REG_RPD,1)[0]
		

	def set_RX_pipe_address(self, pipe, address):
		if((pipe>=0)and(pipe<=5)):
			if(pipe == 0):
				len = self.get_address_width()
			elif((pipe>=1)and(pipe<=5)):
				len = 1
			self.write_Registers(NRF24L01P_REG_RX_ADDR_P0 + pipe,address,len)
	def get_RX_pipe_address(self, pipe):
		if((pipe>=0)and(pipe<=5)):
			if(pipe == 0):
				len = self.get_address_width()
			elif((pipe>=1)and(pipe<=5)):
				len = 1
			return self.read_Registers(NRF24L01P_REG_RX_ADDR_P0 + pipe,len)
	def set_TX_pipe_address(self, address):
		self.write_Registers(NRF24L01P_REG_TX_ADDR, address, self.get_address_width())
	def get_TX_pipe_address(self):
		return self.read_Registers(NRF24L01P_REG_TX_ADDR,self.get_address_width())
	def set_RX_pipe_width(self,pipe,width):
		temp = [width]
		self.write_Registers(NRF24L01P_REG_RX_PW_P0+pipe,temp,1);
	def get_RX_pipe_width(self,pipe):
		return self.read_Registers(NRF24L01P_REG_RX_PW_P0+pipe,1)[0]
	

	def get_fifo_flag_rx_empty(self):
		temp = self.read_Registers(NRF24L01P_REG_FIFO_STATUS,1)
		if not(temp[0]& NRF24L01P_FIFO_STATUS_RX_EMPTY):
			return 0
		else:
			return 1
	def get_fifo_flag_rx_full(self):
		temp = self.read_Registers(NRF24L01P_REG_FIFO_STATUS,1)
		if not(temp[0]& NRF24L01P_FIFO_STATUS_RX_FULL):
			return 0
		else:
			return 1
	def get_fifo_flag_tx_empty(self):
		temp = self.read_Registers(NRF24L01P_REG_FIFO_STATUS,1)
		if not(temp[0] & NRF24L01P_FIFO_STATUS_TX_EMPTY):
			return 0
		else:
			return 1
	def get_fifo_flag_tx_full(self):
		temp = self.read_Registers(NRF24L01P_REG_FIFO_STATUS,1)
		if not(temp[0]& NRF24L01P_FIFO_STATUS_TX_FULL):
			return 0
		else:
			return 1
	def get_fifo_flag_tx_reuse(self):
		temp = self.read_Registers(NRF24L01P_REG_FIFO_STATUS,1)
		if not(temp[0]& NRF24L01P_FIFO_STATUS_RX_REUSE):
			return 0
		else:
			return 1
	def enable_dynamic_payload_on_pipe(self, pipe, enable):
		temp = self.read_Registers(NRF24L01P_REG_DYNPD,1)
		if(enable==1):
			temp[0] |= (1<<pipe)
		else:
			temp[0] &= ~(1<<pipe)
		self.write_Registers(NRF24L01P_REG_DYNPD,temp,1)
	def enable_feature_dynamic_payload(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_FEATURE,1)
		if(enable==1):
			temp[0] |= NRF24L01_FEATURE_EN_DPL
		else:
			temp[0] &= ~NRF24L01_FEATURE_EN_DPL
		self.write_Registers(NRF24L01P_REG_FEATURE,temp,1)
	def enable_feature_payload_with_ack(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_FEATURE,1)
		if(enable==1):
			temp[0] |= NRF24L01_FEATURE_EN_ACK_PAY
		else:
			temp[0] &= ~NRF24L01_FEATURE_EN_ACK_PAY
		self.write_Registers(NRF24L01P_REG_FEATURE,temp,1)

	def enable_feature_dynamic_payload_with_no_ack(self, enable):
		temp = self.read_Registers(NRF24L01P_REG_FEATURE,1)
		if(enable==1):
			temp[0] |= NRF24L01_FEATURE_EN_DYN_ACK
		else:
			temp[0] &= ~NRF24L01_FEATURE_EN_DYN_ACK
		self.write_Registers(NRF24L01P_REG_FEATURE,temp,1)