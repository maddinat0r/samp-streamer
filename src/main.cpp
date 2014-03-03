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



PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerConnect(int playerid) 
{
	CPlayerHandler::Get()->AddPlayer(playerid, CPlayer::Create(playerid));
	return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDisconnect(int playerid, int reason) 
{
	CPlayerHandler::Get()->RemovePlayer(playerid);
	return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerUpdate(int playerid) 
{
	CPlayerHandler::Get()->Update(playerid);
	return true;
}


PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerStateChange(int playerid, int newstate, int oldstate)
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

PLUGIN_EXPORT bool PLUGIN_CALL OnVehicleDeath(int vehicleid, int killerid)
{

	return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerDeath(int playerid, int killerid, int reason)
{

	return true;
}


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	AMX_DEFINE_NATIVE(Streamer_SetRangeCheckType)
	AMX_DEFINE_NATIVE(Streamer_GetRangeCheckType)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleStreamDist)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleStreamDist)
	AMX_DEFINE_NATIVE(Streamer_ManualVehicleEngineAndLights)


	AMX_DEFINE_NATIVE(Streamer_CreateVehicle)
	AMX_DEFINE_NATIVE(Streamer_DestroyVehicle)
	AMX_DEFINE_NATIVE(Streamer_GetVehiclePos)
	AMX_DEFINE_NATIVE(Streamer_SetVehiclePos)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleZAngle)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleZAngle)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleVelocity)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleVelocity)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleColor)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleColor)
	AMX_DEFINE_NATIVE(Streamer_GetVehiclePaintjob)
	AMX_DEFINE_NATIVE(Streamer_SetVehiclePaintjob)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleHealth)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleHealth)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleDamageStatus)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleDamageStatus)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleVirtualWorld)
	AMX_DEFINE_NATIVE(Streamer_SetVehicleVirtualWorld)
	AMX_DEFINE_NATIVE(Streamer_GetVehicleModel)
	AMX_DEFINE_NATIVE(Streamer_GetPlayerVehicleID)

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

