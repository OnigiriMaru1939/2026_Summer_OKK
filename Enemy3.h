#pragma once


#include "EnemyBase.h"


class Enemy3 : public EnemyBase
{
public:
	Enemy3(FileManager& fileMng, Stage& stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng);
	~Enemy3() override;
	void Update() override;
	void ShotAttack();
private:
	int shotTimer = 0;
};