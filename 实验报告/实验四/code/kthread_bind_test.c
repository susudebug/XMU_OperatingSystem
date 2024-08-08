#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

static struct task_struct *bind_kthread = NULL;

static int MyPrintk(void *data) {
	//int target_new_cpu = 2;
	while (!kthread_should_stop()) {
		int cpu = get_cpu();
		put_cpu();
		printk("kthread %s is running on cpu %d\n", current->comm, cpu);
		msleep(2000);
		//���Ը����󶨵�CPU
//		kthread_bind(bind_kthread, target_new_cpu);
	}
	return 0;
}

static int __init init_kthread(void) {
	int target_cpu = 1; // ���ð󶨵� CPU
	long state_before_bind, state_after_bind, state_after_wake;
	int exceeding_target_cpu=10;//ͨ�������֪��ǰCPU����Ϊ4

	bind_kthread = kthread_create(MyPrintk, NULL, "bind_kthread");
	if (IS_ERR(bind_kthread)) {
		printk("Failed to create kthread\n");
		return PTR_ERR(bind_kthread);
	}

	//�鿴��ǰ����߳�״̬
	state_before_bind = bind_kthread->state;
	printk("State before bind: %ld\n", state_before_bind);

	kthread_bind(bind_kthread, exceeding_target_cpu);//������CPUID,���԰󶨳����ں�����CPUID
	state_after_bind = bind_kthread->state;
	printk("State after bind: %ld\n", state_after_bind);

	wake_up_process(bind_kthread);
	state_after_wake = bind_kthread->state;
	printk("State after wake: %ld\n", state_after_wake);
	printk("kthread bind_kthread start on CPU %d\n", task_cpu(bind_kthread));


	return 0;
}

static void __exit exit_kthread(void) {
	if (bind_kthread) {
		kthread_stop(bind_kthread);
		printk("kill kthread bind_kthread\n");
	}
}

module_init(init_kthread);
module_exit(exit_kthread);

MODULE_LICENSE("GPL");
