// main.cc 
//	����ϵͳ�ں˵��������롣
//
//	����ֱ�ӵ����ڲ�����ϵͳ�������Լ򻯵��ԺͲ��ԡ�ʵ���ϣ���������ֻ���ʼ�����ݽṹ��������һ���û���������ӡ��¼��ʾ����
//
//	���������£�ֱ����������ҵ����Ҫ����ļ���
//
//  �÷�: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d ���´�ӡĳЩ������Ϣ���μ� utility.h��
//    -rs ���������λ�ã������ظ�������Yield
//    -z ��ӡ��Ȩ��Ϣ
//
//  USER_PROGRAM
//    -s �����û������Ե���ģʽִ��
//    -x �����û�����
//    -c ���Կ���̨
//
//  FILESYS
//    -f ����������̸�ʽ��
//    -cp ��UNIX���Ƶ�Nachos
//    -p ��ӡNachos�ļ���stdout
//    -r ���ļ�ϵͳ��ɾ��Nachos�ļ�
//    -l �г�NachosĿ¼������
//    -D ��ӡ�����ļ�ϵͳ������
//    -t ����Nachos�ļ�ϵͳ������
//
//  NETWORK
//    -n ��������ɿ���
//    -m ���ô˻���������ID��������Ҫ��
//    -o ����Nachos��������ļ򵥲���
//
//  ע��--��־�������ҵ֮ǰ�����ԡ�
//  һЩ��־��������ͣ�һЩ��system.cc�н��͡�
//
// ��Ȩ���У�c��1992-1993���������Ǵ�ѧ���»ᡣ
// ��������Ȩ�����йذ�Ȩ�������������ƺ�������������Ϣ������İ�Ȩ������

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

// ���ļ�ʹ�õ��ⲿ����

extern void ThreadTest(void), Copy(char* unixFile, char* nachosFile);
extern void Print(char* file), PerformanceTest(void);
extern void StartProcess(char* file), ConsoleTest(char* in, char* out);
extern void MailTest(int networkID);

//----------------------------------------------------------------------
// main
// 	��������ϵͳ�ںˡ�
//	
//	��������в���
//	��ʼ�����ݽṹ
//	����ѡ�أ����ò��Թ���
//
//	"argc"�������в�����������������������ƣ�--���磺"nachos -d +" -> argc = 3 
//	"argv"��һ���ַ������飬ÿ�������в�����Ӧһ���ַ���
//	���磺"nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------

int
main(int argc, char** argv)
{
	int argCount;			// ĳ������Ĳ�������

	DEBUG('t', "���� main");
	(void)Initialize(argc, argv);
#ifdef THREADS
	for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
		// lab 1 ��ʹ�÷���: 
    	//   ./nachos -q 2 -t [num_threads] -n [num_items] -e [type_of_error]
		argCount = 1;
		switch (argv[0][1]) {
		case 'q':
			testnum = atoi(argv[1]);
			argCount++;
			break;
		case 't':		// �߳�����
			threadnum = atoi(argv[1]);
			argCount++;
			break;
		case 'n':		// ��Ŀ����
			itemnum = atoi(argv[1]);
			argCount++;
			break;
		case 'e': 	// ��������
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
		if (!strcmp(*argv, "-z"))               // ��ӡ��Ȩ
			printf(copyright);
#ifdef USER_PROGRAM
		if (!strcmp(*argv, "-x")) {        	// �����û�����
			ASSERT(argc > 1);
			StartProcess(*(argv + 1));
			argCount = 2;
		}
		else if (!strcmp(*argv, "-c")) {      // ���Կ���̨
			if (argc == 1)
				ConsoleTest(NULL, NULL);
			else {
				ASSERT(argc > 2);
				ConsoleTest(*(argv + 1), *(argv + 2));
				argCount = 3;
			}
			interrupt->Halt();		// һ����������̨��Nachos����Զѭ���ȴ�����̨����
		}
#endif // USER_PROGRAM
#ifdef FILESYS
		if (!strcmp(*argv, "-cp")) { 		// ��UNIX���Ƶ�Nachos
			ASSERT(argc > 2);
			Copy(*(argv + 1), *(argv + 2));
			argCount = 3;
		}
		else if (!strcmp(*argv, "-p")) {	// ��ӡNachos�ļ�
			ASSERT(argc > 1);
			Print(*(argv + 1));
			argCount = 2;
		}
		else if (!strcmp(*argv, "-r")) {	// ���ļ�ϵͳ��ɾ��Nachos�ļ�
			ASSERT(argc > 1);
			fileSystem->Remove(*(argv + 1));
			argCount = 2;
		}
		else if (!strcmp(*argv, "-l")) {	// �г�NachosĿ¼
			fileSystem->List();
		}
		else if (!strcmp(*argv, "-D")) {	// ��ӡ�����ļ�ϵͳ
			fileSystem->Print();
		}
		else if (!strcmp(*argv, "-t")) {	// ���ܲ���
			PerformanceTest();
		}
#endif // FILESYS
#ifdef NETWORK
		if (!strcmp(*argv, "-o")) {
			ASSERT(argc > 1);
			Delay(2); 				// �ӳ�2��
			// ���û�ʱ��������һ��nachos
			MailTest(atoi(*(argv + 1)));
			argCount = 2;
		}
#endif // NETWORK
	}

	currentThread->Finish();	// ע��: ������̡�main�����أ������nachos�����˳�����������������һ�������������������߳���׼�������б��С�����ͨ��������main���߳��Ѿ�������л�����Щ�̣߳���ֹ�����ء�
	return(0);			// ���ᵽ��...
}
