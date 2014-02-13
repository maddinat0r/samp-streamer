#include "natives.h"
#include "CVehicle.h"
#include "COption.h"


//native Streamer_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);
AMX_DECLARE_NATIVE(Native::Streamer_CreateVehicle)
{	
	CVehicle *veh = CVehicle::Create(
		static_cast<uint16_t>(params[1]),
		amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]), amx_ctof(params[5]),
		static_cast<int16_t>(params[6]), static_cast<int16_t>(params[7])
	);
	return veh->GetId();
}


//native Streamer_SetRangeCheckType(E_RANGECHECK_TYPE:checktype);
AMX_DECLARE_NATIVE(Native::Streamer_SetRangeCheckType)
{
	switch(static_cast<uint8_t>(params[1]))
	{
	case RangeCheckType::RADIUS:
		COption::Get()->SetRangeCheckType(RangeCheckType::RADIUS);
		break;

	case RangeCheckType::SIGHT:
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
