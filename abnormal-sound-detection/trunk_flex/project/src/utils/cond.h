#ifndef __COND_H__
#define __COND_H__

#include <stdio.h>
#ifdef WIN32
#include <thread>
#include <condition_variable> 
#else
#include <pthread.h>
#endif

class CThread
{
public:
	CThread()
	{
#if WIN32

#else

#endif
	}
	~CThread()
	{
#ifdef WIN32

#else

#endif
	}

	void create(void *(*start) (void *), void *args)
	{
#ifdef WIN32
		m_thread = std::thread(start, args);
#else
		pthread_create(&m_thread, NULL, start, args);
#endif
	}

	void join()
	{
#if WIN32
		m_thread.join();
#else
		pthread_join(m_thread, NULL);
#endif
	}

private:
#ifdef WIN32
	std::thread m_thread;
#else
	pthread_t m_thread;
#endif
};

class CCond
{
public:
	CCond() 
#ifdef WIN32
		: m_lock(std::unique_lock<std::mutex>(m_mutex))
#else
		: m_cond(PTHREAD_COND_INITIALIZER)
		, m_lock(PTHREAD_MUTEX_INITIALIZER)
#endif
	{
#ifdef WIN32
		m_lock.unlock();
#else
		
#endif
	}

	~CCond() 
	{ 
#ifdef WIN32
		m_lock.unlock();
#else
		pthread_mutex_unlock(&m_lock);
		pthread_cond_destroy(&m_cond);
		pthread_mutex_destroy(&m_lock);
#endif
	}
		
	void lock()
	{
#ifdef WIN32
		m_lock.lock();
#else
		pthread_mutex_lock(&m_lock);
#endif
	}

	void unlock()
	{
#ifdef WIN32
		m_lock.unlock();
#else
		pthread_mutex_unlock(&m_lock);
#endif
	}

	void wait()
	{
#ifdef WIN32
		m_cond.wait(m_lock);
#else
		pthread_cond_wait(&m_cond, &m_lock);
#endif
	}

	void wait_s()
	{
#ifdef WIN32
		m_lock.lock();
		m_cond.wait(m_lock);
		m_lock.unlock();
#else
		pthread_mutex_lock(&m_lock);
		pthread_cond_wait(&m_cond, &m_lock);
		pthread_mutex_unlock(&m_lock);
#endif
	}

	void signal()
	{
#ifdef WIN32
		m_cond.notify_one();
#else
		pthread_cond_signal(&m_cond);
#endif
	}

	void signal_s()
	{
#ifdef WIN32
		m_lock.lock();
		m_cond.notify_one();
		m_lock.unlock();
#else
		pthread_mutex_lock(&m_lock);
		pthread_cond_signal(&m_cond);
		pthread_mutex_unlock(&m_lock);
#endif
	}

private:
#ifdef WIN32
	std::condition_variable m_cond;
	std::mutex m_mutex;
	std::unique_lock<std::mutex> m_lock;
#else
	pthread_cond_t m_cond;
	pthread_mutex_t m_lock;
#endif
};

#endif /*__COND_H__*/