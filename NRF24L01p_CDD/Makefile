obj-m := NRF24L01p.o 
 

#KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)
KERNEL_DIR = /usr/src/linux-headers-4.9.24-v7+


all: 
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~

perm:
	sudo chmod 666 /dev/NRF24L01p

rmentry:
	sudo rm /dev/NRF24L01p

install:
	sudo insmod NRF24L01p.ko

remove:
	sudo rmmod NRF24L01p

