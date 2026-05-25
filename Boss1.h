#pragma once


#include "EnemyBase.h"

class Boss1 : public EnemyBase
{
public:
	Boss1(FileManager& fileMng, Stage* stage, float x, float y);
	~Boss1() override;
	void Update() override;
};

