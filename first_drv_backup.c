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
//#include "../base/base.h"
//#include <../drivers/base/base.h>
#include "/media/roger/f_land/tina/lichee/linux-4.9/drivers/base/base.h"


int major;
static struct class *firstdrv_class; //make a device class 
static struct device *firstdrv_class_devs;// make a device for this class


volatile unsigned long *GPHcon=NULL;
volatile unsigned long *GPHdat=NULL;



/*1写出驱动程序first_drv_open first_drv_write */
/*inode结构表示具体的文件，file结构体用来追踪文件在运行时的状态信息。*/
static int first_drv_open(struct inode *inode, struct file  *file)
{
    //*GPHcon &= ~((0x11<<8) | (0x11<<10) | (0x11<<12));
    //*GPHcon |=  ((0x01<<8) | (0x01<<10) | (0x01<<12));
      *GPHcon &= 0x0fffffff;
      *GPHcon |= 0x1<<28;
    printk("first_drv_open\n");      //打印，在内核中打印只能用printk()
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
            *GPHdat &=~ (0x1<<7);

    }
    else
    {
	    *GPHdat |= (0x1<<7);
            //*GPHdat |= ((0x1<<4) | (0x1<<5) | (0x1<<6));

    }
    printk("first_drv_write\n");      //打印，在内核中打印只能用printk()
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
	GPHcon = ioremap(0x0300b0fc,16);
	if(!GPHcon){
		printk("GPHcon ioremap failed\n");
		return -EIO;
	}
	printk("GPHcon address is %lx\n", GPHcon);
	unsigned long GPHcon_val;
	GPHcon_val = readl(GPHcon);
	printk("GPHcon_val is %lx\n",GPHcon_val);
	
	//GPHdat = GPHcon + 4;
	GPHdat = ioremap(0x0300010c,16);
	if(!GPHcon){
		printk("GPHdat ioremap failed\n");
		return -EIO;
	}
	printk("GPHdat address is %lx\n", GPHdat);
	unsigned long GPHdat_val;
	GPHdat_val = readl(GPHdat);
	printk("GPHdat_val is %lx\n",GPHdat_val);

        GPHcon_val &= 0x0fffffff;
        GPHcon_val |= 0x1<<28;
       
	printk("GPHcon_val is %lx\n",GPHcon_val);

        //GPHdat_val &=~ (0x1<<7);
        GPHdat_val = 0x8;
	printk("GPHdat_val is %lx\n",GPHdat_val);

        printk("Now I will write the GPHcon_val in to GPHcon address\n");
        printk("===============================================\n");

	writel(GPHcon_val,GPHcon);
	writel(GPHdat_val,GPHdat);

	printk("first_drv_open\n");      //打印，在内核中打印只能用printk()
        unsigned long *GPHcon_addr,*GPHdat_addr;

	GPHcon_addr = io_v2p(GPHcon);
	GPHdat_addr = io_v2p(GPHdat);
        printk("GPHcon_addr is %lx\n",GPHcon_addr);
        printk("GPHcon_addr is %lx\n",GPHcon_addr);


	
	//GPHdat = GPHcon + 16;
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
	class_destroy(firstdrv_class);
	unregister_chrdev (major, "first_drv");  //卸载驱动,只需要主设备号和设备名就行
	printk("GPHcon address is %lx \n", GPHcon);
	printk("GPHdat address is %lx \n", GPHdat);

}

/*7 module_exit修饰出口函数*/
module_exit(first_drv_exit);

/*8许可证声明, 描述内核模块的许可权限，如果不声明LICENSE，模块被加载时，将收到内核被污染 （kernel tainted）的警告。*/
MODULE_LICENSE( "GPL v2" );
