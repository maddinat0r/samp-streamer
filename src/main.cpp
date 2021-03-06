#include "natives.h"
#include "CPlayer.h"
#include "CFuncCall.h"
#include "CVehicle.h"

#include <sampgdk/core.h>
#include <sampgdk/a_samp.h>
#include <sampgdk/a_players.h>



PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK; 
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
	sampgdk_init(ppData);
	
	sampgdk_logprintf(" >> plugin.streamer: v0.1 successfully loaded.");
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	sampgdk_logprintf("plugin.streamer: Unloading plugin...");
	
	CFuncCall::Get()->Destroy();
	CPlayerHandler::Get()->Destroy();
	CVehicleHandler::Get()->Destroy();
	sampgdk_cleanup();

	sampgdk_logprintf("plugin.streamer: Plugin unloaded."); 
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() 
{
	CFuncCall::Get()->ProcessFuncCalls();
}



SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnPlayerConnect(int playerid)
{
	CPlayerHandler::Get()->AddPlayer(playerid, CPlayer::Create(playerid));
	return true;
}

SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnPlayerDisconnect(int playerid, int reason)
{
	CPlayerHandler::Get()->RemovePlayer(playerid);
	return true;
}

SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnPlayerUpdate(int playerid)
{
	CPlayerHandler::Get()->Update(playerid);
	return true;
}


SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnPlayerStateChange(int playerid, int newstate, int oldstate)
{
	if(oldstate == PLAYER_STATE_ONFOOT && (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER))
	{
		CVehicle *vehicle = CVehicleHandler::Get()->FindVehicleByRealID(GetPlayerVehicleID(playerid));

		if (vehicle != nullptr)
		{
			CPlayer *player = CPlayerHandler::Get()->FindPlayer(playerid);
			vehicle->OnPlayerEnter(player, GetPlayerVehicleSeat(playerid));
			player->OccupiedVehicle = vehicle;
		}
	}
	else if(newstate == PLAYER_STATE_ONFOOT && (oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER))
	{
		CPlayer *player = CPlayerHandler::Get()->FindPlayer(playerid); 

		if (player->OccupiedVehicle != nullptr)
		{
			player->OccupiedVehicle->OnPlayerExit(player);
			player->OccupiedVehicle = nullptr;
		}
	}
	return true;
}

SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnVehiclePaintjob(int playerid, int vehicleid, int paintjobid)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicleByRealID(vehicleid);
	if (vehicle != nullptr)
		vehicle->SetPaintjob(paintjobid, true);
	return true;
}

SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnVehicleRespray(int playerid, int vehicleid, int color1, int color2)
{
	CVehicle *vehicle = CVehicleHandler::Get()->FindVehicleByRealID(vehicleid);
	if (vehicle != nullptr)
		vehicle->SetColor(color1, color2, true);
	return true;
}

SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnVehicleDeath(int vehicleid, int killerid)
{

	return true;
}

SAMPGDK_CALLBACK_EXPORT bool SAMPGDK_CALLBACK_CALL OnPlayerDeath(int playerid, int killerid, int reason)
{

	return true;
}


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	AMX_DEFINE_NATIVE(MSP_SetRangeCheckType)
	AMX_DEFINE_NATIVE(MSP_GetRangeCheckType)
	AMX_DEFINE_NATIVE(MSP_SetVehicleStreamDist)
	AMX_DEFINE_NATIVE(MSP_GetVehicleStreamDist)


	AMX_DEFINE_NATIVE(MSP_CreateVehicle)
	AMX_DEFINE_NATIVE(MSP_DestroyVehicle)
	AMX_DEFINE_NATIVE(MSP_GetVehiclePos)
	AMX_DEFINE_NATIVE(MSP_SetVehiclePos)
	AMX_DEFINE_NATIVE(MSP_GetVehicleZAngle)
	AMX_DEFINE_NATIVE(MSP_SetVehicleZAngle)
	AMX_DEFINE_NATIVE(MSP_GetVehicleVelocity)
	AMX_DEFINE_NATIVE(MSP_SetVehicleVelocity)
	AMX_DEFINE_NATIVE(MSP_GetVehicleColor)
	AMX_DEFINE_NATIVE(MSP_GetVehicleFirstColor)
	AMX_DEFINE_NATIVE(MSP_GetVehicleSecondColor)
	AMX_DEFINE_NATIVE(MSP_SetVehicleColor)
	AMX_DEFINE_NATIVE(MSP_GetVehiclePaintjob)
	AMX_DEFINE_NATIVE(MSP_SetVehiclePaintjob)
	AMX_DEFINE_NATIVE(MSP_GetVehicleHealth)
	AMX_DEFINE_NATIVE(MSP_SetVehicleHealth)
	AMX_DEFINE_NATIVE(MSP_GetVehicleDamageStatus)
	AMX_DEFINE_NATIVE(MSP_SetVehicleDamageStatus)
	AMX_DEFINE_NATIVE(MSP_GetVehicleParamsEx)
	AMX_DEFINE_NATIVE(MSP_SetVehicleParamsEx)
	AMX_DEFINE_NATIVE(MSP_GetVehicleNumberPlate)
	AMX_DEFINE_NATIVE(MSP_SetVehicleNumberPlate)
	AMX_DEFINE_NATIVE(MSP_GetVehicleVirtualWorld)
	AMX_DEFINE_NATIVE(MSP_SetVehicleVirtualWorld)
	AMX_DEFINE_NATIVE(MSP_LinkVehicleToInterior)
	AMX_DEFINE_NATIVE(MSP_GetVehicleInterior)
	AMX_DEFINE_NATIVE(MSP_GetVehicleModel)
	AMX_DEFINE_NATIVE(MSP_IsVehicleStreamedIn)
	AMX_DEFINE_NATIVE(MSP_GetVehicleDistance)
	AMX_DEFINE_NATIVE(MSP_GetPlayerInVehicleSeat)
	AMX_DEFINE_NATIVE(MSP_SetVehicleToRespawn)
	AMX_DEFINE_NATIVE(MSP_AddVehicleComponent)
	AMX_DEFINE_NATIVE(MSP_RemoveVehicleComponent)
	AMX_DEFINE_NATIVE(MSP_GetVehicleComponentInSlot)
	AMX_DEFINE_NATIVE(MSP_AttachTrailerToVehicle)
	AMX_DEFINE_NATIVE(MSP_DetachTrailerFromVehicle)
	AMX_DEFINE_NATIVE(MSP_GetVehicleTrailer)
	AMX_DEFINE_NATIVE(MSP_GetVehiclePullingTrailer)
	AMX_DEFINE_NATIVE(MSP_PutPlayerInVehicle)
	AMX_DEFINE_NATIVE(MSP_GetPlayerVehicleID)
	AMX_DEFINE_NATIVE(MSP_GetPlayerVehicleSeat)
	AMX_DEFINE_NATIVE(MSP_IsPlayerInVehicle)
	AMX_DEFINE_NATIVE(MSP_IsValidVehicleComponent)

	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	//CCallback::Get()->AddAmx(amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	//CCallback::Get()->EraseAmx(amx);
	return AMX_ERR_NONE;
}

