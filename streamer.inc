/*
 * Streamer plugin
 * by maddinat0r
 */

#if defined _streamer_included
	#endinput
#endif
#define _streamer_included


enum E_STREAMER_RANGECHECK_TYPE
{
	INVALID,

	RADIUS,
	SIGHT
};



native Streamer_SetRangeCheckType(E_STREAMER_RANGECHECK_TYPE:checktype);
native E_STREAMER_RANGECHECK_TYPE:Streamer_GetRangeCheckType();
native Streamer_SetVehicleStreamDist(Float:distance);
native Float:Streamer_GetVehicleStreamDist();

native Streamer_ManualVehicleEngineAndLights();



native Streamer_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);
native Streamer_DestroyVehicle(vehicleid);

native Streamer_GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
native Streamer_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
native Streamer_GetVehicleZAngle(vehicleid, &Float:z_angle);
native Streamer_SetVehicleZAngle(vehicleid, Float:z_angle);
native Streamer_GetVehicleVelocity(vehicleid, &Float:x, &Float:y, &Float:z);
native Streamer_SetVehicleVelocity(vehicleid, Float:x, Float:y, Float:z);

native Streamer_GetVehicleColor(vehicleid, &color1, &color2);
native Streamer_SetVehicleColor(vehicleid, color1, color2);
native Streamer_GetVehiclePaintjob(vehicleid);
native Streamer_SetVehiclePaintjob(vehicleid, paintjobid);

native Streamer_GetVehicleHealth(vehicleid, &Float:health);
native Streamer_SetVehicleHealth(vehicleid, Float:health);
native Streamer_GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
native Streamer_SetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);

native Streamer_GetVehicleVirtualWorld(vehicleid);
native Streamer_SetVehicleVirtualWorld(vehicleid, worldid);

native Streamer_GetVehicleModel(vehicleid);

native Streamer_GetPlayerVehicleID(playerid);
