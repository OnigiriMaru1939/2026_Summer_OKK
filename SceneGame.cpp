#include "SceneGame.h"
#include "InputManager.h"
#include "Player.h"

// タイルサイズ
static constexpr int kTileWidth = 32;
static constexpr int kTileHeight = 32;

SceneGame::SceneGame(FileManager& fileMng) : SceneSuper(fileMng)
{
	player = std::make_unique<Player>(fileMng);
    player->SystemInit();
	player->SetImage("Resource/Image/Monster.png");
  
	stage_ = std::make_unique<Stage>(fileMng, kTileWidth, kTileHeight);

	// タイルセットとマップデータを読み込む
	stage_->LoadTileSet("Resource/MapChip/TileSet1.png");
	stage_->LoadMapFromCSV("Resource/MapCSV/AGS2026MAP.csv");
}

SceneGame::~SceneGame()
{
	InputManager::GetInstance().ClearPressCallbacks();
	InputManager::GetInstance().ClearTriggerCallbacks();
	InputManager::GetInstance().ClearReleaseCallbacks();
}

void SceneGame::Update()
{
	if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
	{
		SetNextScene(SceneID::RESULT);
		isEnd = true;
	}

	player->Update();

	// Stageの更新
	if (stage_) stage_->Update();
}

void SceneGame::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x00aa00, true);

	// Stageを描画
	if (stage_) stage_->Draw();

	DrawString(0, 20, "GAME SCENE", 0xffffff);
	//プレイヤーを描画
	player->Draw();

	//ゲージの描画
	DrawGauge(20, 50, 500, 40, player->playerHp, player->playerHpMax, GetColor(0, 255, 0));						//プレイヤーのHPゲージ
	DrawGauge(20, 120, 500, 40, player->sodaShakeGauge, player->sodaShakeGaugeMax, GetColor(0, 0, 255));		//炭酸蓄積ゲージ
}

//ゲージの描画
void SceneGame::DrawGauge(
	int x,
	int y,
	int width,
	int height,
	float value,
	float maxValue,
	int color)
{
	//ゲージの枠
	DrawBox(x - 1,
			y - 1,
			x + width + 1,
			y + height + 1,
			GetColor(255, 0, 0),
			FALSE
	);

	// ゲージ割合
	int barWidth = (int)((value / maxValue) * width);

	// 中身
	DrawBox(
		x,
		y,
		x + barWidth,
		y + height,
		color,
		TRUE
	);
}