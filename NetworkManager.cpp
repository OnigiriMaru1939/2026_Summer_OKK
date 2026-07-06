#include "NetworkManager.h"
#include "Player.h"
#include "RemotePlayer.h"
#include "EnemyBase.h"
#include "SceneGame.h"
#include "ScenePause.h"

NetworkManager::~NetworkManager()
{
	if (udpSocket != -1)
	{
		DeleteUDPSocket(udpSocket);
		udpSocket = -1;
	}
}


bool NetworkManager::Initialize(bool isHost, const std::string& targetIPString)
{
	if (udpSocket != -1) return true;

	// ホストは8080で待ち受け、クライアントは8081で待ち受ける例
	int myPort = isHost ? 8080 : 8081;
	targetPort = isHost ? 8081 : 8080;

	// ソケット作成
	udpSocket = MakeUDPSocket(myPort);
	if (udpSocket == -1) return false;

	// 送信先IPの解析（デフォルト"127.0.0.1"）
	int d1 = 127, d2 = 0, d3 = 0, d4 = 1;
	if (!targetIPString.empty())
	{
		sscanf_s(targetIPString.c_str(), "%d.%d.%d.%d", &d1, &d2, &d3, &d4);
	}
	targetIP.d1 = d1;
	targetIP.d2 = d2;
	targetIP.d3 = d3;
	targetIP.d4 = d4;

	isConnected = true;
	return true;
}

void NetworkManager::SendPlayerState(const PlayerPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(PlayerPacket));
}

void NetworkManager::SendSodaJump(const SodaJumpPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(SodaJumpPacket));
}

void NetworkManager::SendEnemyState(const EnemyPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(EnemyPacket));
}

void NetworkManager::SendEnemyDeath(const EnemyDeathPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(EnemyDeathPacket));
}

void NetworkManager::SendBossEvent(const BossEventPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(BossEventPacket));
}

void NetworkManager::SendBossRequest(const BossRequestPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(BossRequestPacket));
}

void NetworkManager::SendDamage(const DamagePacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(DamagePacket));
}

void NetworkManager::SendHitConfirmed(const HitConfirmedPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(HitConfirmedPacket));
}

void NetworkManager::SendStageSelect(int stageIndex)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&stageIndex, sizeof(int));
}

void NetworkManager::SendChangeScene(const ChangeScenePacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(ChangeScenePacket));
}

void NetworkManager::SendSystem(const SystemPacket& packet)
{
	if (udpSocket == -1) return;
	NetWorkSendUDP(udpSocket, targetIP, targetPort, (void*)&packet, sizeof(SystemPacket));
}

void NetworkManager::ReceiveData(RemotePlayer* remotePlayer, std::vector<std::shared_ptr<EnemyBase>>* enemyList, SceneGame* sceneGame)
{
	if (udpSocket == -1) return;

	// 受信キューにデータがある限りループ
	while (CheckNetWorkRecvUDP(udpSocket) == TRUE)
	{
		char buffer[1024]; // 十分なバッファサイズ
		IPDATA senderIP;
		int senderPort;

		int recvLen = NetWorkRecvUDP(udpSocket, &senderIP, &senderPort, buffer, sizeof(buffer), FALSE);

		if (recvLen > 0)
		{
			targetIP = senderIP;
			targetPort = senderPort;
			// 最初の4バイトをPacketTypeとして読み取る
			int type = *reinterpret_cast<int*>(buffer);

			switch (type)
			{
				case PACKET_SYNC_PLAYER:
					if (remotePlayer && recvLen == sizeof(PlayerPacket))
					{
						PlayerPacket* p = reinterpret_cast<PlayerPacket*>(buffer);
						remotePlayer->SyncState(*p);
					}
					break;
				case PACKET_SODA:
					if (remotePlayer && recvLen == sizeof(SodaJumpPacket))
					{
						SodaJumpPacket* p = reinterpret_cast<SodaJumpPacket*>(buffer);
						remotePlayer->SyncState(*p);
					}
					break;
				case PACKET_SYNC_ENEMY:
					if (enemyList && recvLen == sizeof(EnemyPacket))
					{
						EnemyPacket* e = reinterpret_cast<EnemyPacket*>(buffer);
						for (auto& enemy : *enemyList)
						{
							if (enemy && enemy->GetNetworkId() == e->enemyID)
							{
								enemy->SyncNetworkState(e->posX, e->posY, e->hp, e->isAlive, e->noDamageTime);
								break;
							}
						}
					}
					break;
				case PACKET_ENEMY_DEATH:
					if (enemyList && recvLen == sizeof(EnemyDeathPacket))
					{
						EnemyDeathPacket* edp = reinterpret_cast<EnemyDeathPacket*>(buffer);
						for (auto& enemy : *enemyList)
						{
							// IDが一致する敵を強制的にキルする
							if (enemy && enemy->GetNetworkId() == edp->enemyID)
							{
								enemy->ApplyDamage(99999); // ダメージを与えて強制的にisAliveをfalseにする
								break;
							}
						}
					}
					break;
				case PACKET_SYNC_EVENT:
					if (remotePlayer && recvLen == sizeof(BossEventPacket))
					{
						BossEventPacket* bep = reinterpret_cast<BossEventPacket*>(buffer);

						remotePlayer->SetBossEventState(bep->eventState);
					}
					break;
				case PACKET_REQUEST_BOSS_START:
				{
					// ホストのみがこのリクエストを受けてイベントを開始する
					if (sceneGame && sceneGame->IsHost())
					{
						sceneGame->StartBossEvent();
					}
					break;
				}
				case PACKET_APPLY_DAMAGE:
					if (sceneGame && recvLen == sizeof(DamagePacket))
					{
						DamagePacket* dp = reinterpret_cast<DamagePacket*>(buffer);
						// sceneGame に敵を探してダメージを与えるメソッドを実装
						sceneGame->ApplyDamageToEnemy(dp->enemyID, dp->damage);
					}
					break;
					// クライアントへ
				case PACKET_HIT_CONFIRMED:
				{
					if (sceneGame)
					{
						HitConfirmedPacket* hcp = reinterpret_cast<HitConfirmedPacket*>(buffer);
						// ここでクライアント側のプレイヤーをノックバックさせる
						sceneGame->GetPlayer()->PlayerKnockBack(hcp->enemyX, hcp->enemyY, 10.0f);
					}
				}
				break;
				case PACKET_CHANGE_SCENE:
					if (recvLen == sizeof(ChangeScenePacket))
					{
						ChangeScenePacket* csp = reinterpret_cast<ChangeScenePacket*>(buffer);
						receivedNextScene = csp->nextScene;
					}
					break;
				case PACKET_PAUSE:
					if (sceneGame)
					{
						sceneGame->RequestPause();
					}
			}
		}
	}
}

void NetworkManager::ReceivePauseData(ScenePause* pauseScene)
{
	if (udpSocket == -1) return;

	// ポーズ中も常にパケットを受信し続ける
	while (CheckNetWorkRecvUDP(udpSocket) == TRUE)
	{
		char buffer[1024];
		IPDATA senderIP;
		int senderPort;
		int recvLen = NetWorkRecvUDP(udpSocket, &senderIP, &senderPort, buffer, sizeof(buffer), FALSE);

		if (recvLen > 0)
		{
			int type = *reinterpret_cast<int*>(buffer);

			if (type == PACKET_RESUME)
			{
				pauseScene->SyncResume(); // 相手が再開した
			}
			else if (type == PACKET_CHANGE_SCENE)
			{
				ChangeScenePacket* csp = reinterpret_cast<ChangeScenePacket*>(buffer);
				pauseScene->SyncChangeScene(csp->nextScene); // 相手がシーン遷移を選択した
			}
			// ★重要：PACKET_SYNC_PLAYER などのゲーム内パケットは、
			// 何も処理せずに無視する（捨てる）ことでバッファ詰まりを防ぎます。
		}
	}
}

bool NetworkManager::ReceiveStageSelect(int& outStageIndex)
{
	if (udpSocket == -1) return false;

	bool received = false;
	while (CheckNetWorkRecvUDP(udpSocket) == TRUE)
	{
		char buffer[1024];
		IPDATA senderIP;
		int senderPort;

		int recvLen = NetWorkRecvUDP(udpSocket, &senderIP, &senderPort, buffer, sizeof(buffer), FALSE);
		if (recvLen == sizeof(int))
		{
			outStageIndex = *reinterpret_cast<int*>(buffer);
			received = true;
		}
	}
	return received;
}

bool NetworkManager::ReceiveChangeScene(int& outNextScene)
{
	if (receivedNextScene != -1)
	{
		outNextScene = receivedNextScene;
		receivedNextScene = -1;
		return true;
	}
	return false;
}