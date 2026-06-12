#pragma once

#include "IBoss.h"
#include "EnemyBase.h"

class Boss1 : public EnemyBase, public IBoss
{
public:
	Boss1(FileManager& fileMng, Stage* stage, float x, float y);
	~Boss1() override;
	void Update() override;
	void Draw() const override;
	void BossAppear();
	void BossMove();
	void Jump();
	void SetAppearFlag(bool flag) { isAppearing = flag; } // 出現フラグを設定する関数
private:
	bool isAppearing;		//ボスが出現中かどうかのフラグ
};

