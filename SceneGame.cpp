#include "SceneGame.h"
#include "InputManager.h"
#include "Player.h"
#include "Stage.h"
#include "FileManager.h"
#include "StageConfig.h"
#include "StageConfigTablle.h"

// 静的変数の定義
int SceneGame::selectedStageIndex_ = 1;

SceneGame::SceneGame(FileManager& fileMng) : SceneSuper(fileMng)
{

	stage_ = std::make_unique<Stage>(fileMng);
	player_ = std::make_unique<Player>(fileMng, stage_.get());
    player_->SystemInit();
	player_->SetImage("Resource/Image/Monster.png");

  // ステージ固有のセットアップを実行
	const auto& stageConfigs = GetStageConfigs();
	const StageConfig& config = stageConfigs[selectedStageIndex_ - 1];
	stage_->Load(config.mapPath);
	config.setUpStage(*this);

	InputManager::GetInstance().SetTriggerCallback(ActionID::Jump, [this]() { player_->SpaceJump(); });
	InputManager::GetInstance().SetPressCallback(ActionID::Rotate, [this]() { player_->Rotate(); });
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

	player_->Update();

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
	player_->Draw();




}

