#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "Stage.h"

class Player;

class SceneGame :
    public SceneSuper
{
public:
	SceneGame(FileManager& fileMng);

	~SceneGame() override;

	void Update() override;
	void Draw() override;
	//ゲージの描画
	void DrawGauge(int x,
				   int y,
				   int width,
				   int height,
				   float value,
				   float maxValue,
				   int color);

private:
	static int selectedStageIndex_;

	std::unique_ptr<Stage> stage_;
	std::unique_ptr<Player> player_;
};

