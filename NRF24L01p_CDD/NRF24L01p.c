/**
 * @file   NRF24L01p.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief   An introductory character driver to support the second article of my series on
 * Linux loadable kernel module (LKM) development. This module maps to /dev/NRF24L01p and
 * comes with a helper C program that can be run in Linux user space to communicate with
 * this the LKM.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
 */
 
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/mutex.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/ioctl.h>
#include <asm/uaccess.h>          // Required for the copy to user function
#include <linux/fcntl.h>
#include <linux/errno.h>

#define  DEVICE_NAME "NRF24L01p"    ///< The device will appear at /dev/NRF24L01p using this value
#define  CLASS_NAME  "NRF24"        ///< The device class -- this is a character device driver

#include "NRF24L01pDriver.h"
#include "service.h"


MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Emon");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character Driver for NRF24L01+");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
 

static DEFINE_MUTEX(NRF24L01p_mutex);
static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  NRF24L01pClass  = NULL; ///< The device-driver class struct pointer
static struct device* NRF24L01pDevice = NULL; ///< The device-driver device struct pointer
 
// The prototype functions for the character driver -- must come before the struct definition
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static void process(uint8_t *data, unsigned int size);


/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};
 
/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init NRF24L01p_init(void){
   printk(KERN_INFO "NRF24L01p: Initializing the NRF24L01p LKM\n");
   mutex_init(&NRF24L01p_mutex);
   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "NRF24L01p: failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "NRF24L01p: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   NRF24L01pClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(NRF24L01pClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "NRF24L01p: Failed to register device class\n");
      return PTR_ERR(NRF24L01pClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "NRF24L01p: device class registered correctly\n");
 
   // Register the device driver
   NRF24L01pDevice = device_create(NRF24L01pClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(NRF24L01pDevice)){               // Clean up if there is an error
      class_destroy(NRF24L01pClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "NRF24L01p: Failed to create the device\n");
      return PTR_ERR(NRF24L01pDevice);
   }
   printk(KERN_INFO "NRF24L01p: device class created correctly\n"); // Made it! device was initialized
   return 0;
}
 
/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit NRF24L01p_exit(void){
   mutex_destroy(&NRF24L01p_mutex);
   device_destroy(NRF24L01pClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(NRF24L01pClass);                          // unregister the device class
   class_destroy(NRF24L01pClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "NRF24L01p: Goodbye from the LKM!\n");
}
 
/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){

   if(!mutex_trylock(&NRF24L01p_mutex)){
	printk(KERN_ALERT "NRF24L01p: Device in use by another process");
      	return -EBUSY;
   }

   numberOpens++;
   printk(KERN_INFO "NRF24L01p: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}
 
/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);
 
   if (error_count==0){            // if true then have success
      printk(KERN_INFO "NRF24L01p: Sent %d characters to the user : %s\n", size_of_message, message);
      return (size_of_message=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "NRF24L01p: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}
 
/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s", buffer);   // appending received string with its length
   //sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message

   process(message, size_of_message);

   return len;
}
 
/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   mutex_unlock(&NRF24L01p_mutex);
   printk(KERN_INFO "NRF24L01p: Device successfully closed\n");
   return 0;
}




static void process(uint8_t *data, unsigned int size){
   printk(KERN_INFO "NRF24L01p: Received %zu characters from the user\n", size);
   printk(KERN_INFO "NRF24L01p: And the message is : %s\n", data);

   char *tokenV[10];
   
   int n = parseString(message, strlen(message),  " -" , 10, tokenV);
    int i;
    for(i=0;i<n;i++){
        printk(KERN_INFO "token %d --> %s\r\n", i, tokenV[i]);
    }

    if(!strcmp(tokenV[0], "status")){
		//sprintf(data, "status %#x", NRF24L01p_Status());
		size_of_message = strlen(data);
		GPIO_export(21);
		GPIO_direction(21, 1);
		GPIO_write(21,0);
    }
    else if(!strcmp(tokenV[0], "on")){
		//sprintf(data, "status %#x", NRF24L01p_Status());
		size_of_message = strlen(data);
		GPIO_export(21);
		GPIO_direction(21, 1);
		GPIO_write(21,1);
    }
    else if(!strcmp(tokenV[0], "off")){
		//sprintf(data, "status %#x", NRF24L01p_Status());
		size_of_message = strlen(data);
		GPIO_export(21);
		GPIO_direction(21, 1);
		GPIO_write(21,0);
    }
    else {
		sprintf(data, "unknown command");
    }


   //sprintf(data, "swish swish swish");
   //size_of_message = strlen(data);

}




/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(NRF24L01p_init);
module_exit(NRF24L01p_exit);
