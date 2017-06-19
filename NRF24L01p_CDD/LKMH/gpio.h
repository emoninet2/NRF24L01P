

#include <linux/gpio.h>

void GPIO_export (int pin){
	gpio_export(pin,1);
}
void GPIO_unexport (int pin){
	gpio_unexport(pin);
}
void GPIO_direction(int pin, int dir){
	if(dir){
		gpio_direction_output(pin, 0);
	}
	else{
		gpio_direction_input(pin);
	}
}
void GPIO_write(int pin, int val){
	gpio_set_value(pin, val);
}
int GPIO_read(int pin){
	return gpio_get_value(pin);
}


