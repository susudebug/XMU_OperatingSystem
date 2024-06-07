# Build module kthread_stu_id
ifneq ($(KERNELRELEASE),)
	obj-m := kthread_stu_id.o kthread_bind_test.o \
           kthread_bind_cores.o proc_sys_info.o \
           kthread_sleep_monitor.o
else
	KERNELDIR ?=/root/kernel
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
.PHONY:clean
clean:
	-rm *.mod.c *.o *.order *.symvers *.ko