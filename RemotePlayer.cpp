#include "RemotePlayer.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

static constexpr auto WATER_PARTICLE_PATH = "Resource/ParticleJsonData/waterParameter.json";

RemotePlayer::RemotePlayer(FileManager& fileMng, ParticleManager& pMng)
	: posX(0), posY(0),
	vx(0), vy(0),
	angle(0), scale(1.0f),
	isAttack(false),
	_width(64), _height(64),
	_pMng(pMng)
{
	sodaAttackSE = fileMng.LoadSoundFM("Resource/Sound/SE/Soda_SE.wav");

	_image = fileMng.LoadImageFM("Resource/Image/Player/Cider_Player.png");
	if (_image)
	{
		_width = _image->GetWidth();
		_height = _image->GetHeight();
	}

	_pMng.RegisterConfig(WATER_PARTICLE_PATH);
}

RemotePlayer::~RemotePlayer()
{
}

void RemotePlayer::Update()
{
	_pMng.UpdateAll();
	if (auto emitter = sodaParticle.lock())
	{
		emitter->SetPosition(posX - sinf(angle) * _width, posY + cosf(angle) * _height);
	}
}

void RemotePlayer::SyncState(const PlayerPacket& packet)
{
	// 受信したデータを自身の変数にコピー
	posX = packet.posX;
	posY = packet.posY;
	vx = packet.vx;
	vy = packet.vy;
	angle = packet.angle;
	isAttack = packet.isAttack;
}

void RemotePlayer::SyncState(const SodaJumpPacket& packet)
{

	const ParticleConfig* masterCfg = _pMng.GetConfig(WATER_PARTICLE_PATH);
	if (masterCfg)
	{
		ParticleConfig customCfg = *masterCfg;

		float plusAngle = packet.angle * (180.0f / DX_PI_F);
		while (plusAngle < 0) plusAngle += 360.0f;
		while (plusAngle >= 360.0f) plusAngle -= 360.0f;

		customCfg.initDir += plusAngle;
		customCfg.startScale *= packet.sodaRatio;
		customCfg.initSpeed *= packet.sodaRatio;

		// プレイヤー本体と同じオフセット計算で生成
		sodaParticle = _pMng.PlayParticle(customCfg,
						   packet.startX - sinf(packet.angle) * _width,
						   packet.startY + cosf(packet.angle) * _height);

		sodaAttackSE->SetVolume((int)(packet.sodaRatio * 255)); // 音量を炭酸ゲージの割合に応じて変化させる
		sodaAttackSE->PlayOneShot();
	}
}

void RemotePlayer::Draw(float scrollX, float scrollY)
{
	_pMng.DrawAll(scrollX, scrollY);

	if (!_image) return;

	float drawX = posX - scrollX;
	float drawY = posY - scrollY;

	DrawRotaGraph(static_cast<int>(drawX), static_cast<int>(drawY),
				  scale, angle, _image->GetHandle(), TRUE);
}