#include "CVehicle.h"
#include "CPlayer.h"
#include "CFuncCall.h"
#include "COption.h"

#include <sampgdk/a_vehicles.h>
#include <sampgdk/a_players.h>

#include <boost/chrono/chrono.hpp>
namespace chrono = boost::chrono;


CVehicleHandler *CVehicleHandler::m_Instance = nullptr;


void CVehicleHandler::AddVehicle(CVehicle *veh, bool only_rtree /* = false*/)
{
	if(only_rtree == false)
	{
		if (!m_UnusedIds.empty())
		{
			veh->m_Id = m_UnusedIds.front();
			m_UnusedIds.pop();
		}
		else
			veh->m_Id = ++m_UpperId;

		m_Vehicles.insert(unordered_map<uint32_t, CVehicle *>::value_type(veh->m_Id, veh));
	}
	veh->m_LastRtreeValue = boost::make_tuple(veh->m_Pos, veh);

	m_RtreeMtx.lock();
	m_Rtree.insert(veh->m_LastRtreeValue);
	m_RtreeMtx.unlock();
}

void CVehicleHandler::RemoveVehicle(CVehicle *veh, bool only_rtree /* = false*/)
{
	if (only_rtree == false)
	{
		m_Vehicles.erase(veh->m_Id);
		m_UnusedIds.push(veh->m_Id);
	}
	
	m_RtreeMtx.lock();
	m_Rtree.remove(veh->m_LastRtreeValue);
	m_RtreeMtx.unlock();
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

bool stream_check_func(const tuple<point, CVehicle *> &v, CPlayer *player)
{
	const point &veh_pos = boost::get<0>(v);
	return player->ShouldStream(geo::get<0>(veh_pos), geo::get<1>(veh_pos), geo::get<2>(veh_pos), COption::Get()->GetVehicleStreamDistance());
}
void CVehicleHandler::StreamAll(CPlayer *player)
{
	std::vector<tuple<point, CVehicle *> > query_res;
	m_RtreeMtx.lock();
	m_Rtree.query(geo::index::satisfies(boost::bind(&stream_check_func, _1, player)), std::back_inserter(query_res));
	m_RtreeMtx.unlock();
	
	set<CVehicle *> invalid_vehicles = player->StreamedVehicles;

	//sort query_res by nearest player <-> vehicle distance
	point &player_pos = player->GetPos();
	std::sort(query_res.begin(), query_res.end(), 
		[&](const tuple<point, CVehicle *> &val1, const tuple<point, CVehicle *> &val2) -> bool
		{
			return (geo::distance(player_pos, boost::get<0>(val1)) < geo::distance(player_pos, boost::get<0>(val2)));
		}
	);

	if (query_res.size() > MAX_VEHICLES)
		query_res.resize(MAX_VEHICLES);

	for(auto &t : query_res)
	{
		CVehicle *vehicle = boost::get<1>(t);
		if(vehicle->m_StreamedFor.empty())
			CFuncCall::Get()->QueueFunc(boost::bind(&CVehicle::CreateInternalVeh, vehicle));
			
		vehicle->m_StreamedFor.insert(player);
		player->StreamedVehicles.insert(vehicle);
		
		invalid_vehicles.erase(vehicle);
	}

	for(auto &v : invalid_vehicles)
	{
		if(!v->m_StreamedFor.empty() && v->m_SeatInfo.empty())
		{
			v->m_StreamedFor.erase(player);
			player->StreamedVehicles.erase(v);

			if(v->m_StreamedFor.empty())
				CFuncCall::Get()->QueueFunc(boost::bind(&CVehicle::DestroyInternalVeh, v));
				
		}
	}
}

bool CVehicleHandler::IsValidComponent(uint16_t modelid, uint16_t componentid)
{
	//thanks to JernejL for all of this component data in this function
	static uint16_t valid_wheel_components[17] = { 1025, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1096, 1097, 1098 };
	static uint16_t valid_components[48][22] = 
	{
		{ 400, 1024, 1021, 1020, 1019, 1018, 1013, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 401, 1145, 1144, 1143, 1142, 1020, 1019, 1017, 1013, 1007, 1006, 1005, 1004, 1003, 1001, 0000, 0000, 0000, 0000 },
		{ 404, 1021, 1020, 1019, 1017, 1016, 1013, 1007, 1002, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 405, 1023, 1021, 1020, 1019, 1018, 1014, 1001, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 410, 1024, 1023, 1021, 1020, 1019, 1017, 1013, 1007, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 415, 1023, 1019, 1018, 1017, 1007, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 418, 1021, 1020, 1016, 1006, 1002, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 420, 1021, 1019, 1005, 1004, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 421, 1023, 1021, 1020, 1019, 1018, 1016, 1014, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 422, 1021, 1020, 1019, 1017, 1013, 1007, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 426, 1021, 1019, 1006, 1005, 1004, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 436, 1022, 1021, 1020, 1019, 1017, 1013, 1007, 1006, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 439, 1145, 1144, 1143, 1142, 1023, 1017, 1013, 1007, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 477, 1021, 1020, 1019, 1018, 1017, 1007, 1006, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 478, 1024, 1022, 1021, 1020, 1013, 1012, 1005, 1004, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 489, 1024, 1020, 1019, 1018, 1016, 1013, 1006, 1005, 1004, 1002, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 491, 1145, 1144, 1143, 1142, 1023, 1021, 1020, 1019, 1018, 1017, 1014, 1007, 1003, 0000, 0000, 0000, 0000, 0000 },
		{ 492, 1016, 1006, 1005, 1004, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 496, 1143, 1142, 1023, 1020, 1019, 1017, 1011, 1007, 1006, 1003, 1002, 1001, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 500, 1024, 1021, 1020, 1019, 1013, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 516, 1021, 1020, 1019, 1018, 1017, 1016, 1015, 1007, 1004, 1002, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 517, 1145, 1144, 1143, 1142, 1023, 1020, 1019, 1018, 1017, 1016, 1007, 1003, 1002, 0000, 0000, 0000, 0000, 0000 },
		{ 518, 1145, 1144, 1143, 1142, 1023, 1020, 1018, 1017, 1013, 1007, 1006, 1005, 1003, 1001, 0000, 0000, 0000, 0000 },
		{ 527, 1021, 1020, 1018, 1017, 1015, 1014, 1007, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 529, 1023, 1020, 1019, 1018, 1017, 1012, 1011, 1007, 1006, 1003, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 534, 1185, 1180, 1179, 1178, 1127, 1126, 1125, 1124, 1123, 1122, 1106, 1101, 1100, 0000, 0000, 0000, 0000, 0000 },
		{ 535, 1121, 1120, 1119, 1118, 1117, 1116, 1115, 1114, 1113, 1110, 1109, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 536, 1184, 1183, 1182, 1181, 1128, 1108, 1107, 1105, 1104, 1103, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 540, 1145, 1144, 1143, 1142, 1024, 1023, 1020, 1019, 1018, 1017, 1007, 1006, 1004, 1001, 0000, 0000, 0000, 0000 },
		{ 542, 1145, 1144, 1021, 1020, 1019, 1018, 1015, 1014, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 546, 1145, 1144, 1143, 1142, 1024, 1023, 1019, 1018, 1017, 1007, 1006, 1004, 1002, 1001, 0000, 0000, 0000, 0000 },
		{ 547, 1143, 1142, 1021, 1020, 1019, 1018, 1016, 1003, 1000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 549, 1145, 1144, 1143, 1142, 1023, 1020, 1019, 1018, 1017, 1012, 1011, 1007, 1003, 1001, 0000, 0000, 0000, 0000 },
		{ 550, 1145, 1144, 1143, 1142, 1023, 1020, 1019, 1018, 1006, 1005, 1004, 1003, 1001, 0000, 0000, 0000, 0000, 0000 },
		{ 551, 1023, 1021, 1020, 1019, 1018, 1016, 1006, 1005, 1003, 1002, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 558, 1168, 1167, 1166, 1165, 1164, 1163, 1095, 1094, 1093, 1092, 1091, 1090, 1089, 1088, 0000, 0000, 0000, 0000 },
		{ 559, 1173, 1162, 1161, 1160, 1159, 1158, 1072, 1071, 1070, 1069, 1068, 1067, 1066, 1065, 0000, 0000, 0000, 0000 },
		{ 560, 1170, 1169, 1141, 1140, 1139, 1138, 1033, 1032, 1031, 1030, 1029, 1028, 1027, 1026, 0000, 0000, 0000, 0000 },
		{ 561, 1157, 1156, 1155, 1154, 1064, 1063, 1062, 1061, 1060, 1059, 1058, 1057, 1056, 1055, 1031, 1030, 1027, 1026 },
		{ 562, 1172, 1171, 1149, 1148, 1147, 1146, 1041, 1040, 1039, 1038, 1037, 1036, 1035, 1034, 0000, 0000, 0000, 0000 },
		{ 565, 1153, 1152, 1151, 1150, 1054, 1053, 1052, 1051, 1050, 1049, 1048, 1047, 1046, 1045, 0000, 0000, 0000, 0000 },
		{ 567, 1189, 1188, 1187, 1186, 1133, 1132, 1131, 1130, 1129, 1102, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 575, 1177, 1176, 1175, 1174, 1099, 1044, 1043, 1042, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 576, 1193, 1192, 1191, 1190, 1137, 1136, 1135, 1134, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 580, 1023, 1020, 1018, 1017, 1007, 1006, 1001, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 589, 1145, 1144, 1024, 1020, 1018, 1017, 1016, 1013, 1007, 1006, 1005, 1004, 1000, 0000, 0000, 0000, 0000, 0000 },
		{ 600, 1022, 1020, 1018, 1017, 1013, 1007, 1006, 1005, 1004, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000 },
		{ 603, 1145, 1144, 1143, 1142, 1024, 1023, 1020, 1019, 1018, 1017, 1007, 1006, 1001, 0000, 0000, 0000, 0000, 0000 }
	};


	if (modelid < 400 || modelid > 611)
		return false;

	if (componentid < 1000 || componentid > 1193)
		return false;

	if (componentid == 1086) //stereo
		return true;

	if (componentid == 1087) //hydraulics
		return true;

	if (componentid == 1008 || componentid == 1009 || componentid == 1010) //nos
	{
		static uint16_t illegal_nos_models[29] = { 581, 523, 462, 521, 463, 522, 461, 448, 468, 586, 509, 481, 510, 472, 473, 493, 595, 484, 430, 453, 452, 446, 454, 590, 569, 537, 538, 570, 449 };
		for (size_t i = 0; i < 29; ++i)
		{
			if (illegal_nos_models[i] == modelid)
				return false;
		}
		return true;
	}

	//wheels
	for (size_t i = 0; i < 17; ++i)
	{
		if (componentid == valid_wheel_components[i])
			return true;
	}

	//legal mod check
	for (size_t i = 0; i < 48; ++i)
	{
		if (valid_components[i][0] == modelid)
		{
			for (size_t j = 1; j < 22; ++j)
			{
				if (valid_components[i][j] == componentid)
					return true;
			}
		}
	}

	return false;
}

CVehicleHandler::~CVehicleHandler()
{
	for (auto &v : m_Vehicles)
	{
		v.second->DestroyInternalVeh();
		delete v.second;
	}
}



CVehicle *CVehicle::Create(uint16_t modelid,
	float pos_x, float pos_y, float pos_z, float pos_a,
	uint8_t color1, uint8_t color2)
{
	CVehicle *veh = new CVehicle;


	veh->m_ModelId = modelid;

	veh->m_Pos = veh->m_SpawnPos = point(pos_x, pos_y, pos_z);
	veh->m_FacingAngle = veh->m_SpawnFacingAngle = pos_a;

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
	if (m_VehicleId != 0)
		return;
	
	m_VehicleId = CreateVehicle(m_ModelId, geo::get<0>(m_Pos), geo::get<1>(m_Pos), geo::get<2>(m_Pos), m_FacingAngle, m_Color[0], m_Color[1], -1);
	SetVehicleVelocity(m_VehicleId, m_Velocity[0], m_Velocity[1], m_Velocity[2]);
	if (COption::Get()->IsUsingVehicleParamsEx())
		SetVehicleParamsEx(m_VehicleId, m_ParamsEx[0], m_ParamsEx[1], m_ParamsEx[2], m_ParamsEx[3], m_ParamsEx[4], m_ParamsEx[5], m_ParamsEx[6]);
	ChangeVehiclePaintjob(m_VehicleId, m_Paintjob);
	SetVehicleHealth(m_VehicleId, m_Health);
	UpdateVehicleDamageStatus(m_VehicleId, m_DamageStatus[0], m_DamageStatus[1], m_DamageStatus[2], m_DamageStatus[3]);
	SetVehicleVirtualWorld(m_VehicleId, m_VirtualWorld);
	LinkVehicleToInterior(m_VehicleId, m_InteriorId);
	SetVehicleNumberPlate(m_VehicleId, m_NumberPlate.c_str());

	if (m_Trailer != nullptr)
	{
		m_Trailer->CreateInternalVeh();
		AttachTrailerToVehicle(m_Trailer->m_VehicleId, m_VehicleId);
	}
}

void CVehicle::DestroyInternalVeh()
{
	if (m_VehicleId == 0)
		return;
	
	Update();
	DestroyVehicle(m_VehicleId);
	m_VehicleId = 0;

	if (m_Trailer != nullptr)
	{
		DetachTrailerFromVehicle(m_VehicleId);
		m_Trailer->DestroyInternalVeh();
	}
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

void CVehicle::SetColor(uint8_t color1, uint8_t color2, bool only_val)
{
	m_Color[0] = color1;
	m_Color[1] = color2;

	if (m_VehicleId != 0 && only_val == false)
		ChangeVehicleColor(m_VehicleId, color1, color2);
}

void CVehicle::SetPaintjob(uint8_t paintjobid, bool only_val)
{
	m_Paintjob = paintjobid;

	if (m_VehicleId != 0 && only_val == false)
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

bool *CVehicle::GetParamsEx()
{
	if (m_VehicleId != 0)
		GetVehicleParamsEx(m_VehicleId, &m_ParamsEx[0], &m_ParamsEx[1], &m_ParamsEx[2], &m_ParamsEx[3], &m_ParamsEx[4], &m_ParamsEx[5], &m_ParamsEx[6]);

	return &(m_ParamsEx[0]);
}
void CVehicle::SetParamsEx(bool engine, bool lights, bool alarm, bool doors, bool bonnet, bool boot, bool objective)
{
	m_ParamsEx[0] = engine;
	m_ParamsEx[1] = lights;
	m_ParamsEx[2] = alarm;
	m_ParamsEx[3] = doors;
	m_ParamsEx[4] = bonnet;
	m_ParamsEx[5] = boot;
	m_ParamsEx[6] = objective;

	if (m_VehicleId != 0 && COption::Get()->IsUsingVehicleParamsEx() == true)
		SetVehicleParamsEx(m_VehicleId, engine, lights, alarm, doors, bonnet, boot, objective);
}

void CVehicle::SetNumberPlate(string numberplate)
{
	m_NumberPlate = numberplate;

	if (m_VehicleId != 0)
		SetVehicleNumberPlate(m_VehicleId, numberplate.c_str());
}

void CVehicle::SetInterior(uint8_t interiorid)
{
	m_InteriorId = interiorid;

	if (m_VehicleId != 0)
		LinkVehicleToInterior(m_VehicleId, interiorid);
}


CPlayer *CVehicle::GetPlayerInSeat(int8_t seatid)
{
	CPlayer *player = nullptr;
	auto i = m_SeatInfo.find(seatid);
	if (i != m_SeatInfo.end())
		player = i->second;
	return player;
}
int8_t CVehicle::GetPlayerSeatId(CPlayer* player)
{
	for (auto &s : m_SeatInfo)
	{
		if (s.second == player)
			return s.first;
	}
	return -1;
}
void CVehicle::SetToRespawn()
{
	SetPos(geo::get<0>(m_SpawnPos), geo::get<1>(m_SpawnPos), geo::get<2>(m_SpawnPos));
	SetFacingAngle(m_SpawnFacingAngle);
}
bool CVehicle::AddComponent(uint16_t componentid)
{
	if (CVehicleHandler::Get()->IsValidComponent(m_ModelId, componentid) == false)
		return false;

	uint8_t slot = GetVehicleComponentType(componentid);
	if (slot > 13 || m_Components[slot] != 0)
		return false;

	m_Components[slot] = componentid;

	if (m_VehicleId != 0)
		AddVehicleComponent(m_VehicleId, componentid);

	return true;
}
bool CVehicle::RemoveComponent(uint16_t componentid)
{
	uint8_t slot = GetVehicleComponentType(componentid);
	if (slot > 13 || m_Components[slot] != componentid)
		return false;

	m_Components[slot] = 0;

	if (m_VehicleId != 0)
		RemoveVehicleComponent(m_VehicleId, componentid);

	return true;
}
uint16_t CVehicle::GetComponentInSlot(uint8_t slot)
{
	if (slot > 13)
		return 0;

	return m_Components[slot];
}
bool CVehicle::AttachTrailer(CVehicle *trailer)
{
	if (m_Trailer != nullptr)
		return false;

	CVehicleHandler::Get()->RemoveVehicle(trailer, true);
	trailer->m_PullingVehicle = this;
	m_Trailer = trailer;

	if (m_VehicleId != 0)
		AttachTrailerToVehicle(trailer->m_VehicleId, m_VehicleId);

	return true;
}
bool CVehicle::DetachTrailer()
{
	if (m_Trailer == nullptr)
		return false;

	if (m_VehicleId != 0)
		DetachTrailerFromVehicle(m_VehicleId);

	m_Trailer->m_PullingVehicle = nullptr;
	CVehicleHandler::Get()->AddVehicle(m_Trailer, true);

	m_Trailer = nullptr;
	return true;
}
bool CVehicle::PutPlayerInSeat(CPlayer* player, int8_t seatid)
{
	if (m_VehicleId == 0)
		CreateInternalVeh();

	if (m_SeatInfo.find(seatid) != m_SeatInfo.end())
		return false; //seat is already taken

	int8_t old_seatid = GetPlayerSeatId(player);
	if (seatid == old_seatid)
		return false;


	PutPlayerInVehicle(player->GetId(), m_VehicleId, seatid);

	m_SeatInfo.insert(unordered_map<int8_t, CPlayer *>::value_type(seatid, player));

	if (old_seatid != -1) //make old seat free
		m_SeatInfo.erase(old_seatid);
	return true;
}


void CVehicle::OnPlayerEnter(CPlayer *player, int8_t seatid)
{
	m_SeatInfo.insert( unordered_map<int8_t, CPlayer *>::value_type(seatid, player) );
	CVehicleHandler::Get()->RemoveVehicle(this, true);
}

void CVehicle::OnPlayerExit(CPlayer *player)
{
	Update();
	for(auto &s : m_SeatInfo)
	{
		if(s.second == player)
		{
			m_SeatInfo.erase(s.first);
			break;
		}
	}
	CVehicleHandler::Get()->AddVehicle(this, true);
}
