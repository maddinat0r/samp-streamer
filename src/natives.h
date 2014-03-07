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
	AMX_DECLARE_NATIVE(MSP_SetRangeCheckType);
	AMX_DECLARE_NATIVE(MSP_GetRangeCheckType);
	AMX_DECLARE_NATIVE(MSP_SetVehicleStreamDist);
	AMX_DECLARE_NATIVE(MSP_GetVehicleStreamDist);


	
	AMX_DECLARE_NATIVE(MSP_CreateVehicle);
	AMX_DECLARE_NATIVE(MSP_DestroyVehicle);

	AMX_DECLARE_NATIVE(MSP_GetVehiclePos);
	AMX_DECLARE_NATIVE(MSP_SetVehiclePos);
	AMX_DECLARE_NATIVE(MSP_GetVehicleZAngle);
	AMX_DECLARE_NATIVE(MSP_SetVehicleZAngle);
	AMX_DECLARE_NATIVE(MSP_GetVehicleVelocity);
	AMX_DECLARE_NATIVE(MSP_SetVehicleVelocity);

	AMX_DECLARE_NATIVE(MSP_GetVehicleColor);
	AMX_DECLARE_NATIVE(MSP_SetVehicleColor);
	AMX_DECLARE_NATIVE(MSP_GetVehiclePaintjob);
	AMX_DECLARE_NATIVE(MSP_SetVehiclePaintjob);

	AMX_DECLARE_NATIVE(MSP_GetVehicleHealth);
	AMX_DECLARE_NATIVE(MSP_SetVehicleHealth);
	AMX_DECLARE_NATIVE(MSP_GetVehicleDamageStatus);
	AMX_DECLARE_NATIVE(MSP_SetVehicleDamageStatus);
	AMX_DECLARE_NATIVE(MSP_GetVehicleParamsEx);
	AMX_DECLARE_NATIVE(MSP_SetVehicleParamsEx);
	AMX_DECLARE_NATIVE(MSP_GetVehicleNumberPlate);
	AMX_DECLARE_NATIVE(MSP_SetVehicleNumberPlate);

	AMX_DECLARE_NATIVE(MSP_GetVehicleVirtualWorld);
	AMX_DECLARE_NATIVE(MSP_SetVehicleVirtualWorld);

	AMX_DECLARE_NATIVE(MSP_LinkVehicleToInterior);
	AMX_DECLARE_NATIVE(MSP_GetVehicleInterior);

	AMX_DECLARE_NATIVE(MSP_GetVehicleModel);
	AMX_DECLARE_NATIVE(MSP_IsVehicleStreamedIn);
	AMX_DECLARE_NATIVE(MSP_GetVehicleDistance);
	AMX_DECLARE_NATIVE(MSP_GetPlayerInVehicleSeat);

	AMX_DECLARE_NATIVE(MSP_SetVehicleToRespawn);

	AMX_DECLARE_NATIVE(MSP_AddVehicleComponent);
	AMX_DECLARE_NATIVE(MSP_RemoveVehicleComponent);
	AMX_DECLARE_NATIVE(MSP_GetVehicleComponentInSlot);

	AMX_DECLARE_NATIVE(MSP_AttachTrailerToVehicle);
	AMX_DECLARE_NATIVE(MSP_DetachTrailerFromVehicle);
	AMX_DECLARE_NATIVE(MSP_GetVehicleTrailer);
	AMX_DECLARE_NATIVE(MSP_GetVehiclePullingTrailer);

	AMX_DECLARE_NATIVE(MSP_PutPlayerInVehicle);
	AMX_DECLARE_NATIVE(MSP_GetPlayerVehicleID);
	AMX_DECLARE_NATIVE(MSP_GetPlayerVehicleSeat);

	AMX_DECLARE_NATIVE(MSP_IsPlayerInVehicle);

	AMX_DECLARE_NATIVE(MSP_IsValidVehicleComponent);
}


#endif // INC_NATIVES_H
