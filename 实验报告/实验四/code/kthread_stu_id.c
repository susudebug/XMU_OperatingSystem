#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
#define BUF_SIZE 20

static char* id;
module_param(id, charp, 0644);
MODULE_PARM_DESC(id, "Student ID as a string");

static struct task_struct *stuidThread = NULL;

static int print_id(void *data) {
    int i = 0;
    char* stu_id=(char *)data;
    int len = strlen(stu_id);
	printk(KERN_INFO "Index %d of student ID: %c\n",i, stu_id[i]);
    while (!kthread_should_stop()) {
        if (i < len) {
            printk(KERN_INFO "Index %d of student ID: %c\n",i, stu_id[i]);
            i++;
            msleep(3000);
        } else {
            printk(KERN_INFO "All digits of student ID have been printed\n");
            msleep(5000);
        }
    }
    return 0;
}

static int __init kthread_stu_id_init(void) {
    printk(KERN_INFO "Creating kthread stuidThread.\n");
    stuidThread = kthread_run(print_id, id, "stuidThread");
    return 0;
}

static void __exit kthread_stu_id_exit(void) {
    printk(KERN_INFO "Stopping kthread stuidThread.\n");
    if (stuidThread) {
        kthread_stop(stuidThread);
    }
}

module_init(kthread_stu_id_init);
module_exit(kthread_stu_id_exit);
