#pragma once


#include "EnemyBase.h"

class Enemy3 : public EnemyBase
{
public:
	Enemy3(FileManager& fileMng, Stage* stage, float x, float y);
	~Enemy3() override;
	void Update() override;
	void Draw() const override;
};