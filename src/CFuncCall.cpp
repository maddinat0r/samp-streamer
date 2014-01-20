#include "CFuncCall.h"

#include <boost/thread/lock_guard.hpp>


CFuncCall *CFuncCall::m_Instance = new CFuncCall;


void CFuncCall::ProcessFuncCalls()
{
	boost::lock_guard<mutex> lock(m_QueueMtx);
	function<void()> func;

	while(!m_FuncQueue.empty())
	{
		m_FuncQueue.front()();
		m_FuncQueue.pop();
	}
}