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



native MSP_SetRangeCheckType(E_STREAMER_RANGECHECK_TYPE:checktype);
native E_STREAMER_RANGECHECK_TYPE:MSP_GetRangeCheckType();
native MSP_SetVehicleStreamDist(Float:distance);
native Float:MSP_GetVehicleStreamDist();



native MSP_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);
native MSP_DestroyVehicle(vehicleid);

native MSP_GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
native MSP_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
native MSP_GetVehicleZAngle(vehicleid, &Float:z_angle);
native MSP_SetVehicleZAngle(vehicleid, Float:z_angle);
native MSP_GetVehicleVelocity(vehicleid, &Float:x, &Float:y, &Float:z);
native MSP_SetVehicleVelocity(vehicleid, Float:x, Float:y, Float:z);

native MSP_GetVehicleColor(vehicleid, &color1, &color2);
native MSP_GetVehicleFirstColor(vehicleid);
native MSP_GetVehicleSecondColor(vehicleid);
native MSP_SetVehicleColor(vehicleid, color1, color2);
native MSP_GetVehiclePaintjob(vehicleid);
native MSP_SetVehiclePaintjob(vehicleid, paintjobid);

native MSP_GetVehicleHealth(vehicleid, &Float:health);
native MSP_SetVehicleHealth(vehicleid, Float:health);
native MSP_GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
native MSP_SetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
native MSP_GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective);
native MSP_SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
native MSP_GetVehicleNumberPlate(vehicleid, dest[], max_len=sizeof(dest));
native MSP_SetVehicleNumberPlate(vehicleid, numberplate[]);

native MSP_GetVehicleVirtualWorld(vehicleid);
native MSP_SetVehicleVirtualWorld(vehicleid, worldid);

native MSP_LinkVehicleToInterior(vehicleid, interiorid);
native MSP_GetVehicleInterior(vehicleid);

native MSP_GetVehicleModel(vehicleid);
native MSP_IsVehicleStreamedIn(vehicleid, forplayerid);
native Float:MSP_GetVehicleDistance(vehicleid, Float:X, Float:Y, Float:Z);
native MSP_GetPlayerInVehicleSeat(vehicleid, seatid);

native MSP_SetVehicleToRespawn(vehicleid);

native MSP_AddVehicleComponent(vehicleid, componentid);
native MSP_RemoveVehicleComponent(vehicleid, componentid);
native MSP_GetVehicleComponentInSlot(vehicleid, slot);

native MSP_AttachTrailerToVehicle(trailerid, vehicleid);
native MSP_DetachTrailerFromVehicle(vehicleid);
native MSP_GetVehicleTrailer(vehicleid);
native MSP_GetVehiclePullingTrailer(trailerid);

native MSP_PutPlayerInVehicle(playerid, vehicleid, seatid);
native MSP_GetPlayerVehicleID(playerid);
native MSP_GetPlayerVehicleSeat(playerid);

native MSP_IsPlayerInVehicle(playerid, vehicleid);

native MSP_IsValidVehicleComponent(modelid, componentid);

