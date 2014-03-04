#pragma once
#ifndef INC_CVEHICLE_H
#define INC_CVEHICLE_H


#include <string>

#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/tuple/tuple.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

using std::string;

using boost::atomic;
using boost::thread;
using boost::mutex;
using boost::unordered_map;
using boost::unordered_set;
using boost::tuple;
namespace this_thread = boost::this_thread;

namespace geo = boost::geometry;
typedef geo::model::point<float, 3, geo::cs::cartesian> point;


class CPlayer;
class CVehicle;


class CVehicleHandler
{
private: //variables
	unordered_map<uint32_t, CVehicle *> m_Vehicles;
	mutex m_RtreeMtx;
	geo::index::rtree<tuple<point, CVehicle *>, geo::index::rstar<16387> > m_Rtree;

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
	point
		m_Pos,
		m_SpawnPos;
	float
		m_FacingAngle,
		m_SpawnFacingAngle;
	float m_Velocity[3];
	uint8_t m_Color[2];
	uint8_t m_Paintjob;
	float m_Health;
	bool m_ParamsEx[7];
	int m_DamageStatus[4];
	int m_VirtualWorld;
	uint8_t m_InteriorId;
	string m_NumberPlate;


private: //internal variables
	tuple<point, CVehicle *> m_LastRtreeValue;

	uint16_t m_VehicleId;
	unordered_set<CPlayer *> m_StreamedFor;
	unordered_map<int8_t, CPlayer *> m_SeatInfo;


private: //constructor / destructor
	CVehicle() :
		m_Id(0),

		m_Pos(0.0f, 0.0f, 0.0f),
		m_ModelId(0),
		m_FacingAngle(0.0f),
		m_Paintjob(3),
		m_Health(1000.0f),
		m_VirtualWorld(0),
		m_InteriorId(0),
		m_NumberPlate("XYZSR998"),

		m_VehicleId(0)
	{
		for (size_t i = 0; i < 3; ++i)
			m_Velocity[i] = 0.0f;

		for (size_t i = 0; i < 2; ++i)
			m_Color[i] = 0;

		for (size_t i = 0; i < 7; ++i)
			m_ParamsEx[i] = false;

		for (size_t i = 0; i < 4; ++i)
			m_DamageStatus[i] = 0;
	}
	~CVehicle() { }

private: //internal functions
	void CreateInternalVeh();
	void DestroyInternalVeh();

public: //functions
	static CVehicle *Create(uint16_t modelid, 
		float pos_x, float pos_y, float pos_z, float pos_a,
		uint8_t color1, uint8_t color2);

	void Destroy();

	void Update();

	inline uint32_t GetId() const
	{
		return m_Id;
	}

	point &GetPos();
	void SetPos(float x, float y, float z);
	float GetFacingAngle();
	void SetFacingAngle(float angle);
	float *GetVelocity();
	void SetVelocity(float x, float y, float z);
	inline uint8_t *GetColor()
	{
		return &(m_Color[0]);
	}
	void SetColor(uint8_t color1, uint8_t color2, bool only_val = false);
	inline uint8_t GetPaintjob() const
	{
		return m_Paintjob;
	}
	void SetPaintjob(uint8_t paintjobid, bool only_val = false);
	float GetHealth();
	void SetHealth(float health);
	int *GetDamageStatus();
	void SetDamageStatus(int panels, int doors, int lights, int tires);
	inline int GetVirtualWorld() const
	{
		return m_VirtualWorld;
	}
	void SetVirtualWorld(int worldid);
	inline uint16_t GetModelId() const
	{
		return m_ModelId;
	}
	bool *GetParamsEx();
	void SetParamsEx(bool engine, bool lights, bool alarm, bool doors, bool bonnet, bool boot, bool objective);
	inline uint8_t GetInterior() const
	{
		return m_InteriorId;
	}
	void SetInterior(uint8_t interiorid);

	inline bool IsStreamedForPlayer(CPlayer* player)
	{
		return m_StreamedFor.find(player) != m_StreamedFor.end();
	}
	inline float GetDistance(float x, float y, float z)
	{
		return static_cast<float>(geo::distance(GetPos(), point(x, y, z)));
	}
	CPlayer *GetPlayerInSeat(int8_t seatid);
	int8_t GetPlayerSeatId(CPlayer* player);
	void SetToRespawn();
	inline const char *GetNumberPlate()
	{
		return m_NumberPlate.c_str();
	}
	void SetNumberPlate(string numberplate);
	bool PutPlayerInSeat(CPlayer* player, int8_t seatid);


public: //callback functions
	void OnPlayerEnter(CPlayer *player, int8_t seatid);
	void OnPlayerExit(CPlayer *player);
};


#endif // INC_CVEHICLE_H
