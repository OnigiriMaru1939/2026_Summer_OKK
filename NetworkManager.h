#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include <memory>
#include "Protocol.h"

class Player;
class RemotePlayer;
class EnemyBase;
class SceneGame;
class ScenePause;

class NetworkManager
{
private:
	int udpSocket = -1;
	IPDATA targetIP;
	int targetPort = 0;
	bool isConnected = false;
	int receivedNextScene = -1;
	float receivedClearTime = 0.0f;
public:
	NetworkManager() = default;
	~NetworkManager();

	// 初期化（ホストかクライアントかを指定）
	// デフォルト"127.0.0.1"
	bool Initialize(bool isHost, const std::string& targetIPString);

	// 送信系メソッド
	void SendPlayerState(const PlayerPacket& packet);
	void SendSodaJump(const SodaJumpPacket& packet);
	void SendStageSelect(int stageIndex);
	void SendEnemyState(const EnemyPacket& packet);
	void SendEnemyDeath(const EnemyDeathPacket& packet);
	void SendBossEvent(const BossEventPacket& packet);
	void SendBossRequest(const BossRequestPacket& packet);
	void SendDamage(const DamagePacket& packet);
	void SendHitConfirmed(const HitConfirmedPacket& packet);
	void SendChangeScene(const ChangeScenePacket& packet);
	void SendSystem(const SystemPacket& packet);
	void SendTime(const TimePacket& packet);

	// 受信系（ここでパケットを振り分ける）
	// ループ内で呼び出し、届いたパケットに応じて各オブジェクトを更新する
	void ReceiveData(RemotePlayer* remotePlayer = nullptr, std::vector<std::shared_ptr<EnemyBase>>* enemyList = nullptr, SceneGame* sceneGame = nullptr);
	void ReceivePauseData(ScenePause* pauseScene);
	// ステージインデックスの同期用関数
	bool ReceiveStageSelect(int& outStageIndex);
	// シーンチェンジの同期用関数
	bool ReceiveChangeScene(int& outNextScene);

	bool IsConnected() const { return isConnected; }
};