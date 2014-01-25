#pragma once
#ifndef INC_CFUNCCALL_H
#define INC_CFUNCCALL_H


#include <boost/function.hpp>
#include <readerwriterqueue.h>

using boost::function;


class CFuncCall
{
private: //variables
	moodycamel::ReaderWriterQueue<function<void()>> m_FuncQueue;

private: //constructor / deconstructor
	CFuncCall() :
		m_FuncQueue(100000)
	{}
	~CFuncCall() {}

public: //functions
	inline void QueueFunc(function<void()> &&func)
	{
		m_FuncQueue.try_enqueue(func);
	}
	void ProcessFuncCalls();


private: //singleton
	static CFuncCall *m_Instance;

public: //singleton
	inline static CFuncCall *Get()
	{
		return m_Instance;
	}
	inline void Destroy()
	{
		delete m_Instance;
	}
};


#endif // INC_CFUNCCALL_H
