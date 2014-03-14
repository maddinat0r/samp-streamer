#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "natives.h"
#include "CPlayer.h"
#include "CVehicle.h"
#include "COption.h"


boost::random::mt19937 RandNumGenerator;
boost::random::uniform_int_distribution<> RandNumColorDist(0, 255);


//native MSP_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);
AMX_DECLARE_NATIVE(Native::MSP_CreateVehicle)
{	
	uint16_t modelid = static_cast<uint16_t>(params[1]);
	if (modelid < 400 || modelid > 611)
		return -1;

	int16_t
		color1 = static_cast<int16_t>(params[6]),
		color2 = static_cast<int16_t>(params[7]);

	if (color1 > 255 || color2 > 255)
		return -1;

	if (color1 < 0)
		color1 = RandNumColorDist(RandNumGenerator);

	if (color2 < 0)
		color2 = RandNumColorDist(RandNumGenerator);

	CVehicle *veh = CVehicle::Create(
		modelid,
		amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]), amx_ctof(params[5]),
		static_cast<uint8_t>(color1), static_cast<uint8_t>(color2)
	);
	return veh->GetId();
}

//native MSP_DestroyVehicle(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_DestroyVehicle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->Destroy();
	return 1;
}

//native MSP_GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
AMX_DECLARE_NATIVE(Native::MSP_GetVehiclePos)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


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

//native MSP_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
AMX_DECLARE_NATIVE(Native::MSP_SetVehiclePos)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->SetPos(amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	return 1;
}

//native MSP_GetVehicleZAngle(vehicleid, &Float:z_angle);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleZAngle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	cell *addr_a = nullptr;
	amx_GetAddr(amx, params[2], &addr_a);

	float veh_a = vehicle->GetFacingAngle();
	(*addr_a) = amx_ftoc(veh_a);

	return 1;
}

//native MSP_SetVehicleZAngle(vehicleid, Float:z_angle);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleZAngle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->SetFacingAngle(amx_ctof(params[2]));
	return 1;
}

//native MSP_GetVehicleVelocity(vehicleid, &Float:x, &Float:y, &Float:z);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleVelocity)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	float *veh_velocity = vehicle->GetVelocity();
	for (size_t i = 0; i < 3; ++i)
	{
		cell *addr_ptr = nullptr;
		amx_GetAddr(amx, params[i + 2], &addr_ptr);
		(*addr_ptr) = amx_ftoc(veh_velocity[i]);
	}
	return 1;
}

//native MSP_SetVehicleVelocity(vehicleid, Float:x, Float:y, Float:z);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleVelocity)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->SetVelocity(amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	return 1;
}

//native MSP_GetVehicleColor(vehicleid, &color1, &color2);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleColor)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	uint8_t *color_array = vehicle->GetColor();
	for (size_t i = 0; i < 2; ++i)
	{
		cell *addr_ptr = nullptr;
		amx_GetAddr(amx, params[i + 2], &addr_ptr);
		(*addr_ptr) = color_array[i];
	}
	return 1;
}

//native MSP_GetVehicleColor1(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleColor1)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	uint8_t *color_array = vehicle->GetColor();
	return color_array[0];
}

//native MSP_GetVehicleColor2(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleColor2)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	uint8_t *color_array = vehicle->GetColor();
	return color_array[1];
}

//native MSP_SetVehicleColor(vehicleid, color1, color2);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleColor)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	int16_t
		color1 = static_cast<int16_t>(params[2]),
		color2 = static_cast<int16_t>(params[3]);

	if (color1 > 255 || color2 > 255)
		return 0;

	if (color1 < 0)
		color1 = RandNumColorDist(RandNumGenerator);

	if (color2 < 0)
		color2 = RandNumColorDist(RandNumGenerator);


	vehicle->SetColor(static_cast<uint8_t>(color1), static_cast<uint8_t>(color2));
	return 1;
}

//native MSP_GetVehiclePaintjob(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehiclePaintjob)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return static_cast<cell>(vehicle->GetPaintjob());
}

//native MSP_SetVehiclePaintjob(vehicleid, paintjobid);
AMX_DECLARE_NATIVE(Native::MSP_SetVehiclePaintjob)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;
	

	vehicle->SetPaintjob(static_cast<uint8_t>(params[2]));
	return 1;
}

//native MSP_GetVehicleHealth(vehicleid, &Float:health);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleHealth)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	cell *addr_health = nullptr;
	amx_GetAddr(amx, params[2], &addr_health);

	float health_val = vehicle->GetHealth();
	(*addr_health) = amx_ftoc(health_val);

	return 1;
}

//native MSP_SetVehicleHealth(vehicleid, Float:health);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleHealth)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->SetHealth(amx_ctof(params[2]));
	return 1;
}

//native MSP_GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleDamageStatus)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	int *dmg_status_array = vehicle->GetDamageStatus();
	for (size_t i = 0; i < 4; ++i)
	{
		cell *addr_ptr = nullptr;
		amx_GetAddr(amx, params[i + 2], &addr_ptr);
		(*addr_ptr) = dmg_status_array[i];
	}
	return 1;
}

//native MSP_SetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleDamageStatus)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->SetDamageStatus(params[2], params[3], params[4], params[5]);
	return 1;
}

//native MSP_GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleParamsEx)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr || COption::Get()->IsUsingVehicleParamsEx() == false)
		return 0;


	bool *params_array = vehicle->GetParamsEx();
	for (size_t i = 0; i < 7; ++i)
	{
		cell *addr_ptr = nullptr;
		amx_GetAddr(amx, params[i+2], &addr_ptr);
		(*addr_ptr) = params_array[i] == true ? 1 : 0;
	}
	return 1;
}

//native MSP_SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleParamsEx)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	COption::Get()->UseVehicleParamsEx(true);
	vehicle->SetParamsEx(params[2] != 0, params[3] != 0, params[4] != 0, params[5] != 0, params[6] != 0, params[7] != 0, params[8] != 0);
	return 1;
}

//native MSP_GetVehicleNumberPlate(vehicleid, dest[], max_len=sizeof(dest))
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleNumberPlate)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	cell *dest_addr = nullptr;
	amx_GetAddr(amx, params[2], &dest_addr);
	amx_SetString(dest_addr, vehicle->GetNumberPlate(), 0, 0, params[3]);
	return 1;
}

//native MSP_SetVehicleNumberPlate(vehicleid, numberplate[]);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleNumberPlate)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	char *tmp_plate_str = nullptr;
	amx_StrParam(amx, params[2], tmp_plate_str);
	vehicle->SetNumberPlate(tmp_plate_str == nullptr ? string() : tmp_plate_str);
	return 1;
}

//native MSP_GetVehicleVirtualWorld(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleVirtualWorld)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return vehicle->GetVirtualWorld();
}

//native MSP_SetVehicleVirtualWorld(vehicleid, worldid);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleVirtualWorld)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	int worldid = params[2];
	if (worldid < 0)
		return 0;


	vehicle->SetVirtualWorld(worldid);
	return 1;
}

//native MSP_LinkVehicleToInterior(vehicleid, interiorid);
AMX_DECLARE_NATIVE(Native::MSP_LinkVehicleToInterior)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	int interiorid = params[2];
	if (interiorid < 0)
		return 0;


	vehicle->SetInterior(static_cast<uint8_t>(interiorid));
	return 1;
}

//native MSP_GetVehicleInterior(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleInterior)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return static_cast<cell>(vehicle->GetInterior());
}

//native MSP_GetVehicleModel(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleModel)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	return static_cast<cell>(vehicle->GetModelId());
}

//native MSP_IsVehicleStreamedIn(vehicleid, forplayerid);
AMX_DECLARE_NATIVE(Native::MSP_IsVehicleStreamedIn)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	CPlayer *player = CPlayerHandler::Get()->FindPlayer(static_cast<uint16_t>(params[2]));
	if (player == nullptr)
		return 0;


	return vehicle->IsStreamedForPlayer(player) == true ? 1 : 0;
}

//native Float:MSP_GetVehicleDistance(vehicleid, Float:X, Float:Y, Float:Z);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleDistance)
{
	float distance = 0.0;
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));

	if (vehicle != nullptr)
		distance = vehicle->GetDistance(amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	return amx_ftoc(distance);
}

//native MSP_GetPlayerInVehicleSeat(vehicleid, seatid);
AMX_DECLARE_NATIVE(Native::MSP_GetPlayerInVehicleSeat)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	CPlayer *player = vehicle->GetPlayerInSeat(static_cast<int8_t>(params[2]));
	return player == nullptr ? -1 : static_cast<cell>(player->GetId());
}

//native MSP_SetVehicleToRespawn(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleToRespawn)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	vehicle->SetToRespawn();
	return 1;
}

//native MSP_AddVehicleComponent(vehicleid, componentid);
AMX_DECLARE_NATIVE(Native::MSP_AddVehicleComponent)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	return vehicle->AddComponent(static_cast<uint16_t>(params[2])) == true ? 1 : 0;
}

//native MSP_RemoveVehicleComponent(vehicleid, componentid);
AMX_DECLARE_NATIVE(Native::MSP_RemoveVehicleComponent)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	return vehicle->RemoveComponent(static_cast<uint16_t>(params[2])) == true ? 1 : 0;
}

//native MSP_GetVehicleComponentInSlot(vehicleid, slot);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleComponentInSlot)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	uint16_t componentid = vehicle->GetComponentInSlot(static_cast<uint8_t>(params[2]));
	return componentid == 0 ? -1 : static_cast<cell>(componentid);
}

//native MSP_AttachTrailerToVehicle(trailerid, vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_AttachTrailerToVehicle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;

	CVehicle *trailer = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[2]));
	if (trailer == nullptr)
		return 0;


	return vehicle->AttachTrailer(trailer) == true ? 1 : 0;
}

//native MSP_DetachTrailerFromVehicle(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_DetachTrailerFromVehicle)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return 0;


	return vehicle->DetachTrailer() == true ? 1 : 0;
}

//native MSP_GetVehicleTrailer(vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleTrailer)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (vehicle == nullptr)
		return -1;


	CVehicle *trailer = vehicle->GetTrailer();
	return trailer == nullptr ? -1 : static_cast<cell>(trailer->GetId());
}

//native MSP_GetVehiclePullingTrailer(trailerid);
AMX_DECLARE_NATIVE(Native::MSP_GetVehiclePullingTrailer)
{
	CVehicle *trailer = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[1]));
	if (trailer == nullptr)
		return -1;


	CVehicle *vehicle = trailer->GetPullingVehicle();
	return vehicle == nullptr ? -1 : static_cast<cell>(vehicle->GetId());
}

//native MSP_PutPlayerInVehicle(playerid, vehicleid, seatid);
AMX_DECLARE_NATIVE(Native::MSP_PutPlayerInVehicle)
{
	CPlayer *player = CPlayerHandler::Get()->FindPlayer(static_cast<uint16_t>(params[1]));
	if (player == nullptr)
		return 0;

	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[2]));
	if (vehicle == nullptr)
		return 0;

	
	return vehicle->PutPlayerInSeat(player, static_cast<int8_t>(params[3])) == true ? 1 : 0;
}

//native MSP_GetPlayerVehicleID(playerid);
AMX_DECLARE_NATIVE(Native::MSP_GetPlayerVehicleID)
{
	CPlayer *player = CPlayerHandler::Get()->FindPlayer(static_cast<uint16_t>(params[1]));
	if (player == nullptr)
		return 0;


	return player->OccupiedVehicle == nullptr ? 0 : static_cast<cell>(player->OccupiedVehicle->GetId());
}

//native MSP_GetPlayerVehicleSeat(playerid);
AMX_DECLARE_NATIVE(Native::MSP_GetPlayerVehicleSeat)
{
	CPlayer *player = CPlayerHandler::Get()->FindPlayer(static_cast<uint16_t>(params[1]));
	if (player == nullptr)
		return -1;

	if (player->OccupiedVehicle == nullptr)
		return -1;


	return static_cast<cell>(player->OccupiedVehicle->GetPlayerSeatId(player));
}

//native MSP_IsPlayerInVehicle(playerid, vehicleid);
AMX_DECLARE_NATIVE(Native::MSP_IsPlayerInVehicle)
{
	CPlayer *player = CPlayerHandler::Get()->FindPlayer(static_cast<uint16_t>(params[1]));
	if (player == nullptr)
		return 0;
	
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicle(static_cast<uint32_t>(params[2]));
	if (vehicle == nullptr)
		return 0;


	return vehicle == player->OccupiedVehicle ? 1 : 0;
}

//native MSP_IsValidVehicleComponent(modelid, componentid);
AMX_DECLARE_NATIVE(Native::MSP_IsValidVehicleComponent)
{
	return CVehicleHandler::Get()->IsValidComponent(static_cast<uint16_t>(params[1]), static_cast<uint16_t>(params[2])) == true ? 1 : 0;
}



//native MSP_SetRangeCheckType(E_RANGECHECK_TYPE:checktype);
AMX_DECLARE_NATIVE(Native::MSP_SetRangeCheckType)
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

//native E_RANGECHECK_TYPE:MSP_GetRangeCheckType();
AMX_DECLARE_NATIVE(Native::MSP_GetRangeCheckType)
{
	return static_cast<uint8_t>(COption::Get()->GetRangeCheckType());
}

//native MSP_SetVehicleStreamDistance(Float:distance);
AMX_DECLARE_NATIVE(Native::MSP_SetVehicleStreamDist)
{
	COption::Get()->SetVehicleStreamDistance(amx_ctof(params[1]));
	return 1;
}

//native Float:MSP_GetVehicleStreamDistance();
AMX_DECLARE_NATIVE(Native::MSP_GetVehicleStreamDist)
{
	float stream_dist = COption::Get()->GetVehicleStreamDistance();
	return amx_ftoc(stream_dist);
}
