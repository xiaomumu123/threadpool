#pragma once 
#include <stdint.h>
#include <vector>

   using std::vector;
  template <class T>class SimpleQueue
{
 public:    
    SimpleQueue(uint32_t nSize=4096)
    {
        SetMaxSize(nSize);
        m_nInPos=0;
        m_nOutPos=0;
    }
   ~SimpleQueue()
    {
    }
   bool Put(T &in)
   {
       if(m_nInPos-m_nOutPos>=m_nMaxSize)
     {
          return false;
        }
        uint32_t nPos=m_nInPos&m_nMaxSizeLess;
        m_vDataBuffer[nPos]=in;
        ++m_nInPos;
        return true;
    }
    bool Fetch(T& out)
    {
        if(m_nInPos-m_nOutPos==0)
       {
            return false;
        }
        uint32_t nPos=m_nOutPos&m_nMaxSizeLess;
        out=m_vDataBuffer[nPos];
       ++m_nOutPos;
       return true;
   }
    void SetMaxSize(uint32_t nMaxSize)
    {
        m_nMaxSize=1;
        while(nMaxSize>>=1)
       {
            m_nMaxSize<<=1;
        } 
        m_nMaxSizeLess=m_nMaxSize-1;
        m_vDataBuffer.resize(m_nMaxSize);
    }
    uint32_t MaxSize()
    {
        return m_nMaxSize;
    }
    uint32_t Size()
    {
        return m_nInPos-m_nOutPos;
    }   
    private:
   volatile uint32_t m_nInPos;   
   volatile uint32_t m_nOutPos;
    uint32_t m_nMaxSize;
    uint32_t m_nMaxSizeLess;
   vector<T> m_vDataBuffer;
};