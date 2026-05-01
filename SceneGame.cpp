#include "SceneGame.h"
#include "InputManager.h"
#include "Player.h"

// タイルサイズ
static constexpr int kTileWidth = 32;
static constexpr int kTileHeight = 32;

SceneGame::SceneGame(FileManager& fileMng) : SceneSuper(fileMng)
{

	stage_ = std::make_unique<Stage>(fileMng, kTileWidth, kTileHeight);

	// タイルセットとマップデータを読み込む
	stage_->LoadTileSet("Resource/MapChip/TileSet1.png");
	stage_->LoadMapFromCSV("Resource/MapCSV/AGS2026MAP.csv");

	player = std::make_unique<Player>();
	player->SystemInit();
}

SceneGame::~SceneGame()
{

}

void SceneGame::Update()
{
	player->Update();

	if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
	{
		SetNextScene(SceneID::RESULT);
		isEnd = true;
	}
	// Stageの更新
	if (stage_) stage_->Update();
}

void SceneGame::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x00aa00, true);

	// Stageを描画
	if (stage_) stage_->Draw();

	DrawString(0, 20, "GAME SCENE", 0xffffff);
	player->Draw();
}