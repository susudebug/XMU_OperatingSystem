#include <linux/module.h>
#include <linux/slab.h>
MODULE_LICENSE("GPL");

/*TODO: allocate 1KB for kmallocmem1*/
unsigned char *kmallocmem1;
/*TODO: allocate 8KB for kmallocmem2*/
unsigned char *kmallocmem2;
/*TODO: allocate max-allowed size of memory for kmallocmem3*/
unsigned char *kmallocmem3;
/*TODO: allocate memory lager than max-allowed size for kmallocmem4*/
unsigned char *kmallocmem4;
static int __init mem_module_init(void)
{
    printk(KERN_INFO "Start kmalloc!\n");

    kmallocmem1 = kmalloc(1024, GFP_KERNEL);
    if (!kmallocmem1) {
        printk(KERN_ERR "Failed to allocate kmallocmem1!\n");
    } else {
        printk(KERN_INFO "kmallocmem1 addr = %p\n", kmallocmem1);
    }

    kmallocmem2 = kmalloc(8192, GFP_KERNEL);
    if (!kmallocmem2) {
        printk(KERN_ERR "Failed to allocate kmallocmem2!\n");
    } else {
        printk(KERN_INFO "kmallocmem2 addr = %p\n", kmallocmem2);
    }

    size_t max_alloc_size = (1 << (sizeof(void *) * 8 - 1)) - 1; // Initial guess
    while (max_alloc_size > 0) {
        kmallocmem3 = kmalloc(max_alloc_size, GFP_KERNEL);
        if (kmallocmem3) {
            printk(KERN_INFO "kmallocmem3 max alloc size = %zu\n", max_alloc_size);
            printk(KERN_INFO "kmallocmem3 addr = %p\n", kmallocmem3);
            break;
        }
        max_alloc_size >>= 1; // Halve the size
    }
    if (!kmallocmem3) {
        printk(KERN_ERR "Failed to allocate kmallocmem3!\n");
    }

    kmallocmem4 = kmalloc(max_alloc_size + 10, GFP_KERNEL);
    if (!kmallocmem4) {
        printk(KERN_ERR "Failed to allocate kmallocmem4!\n");
    } else {
        printk(KERN_INFO "kmallocmem4 addr = %p\n", kmallocmem4);
    }
return 0;
}
static void __exit mem_module_exit(void)
{
    if (kmallocmem1) {
        kfree(kmallocmem1);
    }
    if (kmallocmem2) {
        kfree(kmallocmem2);
    }
    if (kmallocmem3) {
        kfree(kmallocmem3);
    }
    if (kmallocmem4) {
        kfree(kmallocmem4);
    }
    printk(KERN_INFO "Exit kmalloc!\n");
}
module_init(mem_module_init);
module_exit(mem_module_exit);