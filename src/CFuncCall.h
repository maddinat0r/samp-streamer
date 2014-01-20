#pragma once
#ifndef INC_CFUNCCALL_H
#define INC_CFUNCCALL_H


#include <queue>
#include <boost/move/move.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

using std::queue;
using boost::function;
using boost::mutex;


class CFuncCall
{
private:
	static CFuncCall *m_Instance;

	mutex m_QueueMtx;
	queue<function<void()>> m_FuncQueue;

	CFuncCall() {}
	~CFuncCall() {}

public:
	inline static CFuncCall *Get()
	{
		return m_Instance;
	}
	inline void Destroy()
	{
		delete m_Instance;
	}

	inline void QueueFunc(function<void()> &&func)
	{
		m_QueueMtx.lock();
		m_FuncQueue.push(boost::move(func));
		m_QueueMtx.unlock();
	}
	void ProcessFuncCalls();
};


#endif // INC_CFUNCCALL_H
