#pragma once
#include "SceneSuper.h"
#include "Application.h"

class Player;

class SceneGame :
    public SceneSuper
{
public:
	SceneGame(FileManager& fileMng);

	~SceneGame() override;

	void Update() override;
	void Draw() override;

private:
	std::unique_ptr<Player> player;
};

