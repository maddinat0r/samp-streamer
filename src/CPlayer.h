#pragma once
#ifndef INC_CPLAYER_H
#define INC_CPLAYER_H


#include <set>
#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_map.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>

using std::set;
using boost::atomic;
using boost::thread;
using boost::unordered_map;
namespace this_thread = boost::this_thread;

typedef boost::geometry::model::point<float, 3, boost::geometry::cs::cartesian> point;

class CVehicle;


class CPlayer
{
private: //variables
	atomic<bool> m_DataLock;


	const uint16_t m_Id;

	float m_CameraPos[3];
	float m_CameraDir[3];
	uint8_t m_CameraMode;

	point m_Pos;
	float m_Velocity[3];

	uint16_t m_InVehicle;
	
public: //variables
	//vehicle streamer data
	set<CVehicle *> StreamedVehicles;
	CVehicle *OccupiedVehicle;

private: //constructor / deconstructor
	CPlayer(uint16_t playerid) :
		m_DataLock(false),
		
		m_Id(playerid),

		m_CameraMode(0),

		m_InVehicle(0),

		OccupiedVehicle(nullptr)
	{ }
	~CPlayer() { }

public: //functions
	inline static CPlayer *Create(uint16_t playerid)
	{
		return new CPlayer(playerid);
	}
	inline void Destroy()
	{
		delete this;
	}

	inline uint16_t GetId() const
	{
		return m_Id;
	}
	inline point &GetPos()
	{
		return m_Pos;
	}

	void Update();
	bool ShouldStream(float x, float y, float z, float stream_dist);
};


class CPlayerHandler
{
private: //variables
	thread *m_Thread;
	atomic<bool> m_ThreadRunning;

	unordered_map<uint16_t, CPlayer *> m_Players;

private: //internal functions
	void ThreadFunc();
	
private: //constructor / deconstructor
	CPlayerHandler();
	~CPlayerHandler();


public: //functions
	inline void AddPlayer(uint16_t playerid, CPlayer *player)
	{
		m_Players.insert(unordered_map<uint16_t, CPlayer *>::value_type(playerid, player));
	}
	inline void RemovePlayer(uint16_t playerid)
	{
		m_Players.quick_erase(m_Players.find(playerid));
	}
	CPlayer *FindPlayer(uint16_t playerid);

	inline void Update(uint16_t playerid)
	{
		m_Players.at(playerid)->Update();
	}
	

private: //singleton
	static CPlayerHandler *m_Instance;

public: //singleton
	inline static CPlayerHandler *Get()
	{
		if (m_Instance == nullptr)
			m_Instance = new CPlayerHandler;
		return m_Instance;
	}
	inline void Destroy()
	{
		m_Instance = nullptr;
		delete this;
	}
};


#endif // INC_CPLAYER_H
