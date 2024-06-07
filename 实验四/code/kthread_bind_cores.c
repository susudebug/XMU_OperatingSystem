#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

#define NUM_THREADS 4

static struct task_struct *kthreads[NUM_THREADS];

static int MyPrintk(void *data) {
    int cpu;

    while (!kthread_should_stop()) {
        cpu = get_cpu();
        put_cpu();

        printk("kthread %s is running on cpu %d\n", current->comm, cpu);

        msleep(2000);
    }

    return 0;
}

static int __init init_kthread(void) {
    int i;
    char thread_name[20];

    for (i = 0; i < NUM_THREADS; ++i) {
        snprintf(thread_name, sizeof(thread_name), "bind_kthread_%d", i);
        kthreads[i] = kthread_create(MyPrintk, NULL, thread_name);
        if (IS_ERR(kthreads[i])) {
            printk("Failed to create kthread %d\n", i);
            return PTR_ERR(kthreads[i]);
        }
        
        kthread_bind(kthreads[i], i);  // Bind thread to CPU core i
        wake_up_process(kthreads[i]);

        printk("Created kthread %s and bound to CPU %d\n", thread_name, i);
    }

    return 0;
}

static void __exit exit_kthread(void) {
    int i;

    for (i = 0; i < NUM_THREADS; ++i) {
        if (kthreads[i]) {
            kthread_stop(kthreads[i]);
            printk("Stopped kthread %d\n", i);
        }
    }
}

module_init(init_kthread);
module_exit(exit_kthread);
MODULE_LICENSE("GPL");
