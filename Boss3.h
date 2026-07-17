#pragma once

#include "IBoss.h"
#include "EnemyBase.h"

class Boss3 : public EnemyBase, public IBoss
{
public:
	enum BOSS_STATE
	{
		NON,

		WAIT,
		MOVE,
		SHOT,
		SUMMON,

		MAX
	};

	Boss3(FileManager& fileMng, Stage& stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng);
	~Boss3() override;
	void Update() override;
	void Draw() const override;
	void BossAppear();
	void Wait();
	void Move();
	void ShotAttack();
	void EnemySummon();
	void BossStateChange();
	void SelectNextState();
	void ApplyDamage(int dmg) override;
private:
	//敵の召喚リスト
	std::vector<std::weak_ptr<EnemyBase>> summonEnemyList_;

	BOSS_STATE bossState_;
	int stateChangeTimer;
	int shotTimer = 0;
	bool summonFlag_;
};

