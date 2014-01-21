#include "CPlayer.h"
#include "CVehicle.h"

#include <sampgdk/a_players.h>
#include <sampgdk/core.h>

#include <boost/chrono/chrono.hpp>

namespace chrono = boost::chrono;


CPlayerHandler *CPlayerHandler::m_Instance = new CPlayerHandler;


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

void CPlayerHandler::UpdateAll()
{
	for(unordered_map<uint16_t, CPlayer *>::iterator i = m_Players.begin(), end = m_Players.end(); i != end; ++i)
		i->second->Update();
}

void CPlayerHandler::ThreadFunc()
{
	m_ThreadRunning = true;
	while(m_ThreadRunning)
	{
		for(unordered_map<uint16_t, CPlayer *>::iterator i = m_Players.begin(), end = m_Players.end(); i != end; ++i) //very bad
		{
			CVehicleHandler::Get()->StreamAll(i->second);
		}

		this_thread::sleep_for(chrono::milliseconds(20));
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

	GetPlayerVelocity(m_Id, &m_Velocity[0], &m_Velocity[1], &m_Velocity[2]);
}


bool CPlayer::IsInRange(float x, float y, float z, RangeCheckType type)
{
	bool ret_val = false;
	m_DataLock = true;

	switch(type)
	{
		case RangeCheckType::RADIUS:
		{
			//TODO: implement radius check
			//TODO: create global option class for streaming values and range-check type

		} break;
	
		case RangeCheckType::SIGHT:
		{
			float camera_angle = 0.0f;

			switch (m_CameraMode)
			{
			case 4:
				camera_angle = 41.5f;
				break;

			default:
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

	float cal_angle = acosf(scal_prod / vec_len) * (180.0f / 3.14159265f);
	ret_val = cal_angle <= camera_angle;
		} break;
	}

	m_DataLock = false;
	return ret_val;
}
