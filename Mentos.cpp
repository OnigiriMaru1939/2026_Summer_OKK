#include "Mentos.h"
#include "Player.h"


Mentos::Mentos(FileManager& fileMng, Stage* stage, float x, float y):ItemBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/Gimmicks/Mentos.png");
	itemTypp_ = ITEM_TYPE::MENTOS;
	SetPosition(x, y);
}

Mentos::~Mentos()
{

}

void Mentos::Update()
{

}

void Mentos::OnGet(Player& player)
{
	player.SetPlayerJumpMag(2.0f);
	alive_ = false;
}
