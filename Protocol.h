#pragma once
#include <DxLib.h>
#include <iostream>
#include <fstream>
#include <string>

// 設定を読み込むシンプルな関数
static std::string GetConfigValue(const std::string& key)
{
	std::ifstream file("ip/config.json");
	if (!file.is_open()) return "127.0.0.1"; // ファイルがない場合はローカル

	std::string line;
	while (std::getline(file, line))
	{
		// キーが含まれている行を探す
		size_t keyPos = line.find("\"" + key + "\"");
		if (keyPos != std::string::npos)
		{
			// キーの後の ":" と最初の '"' を探す
			size_t colonPos = line.find(':', keyPos);
			size_t startQuote = line.find('"', colonPos);
			size_t endQuote = line.find('"', startQuote + 1);

			if (startQuote != std::string::npos && endQuote != std::string::npos)
			{
				// " " で囲まれた部分を返す
				return line.substr(startQuote + 1, endQuote - startQuote - 1);
			}
		}
	}
	return "127.0.0.1"; // 見つからない場合
}



// 通信パケットの種類
enum PacketType
{
	PACKET_SYNC_PLAYER = 1, // プレイヤー位置・状態同期
	PACKET_SODA = 2,       // 攻撃（ソーダジャンプイベント）
	PACKET_SYNC_ENEMY = 3,
	PACKET_ENEMY_DEATH = 4,
	PACKET_SYNC_EVENT = 5,
	PACKET_REQUEST_BOSS_START = 6, // クライアントからの要求
	PACKET_APPLY_DAMAGE = 7,
	PACKET_HIT_CONFIRMED = 8,
	PACKET_CHANGE_SCENE = 9,
};

struct ChangeScenePacket
{
	int type;            // PACKET_CHANGE_SCENE
	int nextScene;     // 遷移先のID (SceneSuper::NextScene)
};

struct HitConfirmedPacket
{
	int type = PACKET_HIT_CONFIRMED;
	int enemyID;
	float enemyX;
	float enemyY;
};

struct DamagePacket
{
	int type; // PACKET_APPLY_DAMAGE
	int enemyID;
	int damage;
};


struct BossRequestPacket
{
	int type; // PACKET_REQUEST_BOSS_START
};

// プレイヤー状態同期用（毎フレーム、あるいは数フレームに1回送る）
struct PlayerPacket
{
	int type;           // PACKET_SYNC_PLAYER
	float posX, posY;   // 位置
	float vx, vy;       // 速度（補完用）
	float angle;         // 向き（補完用）
	bool isAttack;      // 攻撃中か
	// ※必要に応じてHPなども追加可能
};

struct EnemyPacket
{
	int type;			// PACKET_SYNC_ENEMY
	int enemyID;
	int enemyType;
	float posX, posY;
	int hp;
	bool isAlive;
	int noDamageTime;
};

struct EnemyDeathPacket
{
	int type;           // PACKET_ENEMY_DEATH
	int enemyID;
};

struct BossEventPacket
{
	int type;			// PACKET_SYNC_EVENT
	int eventState;
};

// 攻撃イベント用（押した瞬間に送る）
struct SodaJumpPacket
{
	int type;           // PACKET_SHOOT
	float startX, startY;
	float angle;
	float sodaRatio;
};