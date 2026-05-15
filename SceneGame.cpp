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
	player = std::make_unique<Player>(fileMng, stage_.get());
  player->SystemInit();
	player->SetImage("Resource/Image/Monster.png");

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

	//ゲージの描画
	DrawGauge(20, 50, 500, 40, player_->playerHp, player_->playerHpMax, GetColor(0, 255, 0), 0);		//プレイヤーのHPゲージ

	//炭酸蓄積ゲージが0以上の場合のみ描画
	if (player_->sodaShakeGauge > 0)
	{
		DrawGauge(player_->posX - 100, player_->posY - 50, 20, 100, player_->sodaShakeGauge, player_->sodaShakeGaugeMax, GetColor(0, 0, 255), 1);		//炭酸蓄積ゲージ
	}
}

//横ゲージの描画
void SceneGame::DrawGauge(
	int x,
	int y,
	int width,
	int height,
	float value,
	float maxValue,
	int color,
	int mode)			//mode 0:横ゲージ、1:縦ゲージ
{
	//ゲージの枠
	DrawBox(x - 1,
			y - 1,
			x + width + 1,
			y + height + 1,
			GetColor(255, 0, 0),
			FALSE
	);

	if (mode == 0)
	{
		//横ゲージ割合
		int barWidth = (int)((value / maxValue) * width);

		//中身
		DrawBox(
			x,
			y,
			x + barWidth,
			y + height,
			color,
			TRUE
		);
	}
	else if (mode == 1)
	{
		//縦ゲージ割合
		int barHeight = (int)((value / maxValue) * height);
		//中身
		DrawBox(
			x,
			y + height - barHeight,
			x + width,
			y + height,
			color,
			TRUE
		);
	}
}