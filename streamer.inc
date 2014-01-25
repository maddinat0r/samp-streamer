/*
 * Streamer plugin
 * by maddinat0r
 */

#if defined _streamer_included
	#endinput
#endif
#define _streamer_included


enum E_RANGECHECK_TYPE
{
	INVALID,

	RADIUS,
	SIGHT
};


native Streamer_CreateVehicle(modelid, Float:pos_x, Float:pos_y, Float:pos_z, Float:pos_a, color1, color2);

native Streamer_SetRangeCheckType(E_RANGECHECK_TYPE:checktype);
native E_RANGECHECK_TYPE:Streamer_GetRangeCheckType();
native Streamer_SetVehicleStreamDist(Float:distance);
native Float:Streamer_GetVehicleStreamDist();
