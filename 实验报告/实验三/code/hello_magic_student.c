/* Hello from Kernel! */
#include <linux/module.h>
MODULE_LICENSE("GPL");
static char* name = "dys";
module_param(name, charp, 0644);
MODULE_PARM_DESC(name, "char* param\n");

static char* id = "4066";
module_param(id, charp, 0644);
MODULE_PARM_DESC(id, "char* param\n");

static int age = 20;
module_param(age, int, 0644);
MODULE_PARM_DESC(age, "int param\n");

void hello_student(char* name,char* id, int age) {
	printk(KERN_ALERT "My name is %s, student id is %s. I am %d years old.\n",name,id,age);
}


void my_magic_number(char* id,int age){
	int i;
	int magic_number=0;
	for (i=0;i<4;i++){
		magic_number+=id[i]-'0';
	}
	magic_number+=age;
	magic_number=magic_number%10;
	printk(KERN_ALERT "My magic number is %d\n",magic_number);
}
int __init hello_init(void) {
	hello_student(name,id,age);
	my_magic_number(id,age);
	return 0;
}
void __exit hello_exit(void) {
	printk(KERN_ALERT "Exit module.\n");
}
module_init(hello_init);
module_exit(hello_exit);
