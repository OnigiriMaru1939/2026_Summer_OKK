#define NOMINMAX
#include "SceneGame.h"
#include "InputManager.h"
#include "ParticleManager.h"
#include "Player.h"
#include "EnemyBase.h"
#include "Enemy1.h"
#include "Enemy2.h"
#include "Enemy3.h"
#include "Boss1.h"
#include "Boss2.h"
#include "Boss3.h"
#include "ItemBase.h"
#include "Mentos.h"
#include "Stage.h"
#include "FileManager.h"
#include "StageConfig.h"
#include "StageConfigTablle.h"
#include "SceneManager.h"
#include "SceneResult.h"
#include "Fonts.h"
#include "GimmickBase.h"
#include "GimmickTeleport.h"
#include <algorithm>

// 静的変数の定義
int SceneGame::selectedStageIndex_ = 1;

SceneGame::SceneGame(FileManager& fileMng, SceneManager& sceneMng, bool isHost, const std::string& ip) : SceneSuper(fileMng, sceneMng), isHost_(isHost), 
networkManager_(sceneMng.GetNetworkManager())
{
	_bossNameFont = fileMng.CreateFontFM(Fonts::DotGothic16::PATH,
										 Fonts::DotGothic16::NAME,
										 60);
	_warningFont = fileMng.CreateFontFM(Fonts::DotGothic16::PATH,
										 Fonts::DotGothic16::NAME,
										 160);
	_PauseImg = fileMng.LoadImageFM("Resource/Image/Game/Pause.png");

	_pMng = std::make_unique<ParticleManager>(fileMng);

	stage_ = std::make_unique<Stage>(fileMng);
	player_ = std::make_unique<Player>(fileMng, *stage_, *this, *_pMng, networkManager_);
	remotePlayer_ = std::make_shared<RemotePlayer>(fileMng, *_pMng);

	//ステージ固有のセットアップを実行
	const auto& stageConfigs = GetStageConfigs();
	const StageConfig& config = stageConfigs[selectedStageIndex_ - 1];
	stage_->Load(config.mapPath);
	config.setUpStage(*this);

	bossEventState = BossEventState::NONE;
	isBossSpawned_ = false;

	bossTimer = 0;
	clearTime = 0.0f;

	_isClear = false;

	_gameScreen = fileMng.CreateScreenFM("gameScreen", Application::SCREEN_WID, Application::SCREEN_HIG, true);
	_offScreen = fileMng.CreateScreenFM("offScreen", Application::SCREEN_WID, Application::SCREEN_HIG, true);

	highBrightScreen = fileMng.CreateScreenFM("highBrightScreen", Application::SCREEN_WID, Application::SCREEN_HIG, false);
	downScaleScreen = fileMng.CreateScreenFM("downScaleScreen", DOWN_SCALE_SCREEN_W, DOWN_SCALE_SCREEN_H, false);
	gaussScreen = fileMng.CreateScreenFM("gaussScreen", DOWN_SCALE_SCREEN_W, DOWN_SCALE_SCREEN_H, false);

	gaussRatio = 0;
	filterRatio = 200;
	_fadeAlpha = 255.0f;


	InputManager::GetInstance().SetTriggerCallback(ActionID::Pause,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   SystemPacket p;
														   p.type = PACKET_PAUSE;
														   networkManager_.SendSystem(p);
														   sceneMng_.PushScene(SceneID::PAUSE);
													   }
												   });

	sceneMng_.SetTransitionDuration(45.0f);
}

SceneGame::~SceneGame()
{
	enemyList_.clear();
}

void SceneGame::Update()
{
	if (_isBossDefeatedSequence)
	{
		_sequenceTimer++;

		if (_sequenceTimer > 90)
		{
			_isClear = true;
			ClearResult result;
			result.time = clearTime;
			result.stageIndex = selectedStageIndex_;
			sceneMng_.SetTransitionDuration(60.0f);
			sceneMng_.SetClearResult(result);
			sceneMng_.SetGameResult(_isClear); // クリア
			SetNextScene(SceneID::RESULT);
			isEnd = true;
		}
		_pMng->UpdateAll();
		return;
	}
	clearTime += 1.0f / 60.0f; // クリアタイムの更新

	networkManager_.ReceiveData(remotePlayer_.get(), &enemyList_, this);

	int outNextScene;
	if (networkManager_.ReceiveChangeScene(outNextScene) && outNextScene == 4)
	{
		_isClear = false;
		ClearResult result;
		result.time = 0.0f;
		result.stageIndex = selectedStageIndex_;
		sceneMng_.SetTransitionDuration(45.0f);
		sceneMng_.SetClearResult(result);
		sceneMng_.SetGameResult(_isClear); // ゲームオーバー
		SetNextScene(SceneID::RESULT);
		isEnd = true;
	}

	if (remotePlayer_->IsActive())
	{
		// リモートプレイヤーの更新
		remotePlayer_->Update();
	}


	// プレイヤーの更新
	UpdatePlayer();

	PlayerPacket p;
	p.type = PACKET_SYNC_PLAYER;
	p.posX = player_->GetWorldX();
	p.posY = player_->GetWorldY();
	p.vx = player_->GetVX();
	p.vy = player_->GetVY();
	p.angle = player_->GetAngle();
	p.isAttack = player_->GetAliveFlag();
	networkManager_.SendPlayerState(p);

	//敵の更新
	UpdateEnemy();

	if (isHost_)
	{
		for (auto& enemy : enemyList_)
		{
			if (enemy && enemy->IsAlive())
			{
				EnemyPacket ep;
				ep.type = PACKET_SYNC_ENEMY;
				ep.enemyID = enemy->GetNetworkId();
				ep.enemyType = static_cast<int>(enemy->GetEnemyType());
				ep.posX = enemy->GetX();
				ep.posY = enemy->GetY();
				ep.hp = enemy->GetHp();
				ep.isAlive = enemy->IsAlive();
				ep.noDamageTime = enemy->GetNoDamageTime();
				networkManager_.SendEnemyState(ep);
			}
		}
	}

	//敵の弾の更新
	UpdateEnemyShot();

	// ギミックの更新（必要に応じて）やリストのクリーンアップ
	for (auto& gimmick : gimmickList_)
	{
		// 必要なら gimmick->Update() など
	}

	// 死んだギミック（取得済みアイテムなど）を削除
	gimmickList_.erase(
		std::remove_if(gimmickList_.begin(), gimmickList_.end(),
					   [](const auto& g) { return !g->IsAlive(); }),
		gimmickList_.end()
	);

	//ギミックの更新
	UpdateGimmick();
	//アイテムの更新
	UpdateItem();

	//ボスの生成判定
	CheckBossSpawn();
	//ボスイベントの処理
	BossEvent();
	// ボスイベントの状態に応じた制限
	if (bossEventState == BossEventState::WARNING)
	{
		player_->SetCanMoveFlag(false);
	}
	//プレイヤーと敵の衝突判定
	CheckPlayerEnemyCollision();
	//プレイヤーとギミックの衝突判定
	CheckPlayerGimmickCollision();
	//プレイヤーとアイテムの衝突判定
	CheckPlayerItemCollision();
	//プレイヤーと敵の弾の衝突判定
	CheckPlayerEnemyShotCollision();

	_pMng->UpdateAll();
	// Stageの更新
	UpdateStage();
}

void SceneGame::Draw()
{
	SetDrawScreen(_gameScreen->GetHandle());
	ClearDrawScreen();
	// 様々な画面エフェクトを受けるものはこっちに描画
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x00aa00, true);

	// Stageを描画
	if (stage_) stage_->Draw();

#ifdef _DEBUG
	//ボスエリアを描画
	RECT area = bossArea;

	DrawBox(area.left - stage_->GetScrollX(),
			area.top - stage_->GetScrollY(),
			area.right - stage_->GetScrollX(),
			area.bottom - stage_->GetScrollY(),
			0xff0000, false);
#endif

	for (auto& t : _tutorialPanelList)
	{
		DrawGraph(static_cast<int>(t.x) - stage_->GetScrollX(), static_cast<int>(t.y) - stage_->GetScrollY(), t.path->GetHandle(), true);
	}
	_pMng->DrawAll(stage_->GetScrollX(), stage_->GetScrollY());

	for (auto& gimmick : gimmickList_) { gimmick->Draw(); }
	//敵を描画
	for (auto& enemy : enemyList_)
	{
		enemy->Draw();
	}

	//敵の弾を描画
	for (auto& bullet : bulletList_)
	{
		bullet->Draw();
	}

	//プレイヤーを描画
	player_->Draw();
	if (remotePlayer_->IsActive())
	{
		remotePlayer_->Draw(stage_->GetScrollX(), stage_->GetScrollY());
	}
	//アイテムを描画
	for (auto& item : itemList_)
	{
		item->Draw();
	}

	DrawClearTransition();

	SetDrawScreen(_offScreen->GetHandle());
	ClearDrawScreen();

	// 画面揺れなどの影響を受けないものはこっちへ描画

	//ボスイベントの描画
	BossEventDraw();

	SetDrawScreen(DX_SCREEN_BACK);
	if (_isBossDefeatedSequence)
	{
		float t = std::min(1.0f, _sequenceTimer / 90.0f);
		float easeT = t * t * (3.0f - 2.0f * t); // Clamp


		float targetScale = 2.0f;
		float scale = 1.0f + (targetScale - 1.0f) * easeT;

		int shakeX = 0, shakeY = 0;
		if (_sequenceTimer < 150)
		{
			shakeX = GetRand(_sequenceTimer) - _sequenceTimer / 2;
			shakeY = GetRand(_sequenceTimer) - _sequenceTimer / 2;
		}

		RECT pRect = player_->GetRect();
		// 目標注視点
		float targetFocusX = static_cast<float>(pRect.left + (pRect.right - pRect.left) / 2 - stage_->GetScrollX());
		float targetFocusY = static_cast<float>(pRect.top + (pRect.bottom - pRect.top) / 2 - stage_->GetScrollY());
		
		// 初期注視点
		float startFocusX = Application::SCREEN_WID / 2;
		float startFocusY = Application::SCREEN_HIG / 2;

		// 現在の注視点
		float currentFocusX = startFocusX + (targetFocusX - startFocusX) * easeT;
		float currentFocusY = startFocusY + (targetFocusY - startFocusY) * easeT;

		float minFocusX = Application::SCREEN_WID / (2.0f * scale);
		float maxFocusX = Application::SCREEN_WID - minFocusX;
		float minFocusY = Application::SCREEN_HIG / (2.0f * scale);
		float maxFocusY = Application::SCREEN_HIG - minFocusY;

		currentFocusX = std::clamp(currentFocusX, minFocusX, maxFocusX);
		currentFocusY = std::clamp(currentFocusY, minFocusY, maxFocusY);

		DrawRotaGraph3(Application::SCREEN_WID / 2 + shakeX,
					   Application::SCREEN_HIG / 2 + shakeY,
					   static_cast<int>(currentFocusX),
					   static_cast<int>(currentFocusY), scale, scale, 0.0f, _gameScreen->GetHandle(), TRUE);
	}
	else
	{
		DrawGraph(0, 0, _gameScreen->GetHandle(), false);
	}

	if (CollisionPauseImg())
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
	}
	DrawGraph(0, 0, _PauseImg->GetHandle(), true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	
	DrawGraph(0, 0, _offScreen->GetHandle(), TRUE);
#ifdef _DEBUG
	DrawFormatString(500, 500, 0x00ff00, isHost_ ? "Host" : "not Host");
#endif
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeAlpha));
	if (IsClear())
	{
		DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0xffffff, true);
	}
	else
	{
		DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

//ボスイベントの描画
void SceneGame::BossEventDraw()
{
	auto boss = GetBoss();
	int strWidth = 0;

	if (boss)
	{
		strWidth = GetDrawStringWidthToHandle(
			boss->GetName().c_str(),
			strlen(boss->GetName().c_str()),
			_bossNameFont->GetHandle()
		);
	}

	if (bossEventState == BossEventState::UIDRAW)
	{
		if (boss)
		{
			DrawStringToHandle(
				(Application::SCREEN_WID - strWidth) / 2,
				20,
				boss->GetName().c_str(),
				GetColor(255, 255, 255),
				_bossNameFont->GetHandle()
			);
		}
		DrawGauge(100, 90, 1720, 50, bossHpGauge, bossHpGaugeMax, GetColor(255, 0, 0));
	}

	if (bossEventState == BossEventState::BATTLE)
	{
		if (boss)
		{
			DrawStringToHandle(
				(Application::SCREEN_WID - strWidth) / 2,
				20,
				boss->GetName().c_str(),
				GetColor(255, 255, 255),
				_bossNameFont->GetHandle()
			);

			DrawGauge(100, 90, 1720, 50, boss->GetHp(), boss->GetHpMax(), GetColor(255, 0, 0));
		}
	}

	if (bossEventState == BossEventState::WARNING)
	{
		if (bossTimer > 0)
		{
			if ((bossTimer / 10) % 6 == 0) return;
		}

		DrawBox(
			0,
			Application::SCREEN_HIG / 2 - 100,
			Application::SCREEN_WID,
			Application::SCREEN_HIG / 2 + 100,
			GetColor(255, 0, 0),
			true);

		strWidth = GetDrawStringWidthToHandle(
			"WARNING !!",
			strlen("WARNING !!"),
			_warningFont->GetHandle()
		);
		DrawStringToHandle(
			(Application::SCREEN_WID - strWidth) / 2,
			(Application::SCREEN_HIG - GetFontSizeToHandle(_warningFont->GetHandle())) / 2,
			"WARNING !!",
			GetColor(255, 255, 255),
			_warningFont->GetHandle()
			);
	}
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
	DrawBox(
		x - 1,
		y - 1,
		x + width + 1,
		y + height + 1,
		GetColor(255, 255, 255),
		FALSE
	);
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

void SceneGame::DrawClearTransition()
{
	if (IsClear())
	{
		// 描画結果から高輝度部分のみを抜き出した画像を得る
		GraphFilterBlt(_gameScreen->GetHandle(), highBrightScreen->GetHandle(), DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_LESS, filterRatio, TRUE, GetColor(0, 0, 0), 255);
		// 高輝度部分を８分の１に縮小した画像を得る
		GraphFilterBlt(highBrightScreen->GetHandle(), downScaleScreen->GetHandle(), DX_GRAPH_FILTER_DOWN_SCALE, DOWN_SCALE);
		// ８分の１に縮小した画像をガウスフィルタでぼかす
		GraphFilterBlt(downScaleScreen->GetHandle(), gaussScreen->GetHandle(), DX_GRAPH_FILTER_GAUSS, 16, gaussRatio);
		// 描画モードをバイリニアフィルタリングにする(拡大したときにドットがぼやけるようにする)
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		// 描画ブレンドモードを加算にする
		SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
		// 高輝度部分を縮小してぼかした画像を画面いっぱいに数回描画する( 数回描画するのはより明るくみえるようにするため )
		DrawExtendGraph(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, gaussScreen->GetHandle(), FALSE);
		DrawExtendGraph(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, gaussScreen->GetHandle(), FALSE);
		// 描画ブレンドモードをブレンド無しに戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		// 描画モードを二アレストに戻す
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}
}

//プレイヤーの更新
void SceneGame::UpdatePlayer()
{
	player_->Update();
	if (!player_->GetAliveFlag())
	{
		_isClear = false;
		ClearResult result;
		result.time = 0.0f;
		result.stageIndex = selectedStageIndex_;
		sceneMng_.SetTransitionDuration(45.0f);
		sceneMng_.SetClearResult(result);
		sceneMng_.SetGameResult(_isClear); // ゲームオーバー
		SetNextScene(SceneID::RESULT);
		isEnd = true;

		ChangeScenePacket csp;
		csp.type = PACKET_CHANGE_SCENE;
		csp.nextScene = static_cast<int>(NextScene::Result);
		networkManager_.SendChangeScene(csp);
	}
}

//敵の更新
void SceneGame::UpdateEnemy()
{
	for (auto& enemy : enemyList_)
	{
		if (enemy->IsAlive())
		{
			if (isHost_)
			{
				enemy->Update();
			}
		}
		if (!enemy->IsAlive())
		{
			if (isHost_)
			{
				EnemyDeathPacket edp;
				edp.type = PACKET_ENEMY_DEATH;
				edp.enemyID = enemy->GetNetworkId();
				networkManager_.SendEnemyDeath(edp);
			}
			// 敵が死んでいる場合は演出をはさんでリストから削除
			enemy->KillEffect();
			// ボスを倒したらトランジション
			if (std::dynamic_pointer_cast<IBoss>(enemy) && !_isBossDefeatedSequence)
			{
				_isBossDefeatedSequence = true;
				_sequenceTimer = 0;
			}
			else
			{
				enemy.reset(); // shared_ptrをメモリ解放+nullptrに設定
			}
		}
	}

	// nullPtrを削除
	enemyList_.erase(
		std::remove(enemyList_.begin(), enemyList_.end(), nullptr),
		enemyList_.end()
	);
 }

//敵の弾の更新
void SceneGame::UpdateEnemyShot()
{
	for (auto& shot : bulletList_)
	{
		if (shot->IsAlive())
		{
			shot->Update();
		}
		else
		{
			//敵の弾がなくなったらリストから削除
			shot = nullptr;
		}
	}
	bulletList_.erase(
		std::remove(bulletList_.begin(), bulletList_.end(), nullptr), 
		bulletList_.end());
}

//ステージの更新
void SceneGame::UpdateStage()
{
	if (stage_) stage_->Update();
}

void SceneGame::UpdateGimmick()
{
	for (auto& gimmick : gimmickList_)
	{
		gimmick->Update();
	}
}

//アイテムの更新
void SceneGame::UpdateItem()
{
	for (auto& item : itemList_)
	{
		if (item->IsAlive())
		{
			item->Update();
		}
		else
		{
			//アイテムがなくなったらリストから削除
			item = nullptr;
		}
	}
	itemList_.erase(std::remove(itemList_.begin(), itemList_.end(), nullptr), itemList_.end());
}

void SceneGame::UpdateDuringTransition()
{
	// プレイヤーの更新
	player_->UpdateStageScroll();
	//UpdatePlayer();
	_pMng->UpdateAll();
	// Stageの更新
	UpdateStage();

}

//プレイヤーと敵の衝突判定
void SceneGame::CheckPlayerEnemyCollision()
{

	for (auto& enemy : enemyList_)
	{
		if (!enemy->IsAlive()) continue;
		RECT p = player_->GetRect();
		RECT e = enemy->GetRect();

		bool hit =
			p.right > e.left &&
			p.left < e.right &&
			p.bottom > e.top &&
			p.top < e.bottom;

		if (!hit)
		{
			enemy->SetHitPlayerAlready(false);
			continue;
		}
		if (!player_->GetAttakFlag())
		{
			//ダメージ・ノックバック
			player_->Damage(10.0f);
			player_->PlayerKnockBack(enemy->GetX(), enemy->GetY(), 10.0f);
		}
		else if (player_->GetAttakFlag() && player_->GetSpeed() >= Player::ATTACK_SPEED_THRESHOLD)
		{
			if (isHost_)
			{
				enemy->ApplyDamage(static_cast<int>(player_->GetAttackDamage()));
				if (enemy->IsAlive())
				{
					player_->PlayerKnockBack(enemy->GetX(), enemy->GetY(), 10.0f);
				}
			}
			else
			{
				DamagePacket dp;
				dp.type = PACKET_APPLY_DAMAGE;
				dp.enemyID = enemy->GetNetworkId();
				dp.damage = static_cast<int>(player_->GetAttackDamage());
				networkManager_.SendDamage(dp);
			}
		}

		if (!enemy->GetHitPlayerAlready())
		{
			player_->PlayHitSE();
			enemy->SetHitPlayerAlready(true);
		}
	}
}

void SceneGame::ApplyDamageToEnemy(int enemyID, int damage)
{
	for (auto& enemy : enemyList_)
	{
		if (enemy && enemy->GetNetworkId() == enemyID)
		{
			enemy->ApplyDamage(damage);
			// ダメージを与えたら、自動的に次のフレームの同期で 
			// 敵のHPがクライアントに伝わります
			if (enemy->IsAlive())
			{
				// ホストからクライアントへヒット通知
				HitConfirmedPacket hcp;
				hcp.type = PACKET_HIT_CONFIRMED;
				hcp.enemyID = enemyID;
				hcp.enemyX = enemy->GetX();
				hcp.enemyY = enemy->GetY();
				networkManager_.SendHitConfirmed(hcp);
			}
			break;
		}
	}
}

//プレイヤーとギミックの衝突判定
void SceneGame::CheckPlayerGimmickCollision()
{
	if (!player_->GetAliveFlag()) return;

	RECT p = player_->GetRect();

	for (auto& gimmick : gimmickList_)
	{
		if (!gimmick->IsAlive()) continue;

		RECT g = gimmick->GetRect();

		// 矩形交差判定
		bool hit =
			p.right > g.left &&
			p.left < g.right &&
			p.bottom > g.top &&
			p.top < g.bottom;

		if (hit)
		{
			// ★ ポリモーフィズムにより、型に応じた OnCollidePlayer が自動で動く！
			gimmick->OnCollidePlayer(*player_);
		}
	}
}

//プレイヤーとアイテムの衝突判定
void SceneGame::CheckPlayerItemCollision()
{
	RECT p = player_->GetRect();

	for (auto& item : itemList_)
	{
		if (!item->IsAlive())
			continue;

		RECT i = item->GetRect();

		bool hit =
			p.right > i.left &&
			p.left < i.right &&
			p.bottom > i.top &&
			p.top < i.bottom;

		if (hit)
		{
			item->OnGet(*player_);
		}
	}
}

//プレイヤーと敵の弾の衝突判定
void SceneGame::CheckPlayerEnemyShotCollision()
{
	RECT p = player_->GetRect();

	for (auto& bullet : bulletList_)
	{
		if (!bullet->IsAlive())
			continue;

		RECT b = bullet->GetRect();

		bool hit =
			p.right > b.left &&
			p.left < b.right &&
			p.bottom > b.top &&
			p.top < b.bottom;
		if (hit)
		{
			player_->Damage(bullet->GetApplyDamage());
			player_->PlayerKnockBack(bullet->GetX(), bullet->GetY(), 10.0f);
			bullet->SetAliveFlag(false);
		}
	}
}

//ボスエリアの矩形を設定
void SceneGame::SetBossArea(int left, int top, int right, int bottom)
{
	// ボスエリアの矩形を定義
	bossArea.left = left;
	bossArea.top = top;
	bossArea.right = right;
	bossArea.bottom = bottom;
}

//ボスエリアの矩形を取得
RECT SceneGame::GetBossArea() const
{
	return bossArea;
}

//ボスの生成判定
void SceneGame::CheckBossSpawn()
{
	if (isBossSpawned_) return;

	bossArea = GetBossArea();

	float x = player_->GetX();
	float y = player_->GetY();

	// プレイヤーがボスエリアに入ったかどうかの判定
	bool isEnterBossArea = (x >= bossArea.left - stage_->GetScrollX() &&
							x <= bossArea.right - stage_->GetScrollX() &&
							y >= bossArea.top - stage_->GetScrollY() &&
							y <= bossArea.bottom - stage_->GetScrollY());

	if (isEnterBossArea)
	{
		if (isHost_)
		{
			StartBossEvent();
		}
		else
		{
			// クライアントはホストに開始を通知
			BossRequestPacket req;
			req.type = PACKET_REQUEST_BOSS_START;
			networkManager_.SendBossRequest(req);
		}
	}
}

std::shared_ptr<EnemyBase> SceneGame::GetBoss()
{
	for (auto& enemy : enemyList_)
	{
		switch (enemy->GetEnemyType())
		{
			case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1:
			case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_2:
			case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_3:
				return enemy;

			default:
				break;
		}
	}

	return nullptr;
}

//ボスイベント処理
void SceneGame::BossEvent()
{
	auto boss = GetBoss();

	if (isHost_)
	{
		BossEventState previousState = bossEventState;

		switch (bossEventState)
		{
			case SceneGame::BossEventState::NONE:
				break;

			case SceneGame::BossEventState::WARNING:
				bossTimer++;
				if (bossTimer > 180)
				{
					switch (selectedStageIndex_)
					{
						case 1:
							AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1,1800.0f,3300.0f);
							break;
						case 2:
							AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_2,950.0f,5000.0f);
							break;
						case 3:
							AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_3,1000.0f,6000.0f);
							break;
					}
					bossEventState = BossEventState::APPEAR;
					bossTimer = 0;
				}
				break;

			case SceneGame::BossEventState::APPEAR:
				if (boss)
				{
					boss->SetAppearFlag(true);
				}
				bossTimer++;
				//2秒後にUIを表示
				if (bossTimer > 120)
				{
					bossEventState = BossEventState::UIDRAW;

					if (boss)
					{
						bossHpGauge = 0;
						bossHpGaugeMax = boss->GetHpMax();
					}
					bossTimer = 0;
				}
				break;

			case SceneGame::BossEventState::UIDRAW:
				//HPバーを徐々に表示させる
				if (bossHpGauge < bossHpGaugeMax)
				{
					float addHp = bossHpGaugeMax / HP_GAUGE_ANIM_TIME;
					bossHpGauge += addHp;
				}

				if (bossHpGauge >= bossHpGaugeMax)
				{
					bossHpGauge = bossHpGaugeMax;
					//player_->SetCanMoveFlag(true);

					if (boss)
					{
						boss->SetAppearFlag(false);
					}

					player_->SetCanMoveFlag(true);

					bossEventState = BossEventState::BATTLE;
				}

				break;

			case SceneGame::BossEventState::BATTLE:
				break;

			default:
				break;
		}

		if (previousState != bossEventState)
		{
			BossEventPacket packet;
			packet.type = PACKET_SYNC_EVENT;
			packet.eventState = static_cast<int>(bossEventState);
			networkManager_.SendBossEvent(packet);
		}
	}
	else
	{
		int hostEventState = remotePlayer_->GetBossEventState();
		BossEventState newBossState = static_cast<BossEventState>(hostEventState);

		if (bossEventState != newBossState)
		{
			switch (newBossState)
			{
				case SceneGame::BossEventState::WARNING:
					bossTimer = 0;
					Teleport2BossArea();
					break;
				case SceneGame::BossEventState::APPEAR:
					if (boss)
					{
						boss->SetAppearFlag(true);
					}
					switch (selectedStageIndex_)
					{
						case 1:
							AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1, 1800.0f,200.0f);
							break;
						case 2:
							AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_2, 950.0f,5000.0f);
							break;
						case 3:
							AddBoss(EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_3,1000.0f,6000.0f);
							break;
					}
					bossTimer = 0;
					break;
				case SceneGame::BossEventState::UIDRAW:
					// ホストがUIDRAWに移行したタイミングでHPゲージの準備
					boss = GetBoss(); // 生成された直後なので再取得
					if (boss)
					{
						bossHpGauge = 0;
						bossHpGaugeMax = boss->GetHpMax();
					}
					bossTimer = 0;
					break;

				case SceneGame::BossEventState::BATTLE:
					// ホストがBATTLEに移行したタイミング
					bossHpGauge = bossHpGaugeMax; // 念のためMAXで固定
					//player_->SetCanMoveFlag(true);
					boss = GetBoss();
					if (boss)
					{
						boss->SetAppearFlag(false);
					}
					player_->SetCanMoveFlag(true);
					break;
			}
			bossEventState = newBossState;
		}

		if (bossEventState == SceneGame::BossEventState::WARNING || bossEventState == SceneGame::BossEventState::APPEAR)
		{
			bossTimer++;
		}
		else if (bossEventState == SceneGame::BossEventState::UIDRAW)
		{
			if (bossHpGauge < bossHpGaugeMax)
			{
				float addHp = bossHpGaugeMax / HP_GAUGE_ANIM_TIME;
				bossHpGauge += addHp;
			}
		}
	}
}

//敵生成関数(雑魚敵)
void SceneGame::AddEnemy(EnemyBase::ENEMY_TYPE type, float x, float y)
{
	std::shared_ptr<EnemyBase> newEnemy = nullptr;

	switch (type)
	{
	case EnemyBase::ENEMY_TYPE::E_TYPE_1:
		newEnemy = std::make_shared<Enemy1>(fileMng_, stage_.get(), this, x, y, *_pMng);
		break;
	case EnemyBase::ENEMY_TYPE::E_TYPE_2:
		newEnemy = std::make_shared<Enemy2>(fileMng_, stage_.get(), this, x, y, *_pMng);
		break;
	case EnemyBase::ENEMY_TYPE::E_TYPE_3:
		newEnemy = std::make_shared<Enemy3>(fileMng_, stage_.get(), this, x, y, *_pMng);
		break;
	default:
		break;
	}

	if (newEnemy)
	{
		newEnemy->SetNetworkId(_nextEnemyId);
		_nextEnemyId++;
		enemyList_.push_back(newEnemy);
	}
}

//敵生成関数(ボス敵)
void SceneGame::AddBoss(EnemyBase::ENEMY_TYPE type, float x, float y)
{
	std::shared_ptr<EnemyBase> newBoss = nullptr;

	switch (type)
	{
	case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_1:
		newBoss = std::make_shared<Boss1>(fileMng_, stage_.get(), this, x, y, *_pMng);
		break;
	case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_2:
		newBoss = std::make_shared<Boss2>(fileMng_, stage_.get(), this, x, y, *_pMng);
		break;
	case EnemyBase::ENEMY_TYPE::E_TYPE_BOSS_3:
		newBoss = std::make_shared<Boss3>(fileMng_, stage_.get(), this, x, y, *_pMng);
		break;
	default:
		break;
	}

	if (newBoss)
	{
		newBoss->SetNetworkId(_nextEnemyId);
		_nextEnemyId++;
		enemyList_.push_back(newBoss);
	}
}

//敵弾生成関数
void SceneGame::AddEnemyShot(BulletBase::BULLET_TYPE type, float x, float y, float vx, float vy, float scale)
{
	switch (type)
	{
		case BulletBase::BULLET_TYPE::B_TYPE_NON:
			break;
		case BulletBase::BULLET_TYPE::B_TYPE_1:
			bulletList_.push_back(std::make_shared<Bullet>(fileMng_, stage_.get(), x, y, vx, vy, scale));
			break;
		case BulletBase::BULLET_TYPE::B_TYPE_2:
			break;
		case BulletBase::BULLET_TYPE::B_TYPE_3:
			break;
		case BulletBase::BULLET_TYPE::B_TYPE_MAX:
			break;
		default:
			break;
	}
}

//テレポートギミック生成関数
void SceneGame::AddTeleport(float x, float y, float targetX, float targetY)
{
	gimmickList_.push_back(std::make_shared<GimmickTeleport>(fileMng_, stage_.get(), x, y, targetX, targetY));
}

//アイテム生成関数
void SceneGame::AddItem(ItemBase::ITEM_TYPE type, float x, float y)
{
	switch (type)
	{
		case ItemBase::ITEM_TYPE::MENTOS:
			itemList_.push_back(std::make_shared<Mentos>(fileMng_, stage_.get(), x, y));
			break;
		case ItemBase::ITEM_TYPE::I_TYPE_MAX:
			break;
		default:
			break;
	}
}

void SceneGame::SetTutorial(std::string path, float x, float y)
{
	tutorialPanel newTutorialPanel;
	newTutorialPanel.path = fileMng_.LoadImageFM(path);
	newTutorialPanel.x = x;
	newTutorialPanel.y = y;

	_tutorialPanelList.push_back(newTutorialPanel);
}

void SceneGame::TransitionOut(float t)
{
	if (IsClear())
	{
		// クリア時の遷移アニメーション
		_fadeAlpha = EaseOutCubic(t) * 255.0f;
		gaussRatio = static_cast<int>(t * 1000);
	}
	else
	{
		_fadeAlpha = EaseOutCubic(t) * 255.0f;
	}
}

void SceneGame::TransitionIn(float t)
{
	float e = EaseInCubic(1 - t);
	_fadeAlpha = e * 255.0f;
}

void SceneGame::StartBossEvent()
{
	if (isBossSpawned_) return;

	bossEventState = BossEventState::WARNING;
	bossTimer = 0;
	//player_->SetCanMoveFlag(false);
	isBossSpawned_ = true;

	// ボスエリアに強制合流させる
	Teleport2BossArea();

	// 全クライアントへ開始命令を同期
	BossEventPacket packet;
	packet.type = PACKET_SYNC_EVENT;
	packet.eventState = static_cast<int>(BossEventState::WARNING);
	networkManager_.SendBossEvent(packet);
}

void SceneGame::Teleport2BossArea()
{
	// 念のためボスエリアの矩形を最新化
	bossArea = GetBossArea();

	float myX = player_->GetWorldX();
	float myY = player_->GetWorldY();

	// 自分がすでにボスエリア内にいるか判定
	bool amIInBossArea = (myX >= bossArea.left && myX <= bossArea.right &&
						  myY >= bossArea.top && myY <= bossArea.bottom);

	if (amIInBossArea)
	{
		// 自分が先に到着した側のためワープはしない
		return;
	}

	// 相手の座標を取得
	float targetX = remotePlayer_->GetPosX();
	float targetY = remotePlayer_->GetPosY();

	player_->SetPosition(targetX, targetY);

	// ジャンプ中などにワープした場合に備えて速度をリセット
	player_->SetVelocity(0.0f, 0.0f);
}

bool SceneGame::CollisionPauseImg()
{
	RECT rc = player_->GetRect();
	rc.left -= stage_->GetScrollX();
	rc.right -= stage_->GetScrollX();
	rc.top -= stage_->GetScrollY();
	rc.bottom -= stage_->GetScrollY();

	RECT pauseRc;
	pauseRc.left = 0;
	pauseRc.top = 0;
	pauseRc.right = _PauseImg->GetWidth();
	pauseRc.bottom = _PauseImg->GetHeight();

	return rc.right  > pauseRc.left &&
		   rc.left   < pauseRc.right &&
		   rc.bottom > pauseRc.top &&
		   rc.top    < pauseRc.bottom;
}
