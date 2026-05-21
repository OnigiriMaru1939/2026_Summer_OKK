#pragma once


#include "EnemyBase.h"

class Enemy1 : public EnemyBase
{
public:
	Enemy1(FileManager& fileMng, float x = 0.0f, float y = 0.0f);
	~Enemy1() override;
	void Update() override;
};

