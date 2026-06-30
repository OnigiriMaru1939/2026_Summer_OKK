#pragma once

#include "EnemyBase.h"

class Enemy1 : public EnemyBase
{
public:
	Enemy1(FileManager& fileMng, Stage* stage, float x, float y, ParticleManager& pMng);
	~Enemy1() override;
	void Update() override;
};

