#pragma once


#include "EnemyBase.h"

class Enemy2 : public EnemyBase
{
public:
	Enemy2(FileManager& fileMng, Stage* stage, float x, float y);
	~Enemy2() override;
	void Update() override;
};

