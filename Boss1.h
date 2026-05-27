#pragma once

#include "IBoss.h"
#include "EnemyBase.h"

class Boss1 : public EnemyBase, public IBoss
{
public:
	Boss1(FileManager& fileMng, Stage* stage, float x, float y);
	~Boss1() override;
	void Update() override;
	const char* GetEnemyName() const override
	{
		return "Boss1";
	}
};

