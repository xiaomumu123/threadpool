#include "ThreadPool.h"
#include <unistd.h> 
void* func1(void* arg)
{
	int num1 = 1;
	int i = 1;
	for(;i<10;i++)
	{
	num1=i*num1;
	}
	printf("The num1 Is %d\n",num1);
	printf("The i Is %d\n",i);
	printf("The Thread Is %lu\n",pthread_self());
	return NULL;
}

void* func2(void* arg)
{

	int num2 = 0;
	for(int i = 0;i<10;i++)
	{
			for(int j = 0;j<100;j++)
		{num2 = num2 + i+j;}
	}
	printf("The Answer2 Is %d\n",num2);
	return NULL;
}

int  main()
{
	ThreadPool* pThread =new ThreadPool(16,4);
	pThread->AddWorker(func1,NULL);
	//sleep(2);
	pThread->AddWorker(func2,NULL);
	//sleep(2);
	pThread->AddWorker(func1,NULL);
	//sleep(2);
	pThread->AddWorker(func2,NULL);
	//sleep(2);
	pThread->AddWorker(func1,NULL);
//sleep(2);
	pThread->AddWorker(func2,NULL);
//sleep(2);
	pThread->AddWorker(func1,NULL);
//sleep(2);
	pThread->AddWorker(func2,NULL);
//sleep(2);
	pThread->AddWorker(func1,NULL);
	sleep(2);

	return 1;
}
