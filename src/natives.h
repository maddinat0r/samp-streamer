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
	AMX_DECLARE_NATIVE(Streamer_CreateVehicle);

	AMX_DECLARE_NATIVE(Streamer_SetRangeCheckType);
	AMX_DECLARE_NATIVE(Streamer_GetRangeCheckType);
	AMX_DECLARE_NATIVE(Streamer_SetVehicleStreamDist);
	AMX_DECLARE_NATIVE(Streamer_GetVehicleStreamDist);
}


#endif // INC_NATIVES_H
