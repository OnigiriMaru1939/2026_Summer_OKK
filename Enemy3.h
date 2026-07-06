#pragma once


#include "EnemyBase.h"

class Enemy3 : public EnemyBase
{
public:
	Enemy3(FileManager& fileMng, Stage* stage, float x, float y, ParticleManager& pMng);
	~Enemy3() override;
	void Update() override;
};