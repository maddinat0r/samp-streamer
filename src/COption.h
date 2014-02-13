#pragma once
#ifndef INC_COPTION_H
#define INC_COPTION_H


#include <cstdint>
#include <boost/atomic.hpp>

using boost::atomic;


enum class RangeCheckType : uint8_t
{
	INVALID,

	RADIUS,
	SIGHT
};


class COption
{
private: //variables
	atomic<RangeCheckType> m_CheckType;

	atomic<float> m_VehicleStreamDist;
	
private: //constructor / deconstructor
	COption() :
		m_CheckType(RangeCheckType::RADIUS),

		m_VehicleStreamDist(400.0f)
	{ }
	~COption() { }


public: //functions
	inline void SetRangeCheckType(RangeCheckType type)
	{
		m_CheckType = type;
	}
	inline RangeCheckType GetRangeCheckType() const
	{
		return m_CheckType;
	}

	inline void SetVehicleStreamDistance(float dist)
	{
		m_VehicleStreamDist = dist;
	}
	inline float GetVehicleStreamDistance() const
	{
		return m_VehicleStreamDist;
	}


private: //singleton
	static COption *m_Instance;

public: //singleton
	inline static COption *Get()
	{
		return m_Instance;
	}
	inline void Destroy()
	{
		delete this;
	}
};


#endif // INC_COPTION_H
