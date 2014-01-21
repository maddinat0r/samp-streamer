#include "CFuncCall.h"


CFuncCall *CFuncCall::m_Instance = new CFuncCall;


void CFuncCall::ProcessFuncCalls()
{
	function<void()> func;
	while(m_FuncQueue.try_dequeue(func))
		func();
}