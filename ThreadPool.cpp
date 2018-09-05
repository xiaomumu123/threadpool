

#include "ThreadPool.h"
 using namespace std;
 ThreadPool::ThreadPool(uint32_t nQueueSize,uint32_t nThreadNum):m_oQueue(nQueueSize),m_oLeaderID(NO_CURRENT_LEADER)
{
    m_nThreadNum=nThreadNum;
    m_bQueueClose=false;
    m_bPoolClose=false;
    m_nMaxTaskNum=m_oQueue.MaxSize();
    pthread_cond_init(&m_pQueueNotEmpty,NULL);
    pthread_cond_init(&m_pQueueNotFull,NULL);
   pthread_cond_init(&m_pQueueEmpty,NULL);
   pthread_cond_init(&m_pNoLeader,NULL);
   pthread_mutex_init(&m_pLeaderMutex,NULL);
    pthread_mutex_init(&m_pQueueHeadMutex,NULL);
   pthread_mutex_init(&m_pQueueTailMutex,NULL);
  pthread_attr_t attr;
     pthread_attr_init (&attr);
     pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
  m_vThreadID=new pthread_t[m_nThreadNum];
   for(size_t i=0;i<nThreadNum;++i)
    {
	
        pthread_create(&(m_vThreadID[i]),&attr,WorkProcess,this);

    }
}
 ThreadPool::~ThreadPool()
 {
   Destroy();
    pthread_cond_destroy(&m_pQueueNotEmpty);
        pthread_cond_destroy(&m_pQueueNotFull);
       pthread_cond_destroy(&m_pQueueEmpty);
   pthread_cond_destroy(&m_pNoLeader);
       pthread_mutex_destroy(&m_pQueueHeadMutex);
       pthread_mutex_destroy(&m_pQueueTailMutex);
  pthread_mutex_destroy(&m_pLeaderMutex);
}
 void ThreadPool::Destroy()
{
   if (m_bPoolClose) 
         return;
     //关闭队列，不在接受新的任务
     m_bQueueClose=1;
 
     pthread_mutex_lock(&m_pQueueTailMutex);
     while (m_oQueue.Size()!=0)
     {
         pthread_cond_wait(&(m_pQueueEmpty), &(m_pQueueTailMutex));
     }
     m_bPoolClose=1;
     pthread_mutex_unlock(&m_pQueueTailMutex);
     //唤醒所有线程，准备退出
     pthread_cond_broadcast(&m_pNoLeader);
     pthread_cond_broadcast(&m_pQueueNotEmpty);
 
     delete [] m_vThreadID;
 
 
 }
  bool ThreadPool::AddWorker(void *(*process)(void *arg),void* arg)
 {
	
     if(m_bQueueClose)
         return false;
     Job *pNewJob=new Job;
     pNewJob->arg=arg;
     pNewJob->process=process;

 
     pthread_mutex_lock(&m_pQueueHeadMutex);

     while(m_oQueue.Size()>=m_nMaxTaskNum&&!m_bQueueClose)
     {

         pthread_cond_wait(&m_pQueueNotFull, &m_pQueueHeadMutex);
			
     }
     if(m_bQueueClose)
     {
         delete pNewJob;
         pthread_mutex_unlock(&m_pQueueHeadMutex);
         return false;
     }
     m_oQueue.Put(pNewJob);

     pthread_mutex_unlock(&m_pQueueHeadMutex);
     pthread_cond_signal(&m_pQueueNotEmpty);

     return true;
 
 }
 
  void * ThreadPool::WorkProcess(void *arg)
 {

     ThreadPool *pThreadPool=(ThreadPool*)arg;
     pThreadPool->JoinFollower();

     while(true)
     {
         pthread_mutex_lock(&(pThreadPool->m_pQueueTailMutex));
         while(pThreadPool->m_oQueue.Size()==0&&!pThreadPool->m_bPoolClose)
         {
             pthread_cond_wait(&(pThreadPool->m_pQueueNotEmpty),&(pThreadPool->m_pQueueTailMutex));

         }
         pthread_mutex_unlock(&(pThreadPool->m_pQueueTailMutex));
         if(pThreadPool->m_bPoolClose)
         {
            pthread_exit(NULL);
        }
        Job *pJob;

        pThreadPool->m_oQueue.Fetch(pJob);
		
        if(pThreadPool->m_bQueueClose&&pThreadPool->m_oQueue.Size()==0)
        {
            pthread_cond_signal(&(pThreadPool->m_pQueueEmpty));
        }
        pthread_cond_signal(&(pThreadPool->m_pQueueNotFull));

        pThreadPool->PromoteNewLeader();

        pJob->process(pJob->arg);
        delete pJob;    

        pThreadPool->JoinFollower();
    }
    return NULL;
}
 void ThreadPool::JoinFollower()
{
    pthread_mutex_lock(&m_pLeaderMutex);
    while(m_oLeaderID!=NO_CURRENT_LEADER&&!m_bPoolClose)
    {     
		printf("thread block\n");
		
        pthread_cond_wait(&m_pNoLeader,&m_pLeaderMutex);
		printf("the win thread is %lu\n",pthread_self());

    }
    if(m_bPoolClose)
    {
        pthread_mutex_unlock(&m_pLeaderMutex);
        pthread_exit(NULL);
    }
    m_oLeaderID=pthread_self();

    pthread_mutex_unlock(&m_pLeaderMutex);  

}
 void ThreadPool::PromoteNewLeader()
{
    m_oLeaderID=NO_CURRENT_LEADER;
    pthread_cond_signal(&m_pNoLeader);
}
