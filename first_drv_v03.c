/*
 * 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/irq.h>
//#include <asm/arch/regs-gpio.h>
//#include <include/linux/sunxi-gpio.h>
//#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/io.h>
#include <linux/sunxi-gpio.h>

*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/rwsem.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/ctype.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/sunxi-gpio.h>
#include <linux/io.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
//#include "../base/base.h"
//#include <../drivers/base/base.h>
#include "/media/roger/f_land/tina/lichee/linux-4.9/drivers/base/base.h"
#define GPIO_BASE 0x0300b000

int major;
static struct class *firstdrv_class; //make a device class 
static struct device *firstdrv_class_devs;// make a device for this class


volatile unsigned long *GPHcon=NULL;
volatile unsigned long *GPHdat=NULL;
volatile unsigned long *gpio_base=NULL;
volatile unsigned long GPHcon_val,GPHdat_val;
/*1写出驱动程序first_drv_open first_drv_write */
/*inode结构表示具体的文件，file结构体用来追踪文件在运行时的状态信息。*/
static int first_drv_open(struct inode *inode, struct file  *file)
{
    //*GPHcon &= ~((0x11<<8) | (0x11<<10) | (0x11<<12));
    //*GPHcon |=  ((0x01<<8) | (0x01<<10) | (0x01<<12));
        GPHcon_val = readl(GPHcon);
	GPHcon_val &= 0x0fffffff; //clear high end 4bit [31~28]
        GPHcon_val |= 0x1<<28;    //
	writel(GPHcon_val,GPHcon);
      //*GPHcon |= 0x1<<28;
    printk("first_drv_open\n");      //打印，在内核中打印只能用printk()
    printk("GPHcon value is 0x%lx \n",readl(GPHcon));

    return 0;
}

/*参数filp为目标文件结构体指针，buffer为要写入文件的信息缓冲区，count为要写入信息的长度，ppos为当前的偏移位置，这个值通常是用来判断写文件是否越界*/
static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
    int val;
    copy_from_user(&val,buf,count);
    if(val==1)
    {
             // *GPHdat &=~ ((0x1<<4) | (0x1<<5) | (0x1<<6));
    	GPHdat_val = readl(GPHdat);
     	GPHdat_val |= 0x1<<7 ;
	writel(GPHdat_val,GPHdat);
    printk("GPHdat value write is 0x1<<7,and read value is 0x%lx \n",readl(GPHdat));
    printk("first_drv_write\n");      //打印，在内核中打印只能用printk()

    }
    else
    {
    	GPHdat_val = readl(GPHdat);
     	GPHdat_val &=~ 0x1<<7 ;
	writel(GPHdat_val,GPHdat);

    printk("GPHdat value write is 0x1<<7,and read value is 0x%lx \n",readl(GPHdat));
    printk("first_drv_write\n");      //打印，在内核中打印只能用printk()

    }
        return 0;
}
 
/*2定义file_operations结构体来封装驱动函数first_drv_open first_drv_write */
static struct file_operations first_drv_fops = {
    .owner  =   THIS_MODULE,    // 被使用时阻止模块被卸载
    .open   =   first_drv_open,      
    .write  =   first_drv_write,   
};

/*4写first_drv_init入口函数来调用这个register_chrdev()注册函数*/
int first_drv_init(void)
{
	//int request_check;
	//request_mem_region(0x0300b000,0x400,"GPIO");
	printk("----------------------------\n");
        printk("gpio ioremap will star \n");

	gpio_base = (volatile unsigned long *)ioremap(0x0300b10c,1);
	if(!gpio_base){
		printk("gpio ioremap failed\n");
		return -EIO;
	}
	else
	{
		printk("gpio_base address is 0x%lx \n", gpio_base);
	        printk("gpio_base value is 0x%lx \n", readl(gpio_base));
        }

        GPHcon = (volatile unsigned long *) ioremap(0x0300b0fc,0x100);
	if(!GPHcon){
		printk("GPHcon ioremap failed\n");
		return -EIO;
	}
	else
	{
		printk("GPHcon base address is 0x%lx \n", GPHcon);
		printk("GPHcon value is 0x%lx \n",readl(GPHcon));
	}
	
	GPHdat = GPHcon + 2;
        printk("---------------GPHdat write test-------\n");
        printk("GPHdat value write is 0xf0,and read value is 0x%lx \n",readl(GPHdat));

   	printk(" GPHdat-1 address is 0x%lx \n",GPHdat-1);
	printk(" GPHdat-2 address is 0x%lx \n",GPHdat-2);
	printk(" GPHdat-3 address is 0x%lx \n",GPHdat-3);
	printk(" GPHdat-4 address is 0x%lx \n",GPHdat-4);
	printk(" GPHdat   address is 0x%lx \n",GPHdat  );






	/* 
 	printk("---------------GPHcon test-------------\n");


        GPHdat = (volatile unsigned long *) ioremap(0x0300b10c,1);
	if(!GPHcon){
		printk("GPHcon ioremap failed\n");
		return -EIO;
	}
	else
	{
		printk("GPHdat base address is 0x%lx \n", GPHdat);
		printk("GPHdat value is 0x%lx \n",readl(GPHdat));
	}
        printk("---------------GPHdat test-------------\n");
        writel(0x17777777,GPHcon);
	writel(0x80,GPHdat);
        printk("---------------GPHcon write test-------\n");
        printk("GPHcon value write is 0x17777777,and read value is 0x%lx \n",readl(GPHcon));
	printk("---------------GPHdat write test-------\n");
        printk("GPHdat value write is 0xf0,and read value is 0x%lx \n",readl(GPHdat));
*/

/*
	unsigned long PH_CFG0,PH_CFG1,PH_CFG2,PH_CFG3,PH_DAT,PH_DRV0,PH_DRV1,PH_PUL0,PH_PUL1;
	PH_CFG0 = readl(gpio_base + 0x0fc);
        PH_CFG1 = readl(gpio_base + 0x100);
        PH_CFG2 = readl(gpio_base + 0x104);
        PH_CFG3 = readl(gpio_base + 0x108);
        PH_DAT  = readl(gpio_base + 0x10c);
        PH_DRV0 = readl(gpio_base + 0x110);
        PH_DRV1 = readl(gpio_base + 0x114);
        PH_PUL0 = readl(gpio_base + 0x118);
        PH_PUL1 = readl(gpio_base + 0x11c);

	printk("PH_CFG0 value is: %lx\n", PH_CFG0);
	printk("PH_CFG1 value is: %lx\n", PH_CFG1);
	printk("PH_CFG2 value is: %lx\n", PH_CFG2);
	printk("PH_CFG3 value is: %lx\n", PH_CFG3);
	printk("PH_DAT  value is: %lx\n", PH_DAT );
	printk("PH_DRV0 value is: %lx\n", PH_DRV0);
	printk("PH_DRV1 value is: %lx\n", PH_DRV1);
	printk("PH_PUL0 value is: %lx\n", PH_PUL0);
	printk("PH_PUL1 value is: %lx\n", PH_PUL1);

 printk("===============================================\n");
 printk("===============================================\n");

	writel(0x17777777,(gpio_base + 0x0fc));
	writel(0x77777777,(gpio_base + 0x100));
	writel(0x00007777,(gpio_base + 0x104));
	writel(0x17777777,(gpio_base + 0x108));
	writel(0x00000007,(gpio_base + 0x10c));
	writel(0x55555555,(gpio_base + 0x110));
	writel(0x00000055,(gpio_base + 0x114));
	writel(0x00000040,(gpio_base + 0x118));
	writel(0x00000000,(gpio_base + 0x11c));
	
	
	
	PH_CFG0 = readl(gpio_base + 0x0fc);
        PH_CFG1 = readl(gpio_base + 0x100);
        PH_CFG2 = readl(gpio_base + 0x104);
        PH_CFG3 = readl(gpio_base + 0x108);
        PH_DAT  = readl(gpio_base + 0x10c);
        PH_DRV0 = readl(gpio_base + 0x110);
        PH_DRV1 = readl(gpio_base + 0x114);
        PH_PUL0 = readl(gpio_base + 0x118);
        PH_PUL1 = readl(gpio_base + 0x11c);

	printk("PH_CFG0 value is: %lx\n", PH_CFG0);
	printk("PH_CFG1 value is: %lx\n", PH_CFG1);
	printk("PH_CFG2 value is: %lx\n", PH_CFG2);
	printk("PH_CFG3 value is: %lx\n", PH_CFG3);
	printk("PH_DAT  value is: %lx\n", PH_DAT );
	printk("PH_DRV0 value is: %lx\n", PH_DRV0);
	printk("PH_DRV1 value is: %lx\n", PH_DRV1);
	printk("PH_PUL0 value is: %lx\n", PH_PUL0);
	printk("PH_PUL1 value is: %lx\n", PH_PUL1);

*/
	
/*
	unsigned long GPHcon_val,GPHcon_addr;
	printk("GPHcon_val is %lx\n",GPHcon_val);
	
	//GPHdat = GPHcon + 4;
	printk("GPHdat address is %lx\n", GPHdat);
	unsigned long GPHdat_val;
	GPHdat_val = readl(GPHdat);
	printk("GPHdat_val is %lx\n",GPHdat_val);

        //GPHcon_val &= 0x0fffffff;
        //GPHcon_val |= 0x1<<28;
       
	printk("GPHcon_val is %lx\n",GPHcon_val);

        //GPHdat_val &=~ (0x1<<7);
        GPHdat_val = 0x8;
        printk("Now I will write the GPHcon_val in to GPHcon address\n");
        printk("===============================================\n");

	printk("first_drv_open\n");      //打印，在内核中打印只能用printk()

	//GPHcon_addr = io_v2p(GPHcon);
	//GPHdat_addr = io_v2p(GPHdat);
*/	
	firstdrv_class= class_create(THIS_MODULE,"firstdrv");
	firstdrv_class_devs=device_create(firstdrv_class,NULL,MKDEV(major,0),NULL,"xyz");
    /*3 register_chrdev注册字符设备,并设置major=111*/
    /*如果设置major为0，表示由内核动态分配主设备号，函数的返回值是主设备号*/
	//register_chrdev (111, "first_drv", &first_drv_fops); //111:主设备号,”first_drv”:设备名
	major=register_chrdev(0,"first_drv",&first_drv_fops);
    /*register_chrdev作用:在VFS虚拟文件系统中找到字符设备，然后通过主设备号找到内核数组里对应的位置,最后将设备名字和fops结构体填进去*/
    return 0;
}

/*5 module_init修饰入口函数*/
module_init(first_drv_init);

/*6 写first_drv_exit出口函数*/
void first_drv_exit(void)
{
	device_unregister(firstdrv_class_devs);
	iounmap(GPHcon);
//	iounmap(GPHdat);
	class_destroy(firstdrv_class);
	unregister_chrdev (major, "first_drv");  //卸载驱动,只需要主设备号和设备名就行
	printk("GPHcon address is %lx \n", GPHcon);
	printk("GPHdat address is %lx \n", GPHdat);

}

/*7 module_exit修饰出口函数*/
module_exit(first_drv_exit);

/*8许可证声明, 描述内核模块的许可权限，如果不声明LICENSE，模块被加载时，将收到内核被污染 （kernel tainted）的警告。*/
MODULE_LICENSE( "GPL v2" );
