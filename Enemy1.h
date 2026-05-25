#pragma once


#include "EnemyBase.h"

class Enemy1 : public EnemyBase
{
public:
	Enemy1(FileManager& fileMng, Stage* stage, float x, float y);
	~Enemy1() override;
	void Update() override;
	const char* GetEnemyName() const override
	{
		return "Enemy1";
	}
};

