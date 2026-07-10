#pragma once

#include "EnemyBase.h"

class Enemy2 : public EnemyBase
{
public:
	Enemy2(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng);
	~Enemy2() override;
	void Update() override;
	void Move();
};

