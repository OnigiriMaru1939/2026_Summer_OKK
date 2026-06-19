#pragma once


#include "ItemBase.h"


class Mentos : public ItemBase
{
public:
	Mentos(FileManager& fileMng, Stage* stage, float x, float y);
	~Mentos();
	void Update() override;
	void OnGet(Player& player) override;
};

