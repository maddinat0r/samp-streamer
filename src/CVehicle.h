#pragma once
#ifndef INC_CVEHICLE_H
#define INC_CVEHICLE_H


#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

using boost::atomic;
using boost::thread;
using boost::unordered_map;
using boost::unordered_set;
namespace this_thread = boost::this_thread;

namespace geo = boost::geometry;
typedef geo::model::point<float, 3, geo::cs::cartesian> point;


class CPlayer;
class CVehicle;


class CVehicleHandler
{
private: //variables
	unordered_map<uint32_t, CVehicle *> m_Vehicles;
	geo::index::rtree<boost::tuple<point, CVehicle *>, geo::index::rstar<16387> > m_Rtree;

	CVehicleHandler() {}
	~CVehicleHandler();
	
public: //functions


	void AddVehicle(CVehicle *veh, bool only_rtree = false);
	void RemoveVehicle(CVehicle *veh, bool only_rtree = false);
	CVehicle *FindVehicle(uint32_t vid); //for use in natives
	CVehicle *FindVehicleByRealID(uint32_t vehid); //for use in internal callbacks

	void StreamAll(CPlayer *player);


private: //singleton
	static CVehicleHandler *m_Instance;

public: //singleton
	inline static CVehicleHandler *Get()
	{
		if(m_Instance == nullptr)
			m_Instance = new CVehicleHandler;
		return m_Instance;
	}
	inline void Destroy()
	{
		delete this;
		m_Instance = nullptr;
	}
};


class CVehicle
{
	friend class CVehicleHandler;

private: //variables
	uint32_t m_Id;

	uint16_t m_ModelId;
	point m_Pos;
	float m_FacingAngle;
	int16_t m_Color[2];


private: //internal variables
	uint16_t m_VehicleId;
	unordered_set<uint32_t> m_StreamedFor;
	unordered_map<int8_t, uint32_t> m_SeatInfo;


private: //constructor / destructor
	CVehicle() :
		m_Id(0),

		m_ModelId(0),

		m_VehicleId(0)
	{ }
	~CVehicle() { }

private: //internal functions
	void CreateInternalVeh();
	void DestroyInternalVeh();

public: //functions
	static CVehicle *Create(uint16_t modelid, 
		float pos_x, float pos_y, float pos_z, float pos_a,
		int16_t color1, int16_t color2);

	void Destroy();

	void Update();

	inline uint32_t GetId() const
	{
		return m_Id;
	}
	inline point GetPos()
	{
		return m_Pos;
	}


public: //callback functions
	void OnPlayerEnter(CPlayer *player, int8_t seatid);
	void OnPlayerExit(CPlayer *player);
};


#endif // INC_CVEHICLE_H
