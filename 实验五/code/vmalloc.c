#include <linux/module.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");

unsigned char *vmallocmem1;
unsigned char *vmallocmem2;
unsigned char *vmallocmem3;
unsigned char *vmallocmem4;

static int __init mem_module_init(void)
{
    printk(KERN_INFO "Start vmalloc!\n");

    vmallocmem1 = vmalloc(8192);
    if (!vmallocmem1) {
        printk(KERN_ERR "Failed to allocate vmallocmem1!\n");
    } else {
        printk(KERN_INFO "vmallocmem1 addr = %p\n", vmallocmem1);
    }

    vmallocmem2 = vmalloc(1048576);
    if (!vmallocmem2) {
        printk(KERN_ERR "Failed to allocate vmallocmem2!\n");
    } else {
        printk(KERN_INFO "vmallocmem2 addr = %p\n", vmallocmem2);
    }

    vmallocmem3 = vmalloc(67108864);
    if (!vmallocmem3) {
        printk(KERN_ERR "Failed to allocate vmallocmem3!\n");
    } else {
        printk(KERN_INFO "vmallocmem3 addr = %p\n", vmallocmem3);
    }

    // Allocate slightly more than the kmalloc max allowed size
    vmallocmem4 = vmalloc(4194303+10);
    if (!vmallocmem4) {
        printk(KERN_ERR "Failed to allocate vmallocmem4!\n");
    } else {
        printk(KERN_INFO "vmallocmem4 addr = %p\n", vmallocmem4);
    }

    return 0;
}

static void __exit mem_module_exit(void)
{
    if (vmallocmem1) {
        vfree(vmallocmem1);
    }
    if (vmallocmem2) {
        vfree(vmallocmem2);
    }
    if (vmallocmem3) {
        vfree(vmallocmem3);
    }
    if (vmallocmem4) {
        vfree(vmallocmem4);
    }

    printk(KERN_INFO "Exit vmalloc!\n");
}

module_init(mem_module_init);
module_exit(mem_module_exit);
