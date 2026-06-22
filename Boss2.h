#pragma once

#include "IBoss.h"
#include "EnemyBase.h"

class Boss2 : public EnemyBase, public IBoss
{
public:
	enum BOSS_STATE
	{
		NON,

		MOVE,
		JUMP,
		DASH,

		MAX
	};
	Boss2(FileManager& fileMng, Stage* stage, float x, float y);
	~Boss2() override;
	void Update() override;
	void Draw() const override;
	void BossAppear();
	void BossMove();
	void Jump();
	void Dash();
	void BossStateChange();
private:
	BOSS_STATE bossState_;
	int stateChangeTimer;
};