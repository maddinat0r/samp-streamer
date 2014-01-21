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
	CPlayerHandler::Get()->UpdateAll();
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

PLUGIN_EXPORT bool PLUGIN_CALL OnPlayerStateChange(int playerid, int newstate, int oldstate)
{
	if(oldstate = PLAYER_STATE_ONFOOT && (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER))
	{
		CVehicle *vehicle = CVehicleHandler::Get()->FindVehicleByRealID(GetPlayerVehicleID(playerid));
		CPlayer *player = CPlayerHandler::Get()->FindPlayer(playerid); 
		//TODO: callbacks!
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


#if defined __cplusplus
extern "C"
#endif
const AMX_NATIVE_INFO natives_list[] = 
{
	{"Streamer_CreateVehicle",		Streamer_CreateVehicle},
	{0, 0}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	//CCallback::Get()->AddAmx(amx);
	return amx_Register(amx, natives_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	//CCallback::Get()->EraseAmx(amx);
	return AMX_ERR_NONE;
}

