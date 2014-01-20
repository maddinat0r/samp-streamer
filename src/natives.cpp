#include "CVehicle.h"

#include <sampgdk/core.h>


//native Streamer_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);
cell AMX_NATIVE_CALL Streamer_CreateVehicle(AMX* amx, cell* params)
{	
	CVehicle *veh = CVehicle::Create(
		static_cast<uint16_t>(params[1]),
		amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]), amx_ctof(params[5]),
		static_cast<int16_t>(params[6]), static_cast<int16_t>(params[7]));

	return veh->GetId();
}