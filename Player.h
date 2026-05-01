#pragma once


#include <memory>
#include <string>

class FileManager;
class ImageFile;

class Player
{
public:
	static constexpr int SPEED = 15;				//プレイヤーの移動速度
	static constexpr int PLAYER_SUM = 3;			//プレイヤーの残機数

	Player(FileManager& fileMng);
	~Player();
	bool SystemInit();			//初期化
	bool SetImage(const std::string& path);			//画像のセット
	void Update();
	void Draw();
	void SodaShake();			//マウスを振ると炭酸蓄積ゲージが溜まる
	void SodaGaugeCharge();		//炭酸残量ゲージの自然回復
	void AddGravity();			//重力処理
	void Jump();				//ジャンプ処理
	void Rotate();				//回転処理
	void SodaAttack();			//炭酸攻撃処理

	int prevMouseX;				//前回のマウスX座標
	int prevMouseY;				//前回のマウスY座標

	float posX;					//プレイヤーのX座標
	float posY;					//プレイヤーのY座標

	int GetWidth() const;		//プレイヤーの画像の幅を取得
	int GetHeight() const;		//プレイヤーの画像の高さを取得

private:
	FileManager& fileManager;			//ファイルマネージャー
	std::shared_ptr<ImageFile> image_;	//プレイヤーの画像

	int width_;					//プレイヤーの画像の幅
	int height_;				//プレイヤーの画像の高さ

	float sodaGauge;			//炭酸残量ゲージ
	float sodaGaugeMax;			//炭酸残量ゲージの最大値
	float sodaShakeGauge;		//炭酸蓄積ゲージ
	float sodaShakeGaugeMax;	//炭酸蓄積ゲージの最大値
	bool aliveFlag;				//生存フラグ
	bool jumpFlag;				//ジャンプフラグ
	float gravity;				//重力
	float velocityY;			//Y軸の速度
	float angle;				//回転角度
};
