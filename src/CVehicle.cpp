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
	m_Rtree.insert(boost::make_tuple(veh->m_Pos, veh));
}

void CVehicleHandler::RemoveVehicle(CVehicle *veh, bool only_rtree /* = false*/)
{
	if(only_rtree == false)
		m_Vehicles.quick_erase(m_Vehicles.find(veh->m_Id));
	
	m_Rtree.remove(boost::make_tuple(veh->m_Pos, veh));
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
	int16_t color1, int16_t color2)
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
	delete this;
}

void CVehicle::CreateInternalVeh()
{
	m_VehicleId = CreateVehicle(m_ModelId, geo::get<0>(m_Pos), geo::get<1>(m_Pos), geo::get<2>(m_Pos), m_FacingAngle, m_Color[0], m_Color[1], -1);
}

void CVehicle::DestroyInternalVeh()
{
	Update();
	DestroyVehicle(m_VehicleId);
	m_VehicleId = 0;
}

void CVehicle::Update()
{
	float tmp_pos[3];

	GetVehiclePos(m_VehicleId, &tmp_pos[0], &tmp_pos[1], &tmp_pos[2]);
	geo::set<0>(m_Pos, tmp_pos[0]);
	geo::set<1>(m_Pos, tmp_pos[1]);
	geo::set<2>(m_Pos, tmp_pos[2]);

	GetVehicleZAngle(m_VehicleId, &m_FacingAngle);
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
