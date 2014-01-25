#include "CVehicle.h"
#include "CPlayer.h"
#include "CFuncCall.h"

#include <sampgdk/a_vehicles.h>

#include <boost/chrono/chrono.hpp>
namespace chrono = boost::chrono;


//#include <Windows.h>


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
	{
		m_Vehicles.quick_erase(m_Vehicles.find(veh->m_Id));
	}
	m_Rtree.remove(boost::make_tuple(veh->m_Pos, veh));
}

CVehicle *CVehicleHandler::FindVehicle(uint32_t vid)
{
	CVehicle *vehicle = nullptr;
	unordered_map<uint32_t, CVehicle *>::iterator i;
	if( (i = m_Vehicles.find(vid)) != m_Vehicles.end())
		vehicle = i->second;
	return vehicle;
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


void CVehicleHandler::StreamAll(CPlayer *player)
{
	point &player_pos = player->GetPos();

    /*
	LONGLONG g_Frequency, g_CurentCount, g_LastCount; 
    QueryPerformanceFrequency((LARGE_INTEGER*)&g_Frequency);
    QueryPerformanceCounter((LARGE_INTEGER*)&g_CurentCount); 
	*/

	auto check_func = [&player_pos](boost::tuple<point, CVehicle *> const& v) 
	{
		return geo::distance(player_pos, boost::get<1>(v)->GetPos()) <= 400.0f; 
	};

	std::vector<boost::tuple<point, CVehicle *> > query_res;
	m_Rtree.query(geo::index::satisfies(check_func), std::back_inserter(query_res));

    /*
	QueryPerformanceCounter((LARGE_INTEGER*)&g_LastCount); 
    double dTimeDiff = (((double)(g_LastCount-g_CurentCount))/((double)g_Frequency))*1000.0;  
	printf("time: %f\n", dTimeDiff);
	*/

	for(auto &t : query_res)
	{
		CVehicle *vehicle = boost::get<1>(t);
		point veh_pos = vehicle->GetPos();
		if(player->IsInRange(geo::get<0>(veh_pos), geo::get<1>(veh_pos), geo::get<2>(veh_pos)))
		{
			if(vehicle->m_StreamedFor.empty())
				CFuncCall::Get()->QueueFunc(boost::bind(&CVehicle::CreateInternalVeh, vehicle));
			
			vehicle->m_StreamedFor.insert(player->GetId());
		}
		else
		{
			if(!vehicle->m_StreamedFor.empty())
			{
				vehicle->m_StreamedFor.erase(player->GetId());

				if(vehicle->m_StreamedFor.empty())
					CFuncCall::Get()->QueueFunc(boost::bind(&CVehicle::DestroyInternalVeh, vehicle));
				
			}
		}
	}
}


CVehicleHandler::~CVehicleHandler()
{
	for(unordered_map<uint32_t, CVehicle *>::iterator i = m_Vehicles.begin(), end = m_Vehicles.end(); i != end; ++i)
		i->second->Destroy();
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

void CVehicle::Update()
{
	float tmp_pos[3];

	GetVehiclePos(m_VehicleId, &tmp_pos[0], &tmp_pos[1], &tmp_pos[2]);
	m_Pos = point(tmp_pos[0], tmp_pos[1], tmp_pos[2]);

	GetVehicleZAngle(m_VehicleId, &m_FacingAngle);
}

void CVehicle::OnPlayerEnter(CPlayer *player, int8_t seatid)
{
	CVehicleHandler::Get()->RemoveVehicle(this, true);
	m_SeatInfo.insert( unordered_map<int8_t, uint32_t>::value_type(seatid, player->GetId()) );
}

void CVehicle::OnPlayerExit(CPlayer *player)
{
	CVehicleHandler::Get()->AddVehicle(this, true);
	for(unordered_map<int8_t, uint32_t>::iterator i = m_SeatInfo.begin(), end = m_SeatInfo.end(); i != end; ++i)
	{
		if(i->second == player->GetId())
		{
			m_SeatInfo.quick_erase(i);
			break;
		}
	}
}
