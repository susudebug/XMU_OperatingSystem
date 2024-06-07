#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/kthread.h>

static struct task_struct *cycle_print_kthread = NULL;
char current_uptime[8] = "";
int total_memory;
int free_memory;
int occupy_memory;
char str[10];

static void sleep_milli_sec(int n_milli_sec)
{
    long timeout = (n_milli_sec)*HZ / 1000;
    do
    {
        while (timeout > 0)
        {
            timeout = schedule_timeout(timeout);
        }
    } while (0);
}

static int load_kernel_info(void)
{
    struct file *fp_upt, *fp_meminfo;
    loff_t pos = 0;
    ssize_t bytes;
    char buf_uptime[8], buf_meminfo[128];
    fp_upt = filp_open("/proc/uptime", O_RDONLY, 0);
    if (IS_ERR(fp_upt))
    {
        printk("open proc file error\n");
        return -1;
    }
    kernel_read(fp_upt, buf_uptime, sizeof(buf_uptime), &pos);
    strncpy(current_uptime, buf_uptime, 7);
    current_uptime[7] = '\0';
    filp_close(fp_upt, NULL);

    pos = 0;
    fp_meminfo = filp_open("/proc/meminfo", O_RDONLY, 0);
    if (IS_ERR(fp_meminfo))
    {
        printk("open proc file error\n");
        return -1;
    }
    bytes = kernel_read(fp_meminfo, buf_meminfo, sizeof(buf_meminfo), &pos);
    buf_meminfo[bytes] = '\0';

    sscanf(buf_meminfo, "MemTotal: %d kB\nMemFree: %d kB", &total_memory, &free_memory);

    filp_close(fp_meminfo, NULL);

    occupy_memory = total_memory - free_memory;
    total_memory /= 1024;
    free_memory /= 1024;
    occupy_memory /= 1024;

    return 0;
}
static int print_fun(void *data)
{
    while (!kthread_should_stop())
    {
        if (load_kernel_info() != 0)
        {
            printk("error occurs when loading kernel info!\n");
            return -1;
        }
        printk("current uptime: %ss\n", current_uptime);
        printk("total memory: %d MB\n", total_memory);
        printk("free memory: %d MB\n", free_memory);
        printk("occupy memory: %d MB\n", occupy_memory);
        sleep_milli_sec(3000);
    }
    return 0;
}
static int __init proc_parser_init(void)
{
    cycle_print_kthread = kthread_run(print_fun, NULL, "cycle_print_kthread");
    printk("cycle_print_kthread started\n");
    printk("22920212204181\n");
    return 0;
}
static void __exit proc_parser_exit(void)
{
    if (cycle_print_kthread)
    {
        kthread_stop(cycle_print_kthread);
    }
    printk("cycle_print_kthread stopped\n");
}
module_init(proc_parser_init);
module_exit(proc_parser_exit);
MODULE_LICENSE("GPL");
