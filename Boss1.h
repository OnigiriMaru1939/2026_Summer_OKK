#pragma once

#include "IBoss.h"
#include "EnemyBase.h"

class Boss1 : public EnemyBase, public IBoss
{
public:
	Boss1(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng);
	~Boss1() override;
	void Update() override;
	void Draw() const override;
	void BossAppear();
	void BossMove();
	void Jump();
};

