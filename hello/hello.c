#include <linux/module.h>  
#include <linux/kernel.h>

static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello world!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Remove the module\n");
}

module_init(hello_init);  
module_exit(hello_exit); 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dongli Zhang");
MODULE_DESCRIPTION("Android Kernel Module Hello World");

