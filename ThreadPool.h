#pragma once
 #include <pthread.h>
#include <stdio.h>
#include "SimpleQueue.h"
	const int NO_CURRENT_LEADER=0;
	struct Job
{
    void *arg;
    void *(*process)(void *arg);
};


	class ThreadPool
{
	public:
   ThreadPool(uint32_t nQueueSize=4096,uint32_t nThreadNum=8);
    ~ThreadPool();
   bool AddWorker(void *(*process)(void *arg),void* arg);
   void Destroy();
	private:
    static void * WorkProcess(void *arg);
   void JoinFollower();
   void PromoteNewLeader();
   SimpleQueue<Job*> m_oQueue;
   pthread_cond_t  m_pQueueNotEmpty;
   pthread_cond_t  m_pQueueNotFull;
   pthread_cond_t  m_pQueueEmpty;
   pthread_cond_t  m_pNoLeader;
   pthread_mutex_t m_pLeaderMutex;
    pthread_mutex_t m_pQueueHeadMutex;
    pthread_mutex_t m_pQueueTailMutex;
   bool m_bQueueClose;
   bool m_bPoolClose;
    pthread_t *m_vThreadID;
    pthread_t m_oLeaderID;
    uint32_t m_nThreadNum;
   uint32_t m_nMaxTaskNum;
};