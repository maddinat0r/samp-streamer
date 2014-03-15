#include "CPlayer.h"
#include "CVehicle.h"
#include "COption.h"

#include <sampgdk/a_players.h>
#include <sampgdk/a_vehicles.h>
#include <sampgdk/core.h>

#include <boost/chrono/chrono.hpp>

namespace chrono = boost::chrono;


CPlayerHandler *CPlayerHandler::m_Instance = nullptr;


 CPlayerHandler::CPlayerHandler() :
	m_Thread(new thread(boost::bind(&CPlayerHandler::ThreadFunc, this)))
{ }

CPlayerHandler::~CPlayerHandler()
{
	m_ThreadRunning = false;
	m_Thread->join();
	delete m_Thread;


	for(unordered_map<uint16_t, CPlayer *>::iterator i = m_Players.begin(), end = m_Players.end(); i != end; ++i)
		(*i).second->Destroy();
}


CPlayer *CPlayerHandler::FindPlayer(uint16_t playerid)
{
	CPlayer *player = nullptr;
	unordered_map<uint16_t, CPlayer *>::iterator i;
	if( (i = m_Players.find(playerid)) != m_Players.end())
		player = i->second;
	return player;
}


void CPlayerHandler::ThreadFunc()
{
	m_ThreadRunning = true;
	while(m_ThreadRunning)
	{
		for(auto &p : m_Players) //TODO: very bad (concurrency)
			CVehicleHandler::Get()->StreamAll(p.second);

		this_thread::sleep_for(chrono::milliseconds(10));
	}
}




void CPlayer::Update()
{
	if(m_DataLock == true)
		return ;
	
	float tmp_pos[3];


	GetPlayerCameraPos(m_Id, &m_CameraPos[0], &m_CameraPos[1], &m_CameraPos[2]);
	GetPlayerCameraFrontVector(m_Id, &m_CameraDir[0], &m_CameraDir[1], &m_CameraDir[2]);
	m_CameraMode = GetPlayerCameraMode(m_Id);
	
	GetPlayerPos(m_Id, &tmp_pos[0], &tmp_pos[1], &tmp_pos[2]);
	m_Pos = point(tmp_pos[0], tmp_pos[1], tmp_pos[2]);

	m_InVehicle = GetPlayerVehicleID(m_Id);
	if(m_InVehicle)
		GetVehicleVelocity(m_InVehicle, &m_Velocity[0], &m_Velocity[1], &m_Velocity[2]);
	else
		GetPlayerVelocity(m_Id, &m_Velocity[0], &m_Velocity[1], &m_Velocity[2]);
}


bool CPlayer::ShouldStream(float x, float y, float z, float stream_dist)
{
	bool ret_val = false;
	m_DataLock = true;

	//adjust position relative to velocity
	x += m_Velocity[0];
	y += m_Velocity[1];
	z += m_Velocity[2];

	switch(COption::Get()->GetRangeCheckType())
	{
		case RangeCheckType::RADIUS:
		{
			x -= geo::get<0>(m_Pos);
			y -= geo::get<1>(m_Pos);
			z -= geo::get<2>(m_Pos);

			ret_val = sqrtf(x*x + y*y + z*z) <= stream_dist;
		} break;
		case RangeCheckType::SIGHT:
		{
			float camera_angle = 0.0f;

			switch (m_CameraMode)
			{
				case 4: //on-foot
				case 7: //sniper
				case 51: //heat-seaking rocketlauncher
				case 53: //weapon aiming
				case 46: //aiming with weapon "camera"
				case 15: //fixed camera
				case 18: //in-vehicle //TODO: driver drive-by has viewing angle depending on camera<->vehicle distance
					camera_angle = 41.2f;
					break;

				case 16: //first-person
					camera_angle = 36.8f;
					break;
			}

			float 
				scal_prod,
				vec_len;

			x -= m_CameraPos[0];
			y -= m_CameraPos[1];
			z -= m_CameraPos[2];

			scal_prod = (m_CameraDir[0] * x) + (m_CameraDir[1] * y) + (m_CameraDir[2] * z);
			vec_len = sqrtf(x*x + y*y + z*z);
			if(vec_len <= stream_dist)
			{
				float cal_angle = acosf(scal_prod / vec_len) * (180.0f / 3.14159265f);
				ret_val = cal_angle <= camera_angle;
			}
		} break;
	}
	m_DataLock = false;
	return ret_val;
}
