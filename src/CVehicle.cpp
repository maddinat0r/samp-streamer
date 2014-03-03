#include "CVehicle.h"
#include "CPlayer.h"
#include "CFuncCall.h"
#include "COption.h"

#include <sampgdk/a_vehicles.h>

#include <boost/chrono/chrono.hpp>
namespace chrono = boost::chrono;


CVehicleHandler *CVehicleHandler::m_Instance = nullptr;


void CVehicleHandler::AddVehicle(CVehicle *veh, bool only_rtree /* = false*/)
{
	if(only_rtree == false)
	{
		veh->m_Id = 1;
		while(m_Vehicles.find(veh->m_Id) != m_Vehicles.end())
			veh->m_Id++;

		m_Vehicles.insert(unordered_map<uint32_t, CVehicle *>::value_type(veh->m_Id, veh));
	}
	veh->m_LastRtreeValue = boost::make_tuple(veh->m_Pos, veh);
	m_Rtree.insert(veh->m_LastRtreeValue);
}

void CVehicleHandler::RemoveVehicle(CVehicle *veh, bool only_rtree /* = false*/)
{
	if(only_rtree == false)
		m_Vehicles.quick_erase(m_Vehicles.find(veh->m_Id));
	
	m_Rtree.remove(veh->m_LastRtreeValue);
}

CVehicle *CVehicleHandler::FindVehicle(uint32_t vid)
{
	unordered_map<uint32_t, CVehicle *>::iterator i = m_Vehicles.find(vid);
	return (i != m_Vehicles.end()) ? i->second : nullptr;
}

CVehicle *CVehicleHandler::FindVehicleByRealID(uint32_t vehid)
{
	for(unordered_map<uint32_t, CVehicle *>::iterator i = m_Vehicles.begin(), end = m_Vehicles.end(); i != end; ++i)
	{
		if(i->second->m_VehicleId == vehid)
			return i->second;
	}
	return nullptr;
}

bool stream_check_func(const boost::tuple<point, CVehicle *> &v, CPlayer *player)
{
	const point &veh_pos = boost::get<0>(v);
	return player->ShouldStream(geo::get<0>(veh_pos), geo::get<1>(veh_pos), geo::get<2>(veh_pos), COption::Get()->GetVehicleStreamDistance());
}
void CVehicleHandler::StreamAll(CPlayer *player)
{
	std::vector<boost::tuple<point, CVehicle *> > query_res;
	m_Rtree.query(geo::index::satisfies(boost::bind(&stream_check_func, _1, player)), std::back_inserter(query_res));
	
	set<CVehicle *> invalid_vehicles = player->StreamedVehicles;

	for(auto &t : query_res)
	{
		CVehicle *vehicle = boost::get<1>(t);
		if(vehicle->m_StreamedFor.empty())
			CFuncCall::Get()->QueueFunc(boost::bind(&CVehicle::CreateInternalVeh, vehicle));
			
		vehicle->m_StreamedFor.insert(player->GetId());
		player->StreamedVehicles.insert(vehicle);
		
		invalid_vehicles.erase(vehicle);
	}

	for(auto &v : invalid_vehicles)
	{
		if(!v->m_StreamedFor.empty() && v->m_SeatInfo.empty())
		{
			v->m_StreamedFor.erase(player->GetId());
			player->StreamedVehicles.erase(v);

			if(v->m_StreamedFor.empty())
				CFuncCall::Get()->QueueFunc(boost::bind(&CVehicle::DestroyInternalVeh, v));
				
		}
	}
}

CVehicleHandler::~CVehicleHandler()
{
	for(unordered_map<uint32_t, CVehicle *>::iterator i = m_Vehicles.begin(), end = m_Vehicles.end(); i != end; ++i)
		i->second->Destroy();
}



CVehicle *CVehicle::Create(uint16_t modelid,
	float pos_x, float pos_y, float pos_z, float pos_a,
	uint8_t color1, uint8_t color2)
{
	CVehicle *veh = new CVehicle;


	veh->m_ModelId = modelid;

	veh->m_Pos = point(pos_x, pos_y, pos_z);
	veh->m_FacingAngle = pos_a;

	veh->m_Color[0] = color1;
	veh->m_Color[1] = color2;

	CVehicleHandler::Get()->AddVehicle(veh); //vehicle id is assigned in this function

	return veh;
}

void CVehicle::Destroy()
{
	CVehicleHandler::Get()->RemoveVehicle(this);
	if (m_VehicleId != 0)
		DestroyVehicle(m_VehicleId);
	delete this;
}

void CVehicle::CreateInternalVeh()
{
	m_VehicleId = CreateVehicle(m_ModelId, geo::get<0>(m_Pos), geo::get<1>(m_Pos), geo::get<2>(m_Pos), m_FacingAngle, m_Color[0], m_Color[1], -1);
	SetVehicleVelocity(m_VehicleId, m_Velocity[0], m_Velocity[1], m_Velocity[2]);
	if (COption::Get()->IsUsingVehicleParamsEx())
		SetVehicleParamsEx(m_VehicleId, m_ParamsEx[0], m_ParamsEx[1], m_ParamsEx[2], m_ParamsEx[3], m_ParamsEx[4], m_ParamsEx[5], m_ParamsEx[6]);
	ChangeVehiclePaintjob(m_VehicleId, m_Paintjob);
	SetVehicleHealth(m_VehicleId, m_Health);
	UpdateVehicleDamageStatus(m_VehicleId, m_DamageStatus[0], m_DamageStatus[1], m_DamageStatus[2], m_DamageStatus[3]);
	SetVehicleVirtualWorld(m_VehicleId, m_VirtualWorld);
}

void CVehicle::DestroyInternalVeh()
{
	Update();
	DestroyVehicle(m_VehicleId);
	m_VehicleId = 0;
}

void CVehicle::Update()
{
	if (m_VehicleId == 0)
		return;
	
	float tmp_pos[3];

	GetVehiclePos(m_VehicleId, &tmp_pos[0], &tmp_pos[1], &tmp_pos[2]);
	geo::set<0>(m_Pos, tmp_pos[0]);
	geo::set<1>(m_Pos, tmp_pos[1]);
	geo::set<2>(m_Pos, tmp_pos[2]);

	GetVehicleVelocity(m_VehicleId, &m_Velocity[0], &m_Velocity[1], &m_Velocity[2]);
	GetVehicleZAngle(m_VehicleId, &m_FacingAngle);
	GetVehicleParamsEx(m_VehicleId, &m_ParamsEx[0], &m_ParamsEx[1], &m_ParamsEx[2], &m_ParamsEx[3], &m_ParamsEx[4], &m_ParamsEx[5], &m_ParamsEx[6]);
	GetVehicleHealth(m_VehicleId, &m_Health);
	GetVehicleDamageStatus(m_VehicleId, &m_DamageStatus[0], &m_DamageStatus[1], &m_DamageStatus[2], &m_DamageStatus[3]);
	m_VirtualWorld = GetVehicleVirtualWorld(m_VehicleId);
}

point &CVehicle::GetPos()
{
	if (m_VehicleId != 0)
	{
		float tmp_pos[3];

		GetVehiclePos(m_VehicleId, &tmp_pos[0], &tmp_pos[1], &tmp_pos[2]);
		geo::set<0>(m_Pos, tmp_pos[0]);
		geo::set<1>(m_Pos, tmp_pos[1]);
		geo::set<2>(m_Pos, tmp_pos[2]);
	}
	return m_Pos;
}
void CVehicle::SetPos(float x, float y, float z)
{
	geo::set<0>(m_Pos, x);
	geo::set<1>(m_Pos, y);
	geo::set<2>(m_Pos, z);

	if (m_SeatInfo.empty()) //no one is in the vehicle, so we assume the vehicle is in the r-tree
	{
		//update the position of the vehicle in the r-tree
		CVehicleHandler::Get()->RemoveVehicle(this, true);
		CVehicleHandler::Get()->AddVehicle(this, true);
	}

	if (m_VehicleId != 0)
		SetVehiclePos(m_VehicleId, x, y, z);
}

float CVehicle::GetFacingAngle()
{
	if (m_VehicleId != 0)
		GetVehicleZAngle(m_VehicleId, &m_FacingAngle);

	return m_FacingAngle;
}
void CVehicle::SetFacingAngle(float angle)
{
	m_FacingAngle = angle;

	if (m_VehicleId != 0)
		SetVehicleZAngle(m_VehicleId, angle);
}

float *CVehicle::GetVelocity()
{
	if (m_VehicleId != 0)
		GetVehicleVelocity(m_VehicleId, &m_Velocity[0], &m_Velocity[1], &m_Velocity[2]);

	return &(m_Velocity[0]);
}
void CVehicle::SetVelocity(float x, float y, float z)
{
	m_Velocity[0] = x;
	m_Velocity[1] = y;
	m_Velocity[2] = z;

	if (m_VehicleId != 0)
		SetVehicleVelocity(m_VehicleId, x, y, z);
}

void CVehicle::SetColor(uint8_t color1, uint8_t color2)
{
	m_Color[0] = color1;
	m_Color[1] = color2;

	if (m_VehicleId != 0)
		ChangeVehicleColor(m_VehicleId, color1, color2);
}

void CVehicle::SetPaintjob(uint8_t paintjobid)
{
	m_Paintjob = paintjobid;

	if (m_VehicleId != 0)
		ChangeVehiclePaintjob(m_VehicleId, paintjobid);
}

float CVehicle::GetHealth()
{
	if (m_VehicleId != 0)
		GetVehicleHealth(m_VehicleId, &m_Health);

	return m_Health;
}
void CVehicle::SetHealth(float health)
{
	m_Health = health;

	if (m_VehicleId != 0)
		SetVehicleHealth(m_VehicleId, health);
}

int *CVehicle::GetDamageStatus()
{
	if (m_VehicleId != 0)
		GetVehicleDamageStatus(m_VehicleId, &m_DamageStatus[0], &m_DamageStatus[1], &m_DamageStatus[2], &m_DamageStatus[3]);

	return &(m_DamageStatus[0]);
}
void CVehicle::SetDamageStatus(int panels, int doors, int lights, int tires)
{
	m_DamageStatus[0] = panels;
	m_DamageStatus[1] = doors;
	m_DamageStatus[2] = lights;
	m_DamageStatus[3] = tires;

	if (m_VehicleId != 0)
		UpdateVehicleDamageStatus(m_VehicleId, panels, doors, lights, tires);
}

void CVehicle::SetVirtualWorld(int worldid)
{
	m_VirtualWorld = worldid;

	if (m_VehicleId != 0)
		SetVehicleVirtualWorld(m_VehicleId, worldid);
}


void CVehicle::OnPlayerEnter(CPlayer *player, int8_t seatid)
{
	m_SeatInfo.insert( unordered_map<int8_t, uint32_t>::value_type(seatid, player->GetId()) );
	CVehicleHandler::Get()->RemoveVehicle(this, true);
}

void CVehicle::OnPlayerExit(CPlayer *player)
{
	Update();
	for(unordered_map<int8_t, uint32_t>::iterator i = m_SeatInfo.begin(), end = m_SeatInfo.end(); i != end; ++i)
	{
		if(i->second == player->GetId())
		{
			m_SeatInfo.quick_erase(i);
			break;
		}
	}
	CVehicleHandler::Get()->AddVehicle(this, true);
}
