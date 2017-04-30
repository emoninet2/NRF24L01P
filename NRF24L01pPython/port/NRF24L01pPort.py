import spidev
import RPi.GPIO as GPIO
import time

class NRF24L01pPort:
	def __init__(self):
		self.csn_pin = 26
		self.ce_pin = 32

		GPIO.setwarnings(False)
		GPIO.cleanup()
		GPIO.setmode(GPIO.BOARD)
		GPIO.setup(self.csn_pin, GPIO.OUT) 
		GPIO.setup(self.ce_pin, GPIO.OUT) 

		self.spi = spidev.SpiDev()
		self.spi.open(0,1)

	def SPI_transceive(self,dataInOut):
		return self.spi.xfer2(dataInOut)
	
	def write_Pin_CE(self, val):
		GPIO.output(self.ce_pin,val)
	
	def write_Pin_CSN(self, val):
		GPIO.output(self.csn_pin,val)
	
	def read_Pin_CE(self):
		GPIO.input(self.ce_pin)
	
	def read_Pin_CSN(self):
		GPIO.input(self.csn_pin)
	
	def DelayMs(self, ms):
		time.sleep(ms/1000.0)
	
	def DelayUs(self, us):
		time.sleep(ms/1000000.0)
