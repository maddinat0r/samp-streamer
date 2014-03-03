#include "natives.h"
#include "CPlayer.h"
#include "CVehicle.h"
#include "COption.h"


//native Streamer_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);
AMX_DECLARE_NATIVE(Native::Streamer_CreateVehicle)
{	
	CVehicle *veh = CVehicle::Create(
		static_cast<uint16_t>(params[1]),
		amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]), amx_ctof(params[5]),
		static_cast<uint8_t>(params[6]), static_cast<uint8_t>(params[7])
	);
	return veh->GetId();
}

//native Streamer_DestroyVehicle(vehicleid);
AMX_DECLARE_NATIVE(Native::Streamer_DestroyVehicle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->Destroy();
	return 1;
}

//native Streamer_GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehiclePos)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	cell 
		*addr_x = nullptr, 
		*addr_y = nullptr, 
		*addr_z = nullptr;
	amx_GetAddr(amx, params[2], &addr_x);
	amx_GetAddr(amx, params[3], &addr_y);
	amx_GetAddr(amx, params[4], &addr_z);

	point &veh_pos = vehicle->GetPos();
	float
		veh_x = geo::get<0>(veh_pos),
		veh_y = geo::get<1>(veh_pos),
		veh_z = geo::get<2>(veh_pos);

	(*addr_x) = amx_ftoc(veh_x);
	(*addr_y) = amx_ftoc(veh_y);
	(*addr_z) = amx_ftoc(veh_z);

	return 1;
}

//native Streamer_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehiclePos)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetPos(amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	return 1;
}

//native Streamer_GetVehicleZAngle(vehicleid, &Float:z_angle);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleZAngle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	cell *addr_z = nullptr;
	amx_GetAddr(amx, params[2], &addr_z);

	float veh_z = vehicle->GetFacingAngle();
	(*addr_z) = amx_ftoc(veh_z);

	return 1;
}

//native Streamer_SetVehicleZAngle(vehicleid, Float:z_angle);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleZAngle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetFacingAngle(amx_ctof(params[2]));
	return 1;
}

//native Streamer_GetVehicleVelocity(vehicleid, &Float:x, &Float:y, &Float:z);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleVelocity)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	cell
		*addr_x = nullptr,
		*addr_y = nullptr,
		*addr_z = nullptr;
	amx_GetAddr(amx, params[2], &addr_x);
	amx_GetAddr(amx, params[3], &addr_y);
	amx_GetAddr(amx, params[4], &addr_z);

	float *veh_velocity = vehicle->GetVelocity();

	(*addr_x) = amx_ftoc(veh_velocity[0]);
	(*addr_y) = amx_ftoc(veh_velocity[1]);
	(*addr_z) = amx_ftoc(veh_velocity[2]);

	return 1;
}

//native Streamer_SetVehicleVelocity(vehicleid, Float:x, Float:y, Float:z);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleVelocity)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetVelocity(amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	return 1;
}

//native Streamer_GetVehicleColor(vehicleid, &color1, &color2);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleColor)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	cell
		*addr_col1 = nullptr,
		*addr_col2 = nullptr;
	amx_GetAddr(amx, params[2], &addr_col1);
	amx_GetAddr(amx, params[3], &addr_col2);

	uint8_t *color_array = vehicle->GetColor();
	(*addr_col1) = color_array[0];
	(*addr_col2) = color_array[1];

	return 1;
}

//native Streamer_SetVehicleColor(vehicleid, color1, color2);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleColor)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetColor(static_cast<uint8_t>(params[2]), static_cast<uint8_t>(params[3]));
	return 1;
}

//native Streamer_GetVehiclePaintjob(vehicleid);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehiclePaintjob)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return static_cast<cell>(vehicle->GetPaintjob());
}

//native Streamer_SetVehiclePaintjob(vehicleid, paintjobid);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehiclePaintjob)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetPaintjob(static_cast<uint8_t>(params[2]));
	return 1;
}

//native Streamer_GetVehicleHealth(vehicleid, &Float:health);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleHealth)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	cell *addr_health = nullptr;
	amx_GetAddr(amx, params[2], &addr_health);

	float health_val = vehicle->GetHealth();
	(*addr_health) = amx_ftoc(health_val);

	return 1;
}

//native Streamer_SetVehicleHealth(vehicleid, Float:health);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleHealth)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetHealth(amx_ctof(params[2]));
	return 1;
}

//native Streamer_GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleDamageStatus)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	cell
		*addr_panels = nullptr,
		*addr_doors = nullptr,
		*addr_lights = nullptr,
		*addr_tires = nullptr;
	amx_GetAddr(amx, params[2], &addr_panels);
	amx_GetAddr(amx, params[3], &addr_doors);
	amx_GetAddr(amx, params[4], &addr_lights);
	amx_GetAddr(amx, params[5], &addr_tires);

	int *dmg_status_array = vehicle->GetDamageStatus();
	(*addr_panels) = dmg_status_array[0];
	(*addr_doors) = dmg_status_array[1];
	(*addr_lights) = dmg_status_array[2];
	(*addr_tires) = dmg_status_array[3];

	return 1;
}

//native Streamer_SetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleDamageStatus)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetDamageStatus(params[2], params[3], params[4], params[5]);
	return 1;
}

//native Streamer_GetVehicleVirtualWorld(vehicleid);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleVirtualWorld)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return vehicle->GetVirtualWorld();
}

//native Streamer_SetVehicleVirtualWorld(vehicleid, worldid);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleVirtualWorld)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	vehicle->SetVirtualWorld(params[2]);
	return 1;
}

//native Streamer_GetVehicleModel(vehicleid);
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleModel)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return static_cast<cell>(vehicle->GetModelId());
}

//native Streamer_GetPlayerVehicleID(playerid);
AMX_DECLARE_NATIVE(Native::Streamer_GetPlayerVehicleID)
{
	CPlayer *player = CPlayerHandler::Get()->FindPlayer(static_cast<uint16_t>(params[1]));
	if (player == nullptr)
		return -1;


	return player->OccupiedVehicle == nullptr ? 0 : static_cast<cell>(player->OccupiedVehicle->GetId());
}



//native Streamer_SetRangeCheckType(E_RANGECHECK_TYPE:checktype);
AMX_DECLARE_NATIVE(Native::Streamer_SetRangeCheckType)
{
	switch(static_cast<uint8_t>(params[1]))
	{
	case 1:
		COption::Get()->SetRangeCheckType(RangeCheckType::RADIUS);
		break;

	case 2:
		COption::Get()->SetRangeCheckType(RangeCheckType::SIGHT);
		break;

	default:
		return 0;
	}
	return 1;
}

//native E_RANGECHECK_TYPE:Streamer_GetRangeCheckType();
AMX_DECLARE_NATIVE(Native::Streamer_GetRangeCheckType)
{
	return static_cast<uint8_t>(COption::Get()->GetRangeCheckType());
}

//native Streamer_SetVehicleStreamDistance(Float:distance);
AMX_DECLARE_NATIVE(Native::Streamer_SetVehicleStreamDist)
{
	COption::Get()->SetVehicleStreamDistance(amx_ctof(params[1]));
	return 1;
}

//native Float:Streamer_GetVehicleStreamDistance();
AMX_DECLARE_NATIVE(Native::Streamer_GetVehicleStreamDist)
{
	float stream_dist = COption::Get()->GetVehicleStreamDistance();
	return amx_ftoc(stream_dist);
}

//native Streamer_ManualVehicleEngineAndLights();
AMX_DECLARE_NATIVE(Native::Streamer_ManualVehicleEngineAndLights)
{
	COption::Get()->UseVehicleParamsEx(true);
	return 1;
}