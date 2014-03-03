#pragma once
#ifndef INC_NATIVES_H
#define INC_NATIVES_H


#include <sampgdk/core.h>

#define AMX_DECLARE_NATIVE(native) \
        cell AMX_NATIVE_CALL native(AMX *amx, cell *params)

#define AMX_DEFINE_NATIVE(native) \
        {#native, Native::native},


namespace Native
{
	AMX_DECLARE_NATIVE(Streamer_SetRangeCheckType);
	AMX_DECLARE_NATIVE(Streamer_GetRangeCheckType);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleStreamDist);
	AMX_DECLARE_NATIVE(Streamer_GetVehicleStreamDist);

	AMX_DECLARE_NATIVE(Streamer_ManualVehicleEngineAndLights);


	
	AMX_DECLARE_NATIVE(Streamer_CreateVehicle);
	AMX_DECLARE_NATIVE(Streamer_DestroyVehicle);

	AMX_DECLARE_NATIVE(Streamer_GetVehiclePos);
	AMX_DECLARE_NATIVE(Streamer_SetVehiclePos);
	AMX_DECLARE_NATIVE(Streamer_GetVehicleZAngle);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleZAngle);
	AMX_DECLARE_NATIVE(Streamer_GetVehicleVelocity);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleVelocity);

	AMX_DECLARE_NATIVE(Streamer_GetVehicleColor);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleColor);
	AMX_DECLARE_NATIVE(Streamer_GetVehiclePaintjob);
	AMX_DECLARE_NATIVE(Streamer_SetVehiclePaintjob);

	AMX_DECLARE_NATIVE(Streamer_GetVehicleHealth);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleHealth);
	AMX_DECLARE_NATIVE(Streamer_GetVehicleDamageStatus);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleDamageStatus);

	AMX_DECLARE_NATIVE(Streamer_GetVehicleVirtualWorld);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleVirtualWorld);

	AMX_DECLARE_NATIVE(Streamer_GetVehicleModel);

	AMX_DECLARE_NATIVE(Streamer_GetPlayerVehicleID);
}


#endif // INC_NATIVES_H
