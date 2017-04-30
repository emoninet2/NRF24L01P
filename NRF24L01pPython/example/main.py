import sys
sys.path.insert(0, '../port')
sys.path.insert(0, '..')
from NRF24L01pHAL import NRF24L01pHAL



Radio = NRF24L01pHAL()

print("EMON is AWESOME");

Rxaddr = [122,45,76,34,23]
Txaddr = [25,192,223,97,54]
#Radio.write_Registers(0x0a, addr ,1)

#Radio.set_DataRate(250)
addr = 0x02

Radio.set_address_width(4);
#Radio.set_auto_retransmission_count(7);
#Radio.set_auto_retransmission_delay(2300);
#Radio.enable_rx_on_pipe(3,0)
#Radio.set_RF_Power(-4)
Radio.set_TX_pipe_address(Txaddr)
Radio.set_RX_pipe_width(3,20);
Radio.enable_feature_dynamic_payload(1)


#Radio.set_frequency(2468)

r = Radio.read_Registers(addr,1)
w = Radio.get_address_width();
f = Radio.get_frequency()
d = Radio.get_DataRate()
ac = Radio.get_auto_retransmission_count();
ad = Radio.get_auto_retransmission_delay();
p = Radio.get_RF_Power();

f1 = Radio.get_fifo_flag_rx_empty()
f2 = Radio.get_fifo_flag_rx_full()
f3 = Radio.get_fifo_flag_tx_empty()
f4 = Radio.get_fifo_flag_tx_full()
f5 = Radio.get_fifo_flag_tx_reuse()

#Radio.set_RX_pipe_address(0,Rxaddr)
rp0 = Radio.get_RX_pipe_address(0)
tx = Radio.get_TX_pipe_address()
pw = Radio.get_RX_pipe_width(3)



#print(r)
#print(f)
#print(d)
#print(w)
#print(ac)
#print(ad)
#print(p)
#print(rp0)
#print(tx)
#print(pw)

#print(hex(f1))
#print(hex(f2))
#print(hex(f3))
#print(hex(f4))
#print(hex(f5))

Radio.NRF24L01pPort.write_Pin_CSN(0)