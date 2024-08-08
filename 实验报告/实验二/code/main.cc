// main.cc 
//	操作系统内核的引导代码。
//
//	允许直接调用内部操作系统函数，以简化调试和测试。实际上，引导代码只需初始化数据结构，并启动一个用户程序来打印登录提示符。
//
//	大多数情况下，直到后续的作业才需要这个文件。
//
//  用法: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d 导致打印某些调试消息（参见 utility.h）
//    -rs 导致在随机位置（但可重复）发生Yield
//    -z 打印版权信息
//
//  USER_PROGRAM
//    -s 导致用户程序以单步模式执行
//    -x 运行用户程序
//    -c 测试控制台
//
//  FILESYS
//    -f 导致物理磁盘格式化
//    -cp 从UNIX复制到Nachos
//    -p 打印Nachos文件到stdout
//    -r 从文件系统中删除Nachos文件
//    -l 列出Nachos目录的内容
//    -D 打印整个文件系统的内容
//    -t 测试Nachos文件系统的性能
//
//  NETWORK
//    -n 设置网络可靠性
//    -m 设置此机器的主机ID（网络需要）
//    -o 运行Nachos网络软件的简单测试
//
//  注意--标志在相关作业之前被忽略。
//  一些标志在这里解释；一些在system.cc中解释。
//
// 版权所有（c）1992-1993加利福尼亚大学理事会。
// 保留所有权利。有关版权声明、责任限制和免责声明的信息，请参阅版权声明。

#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include "system.h"

#define THREADS

#ifdef THREADS
extern int testnum;
extern int threadnum;
extern int itemnum;
extern int errorType;
#endif

// 此文件使用的外部函数

extern void ThreadTest(void), Copy(char* unixFile, char* nachosFile);
extern void Print(char* file), PerformanceTest(void);
extern void StartProcess(char* file), ConsoleTest(char* in, char* out);
extern void MailTest(int networkID);

//----------------------------------------------------------------------
// main
// 	引导操作系统内核。
//	
//	检查命令行参数
//	初始化数据结构
//	（可选地）调用测试过程
//
//	"argc"是命令行参数的数量（包括命令的名称）--例如："nachos -d +" -> argc = 3 
//	"argv"是一个字符串数组，每个命令行参数对应一个字符串
//	例如："nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------

int
main(int argc, char** argv)
{
	int argCount;			// 某个命令的参数数量

	DEBUG('t', "进入 main");
	(void)Initialize(argc, argv);
#ifdef THREADS
	for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
		// lab 1 的使用方法: 
    	//   ./nachos -q 2 -t [num_threads] -n [num_items] -e [type_of_error]
		argCount = 1;
		switch (argv[0][1]) {
		case 'q':
			testnum = atoi(argv[1]);
			argCount++;
			break;
		case 't':		// 线程数量
			threadnum = atoi(argv[1]);
			argCount++;
			break;
		case 'n':		// 项目数量
			itemnum = atoi(argv[1]);
			argCount++;
			break;
		case 'e': 	// 错误类型
			errorType = atoi(argv[1]);
			argCount++;
			break;
		default:
			testnum = 1;
			break;
		}
	}

	ThreadTest();
#endif

	for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
		argCount = 1;
		if (!strcmp(*argv, "-z"))               // 打印版权
			printf(copyright);
#ifdef USER_PROGRAM
		if (!strcmp(*argv, "-x")) {        	// 运行用户程序
			ASSERT(argc > 1);
			StartProcess(*(argv + 1));
			argCount = 2;
		}
		else if (!strcmp(*argv, "-c")) {      // 测试控制台
			if (argc == 1)
				ConsoleTest(NULL, NULL);
			else {
				ASSERT(argc > 2);
				ConsoleTest(*(argv + 1), *(argv + 2));
				argCount = 3;
			}
			interrupt->Halt();		// 一旦启动控制台，Nachos将永远循环等待控制台输入
		}
#endif // USER_PROGRAM
#ifdef FILESYS
		if (!strcmp(*argv, "-cp")) { 		// 从UNIX复制到Nachos
			ASSERT(argc > 2);
			Copy(*(argv + 1), *(argv + 2));
			argCount = 3;
		}
		else if (!strcmp(*argv, "-p")) {	// 打印Nachos文件
			ASSERT(argc > 1);
			Print(*(argv + 1));
			argCount = 2;
		}
		else if (!strcmp(*argv, "-r")) {	// 从文件系统中删除Nachos文件
			ASSERT(argc > 1);
			fileSystem->Remove(*(argv + 1));
			argCount = 2;
		}
		else if (!strcmp(*argv, "-l")) {	// 列出Nachos目录
			fileSystem->List();
		}
		else if (!strcmp(*argv, "-D")) {	// 打印整个文件系统
			fileSystem->Print();
		}
		else if (!strcmp(*argv, "-t")) {	// 性能测试
			PerformanceTest();
		}
#endif // FILESYS
#ifdef NETWORK
		if (!strcmp(*argv, "-o")) {
			ASSERT(argc > 1);
			Delay(2); 				// 延迟2秒
			// 给用户时间启动另一个nachos
			MailTest(atoi(*(argv + 1)));
			argCount = 2;
		}
#endif // NETWORK
	}

	currentThread->Finish();	// 注意: 如果过程“main”返回，则程序“nachos”将退出（与其他正常程序一样）。但可能有其他线程在准备就绪列表中。我们通过声明“main”线程已经完成来切换到这些线程，防止它返回。
	return(0);			// 不会到达...
}
