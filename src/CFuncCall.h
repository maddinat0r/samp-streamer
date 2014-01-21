#pragma once
#ifndef INC_CFUNCCALL_H
#define INC_CFUNCCALL_H


#include <boost/function.hpp>
#include <readerwriterqueue.h>

using boost::function;


class CFuncCall
{
private:
	static CFuncCall *m_Instance;

	moodycamel::ReaderWriterQueue<function<void()>> m_FuncQueue;

	CFuncCall() :
		m_FuncQueue(10000)
	{}
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
		m_FuncQueue.try_enqueue(func);
	}
	void ProcessFuncCalls();
};


#endif // INC_CFUNCCALL_H
