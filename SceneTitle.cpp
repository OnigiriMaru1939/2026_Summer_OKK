#include "SceneTitle.h"
#include "InputManager.h"
#include "Application.h"
#include "BGMManager.h"
#include <algorithm>

SceneTitle::SceneTitle(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng, sceneMng)
{

	IPDATA ipList[10];
	ipCount = 0;
	int result = GetMyIPAddress(ipList, 10, &ipCount);

	if (result == 0 && ipCount > 0)
	{
		for (int i = 0; i < ipCount; i++)
		{
			sprintf_s(myIPBuffer[i], sizeof(myIPBuffer[i]), "%d.%d.%d.%d", ipList[i].d1, ipList[i].d2, ipList[i].d3, ipList[i].d4);
		}
	}
	else
	{
		sprintf_s(myIPBuffer[0], "Your IP: N/A");
	}
	ClearInputCharBuf();

	_fadeAlpha = 255.0f;
	_bgImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_bg.png");
	_TitleLogoImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_Logo.png");
	_TitleStartImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_Start.png");
	_Title1PImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_1P_Button.png");
	_Title2PImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_2P_Button.png");
	_mainBgm = fileMng_.LoadSoundFM("Resource/Sound/BGM/Night_Light.wav");
	_decideSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");


	BGMManager::GetInstance().PlayBGM(_mainBgm);
	_mainBgm->SetVolume(196);
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   SetNextScene(SceneID::EXIT);
														   isEnd = true;
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   if (_isInputActive)
														   {
															   _isInputActive = false;
															   ClearInputCharBuf();
														   }
														   else
														   {
															   _decideSE->PlayOneShot();

															   if (_selectedIndex == 2)
															   {
																   _isInputActive = true;
																   //_inputIp = "";
																   ClearInputCharBuf();
															   }
															   else
															   {
																   SetNextScene(
																	   SceneID::STAGE_SELECT);
																   sceneMng_.SetIsHost(_selectedIndex == 0);
																   sceneMng_.SetRemoteIp(_inputIp);
																   sceneMng_.GetNetworkManager().Initialize(_selectedIndex == 0, _inputIp);
																   isEnd = true;
															   }
														   }
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveV,
												   [this]()
												   {
													   if (_isInputActive) return;

													   int move = 0;
													   if (InputManager::GetInstance().GetActionValue(ActionID::MoveV) < 0) move = -1;
													   else if (InputManager::GetInstance().GetActionValue(ActionID::MoveV) > 0) move = 1;

													   // 0～2 の間で移動
													   _selectedIndex = std::clamp(_selectedIndex + move, 0, MAX_INDEX);
												   });


	sceneMng_.SetTransitionDuration(45.0f);
}

SceneTitle::~SceneTitle()
{

}

void SceneTitle::Update()
{
	// 入力モードの時だけ入力を拾う
	if (_isInputActive)
	{
		int charCode = GetInputChar(TRUE);
		
		// 文字入力(数字とドットのみ)
		if (charCode != 0 && charCode != -1)
		{
			if ((charCode >= '0' && charCode <= '9') || charCode == '.')
			{
				if (_inputIp.length() < 15) // IPアドレスの文字数制限
				{
					_inputIp += (char)charCode;
				}
			}
		}

		// バックスペース
		if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_BACK))
		{
			if (!_inputIp.empty())
			{
				_inputIp.pop_back();
			}
		}

		if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
		{
			_isInputActive = false;
			ClearInputCharBuf();
		}
	}
}

void SceneTitle::Draw()
{
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, _bgImg->GetHandle(), true);
	DrawRotaGraph(Application::SCREEN_WID / 2, LOGO_Y, 1.0f, 0.0f, _TitleLogoImg->GetHandle(), true);

	// ─── 選択肢の描画 (選択中なら黄色、それ以外は白) ───
	unsigned int colorIP = (_selectedIndex == 2) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

	DrawRotaGraph(Application::SCREEN_WID / 2, MENU_Y, 1.0f, 0.0f, _Title1PImg->GetHandle(), true);
	DrawRotaGraph(Application::SCREEN_WID / 2, MENU_Y + MENU_SPACING, 1.0f, 0.0f, _Title2PImg->GetHandle(), true);

	std::string ipLabel = "Target IP: " + _inputIp;

	if (_isInputActive)
	{
		ipLabel += "_";
	}

	DrawString(Application::SCREEN_WID / 2 - 120, MENU_Y + (MENU_SPACING * 2) - GetFontSize() / 2, ipLabel.c_str(), colorIP);

	DrawString(Application::SCREEN_WID / 2 - 120, IP_Y + 30, "  Your IP:", 0xffC800);
	for (int i = 0; i < ipCount; i++)
	{
		DrawString(Application::SCREEN_WID / 2, IP_Y + 30 + (i * 20), myIPBuffer[i], GetColor(255, 200, 0));
	}

	// 選択中の項目の左に矢印を描画
	SetFontSize(40);
	int arrowX = Application::SCREEN_WID / 2 - 200;
	int arrowY = MENU_Y + (_selectedIndex * MENU_SPACING) - GetFontSize() / 2;
	DrawString(arrowX, arrowY, "▶", GetColor(255, 255, 0));
	SetFontSize(20);

	DrawRotaGraph(Application::SCREEN_WID / 2, START_Y, 0.8f, 0.0f, _TitleStartImg->GetHandle(), true);

	std::string TitleEndText = "Press Escape/Back to Exit";
	int textWidth = GetDrawStringWidth(TitleEndText.c_str(), TitleEndText.length());
	DrawString((Application::SCREEN_WID / 2) - (textWidth / 2), START_Y + 60, TitleEndText.c_str(), GetColor(255, 255, 255));

	//DrawFormatString(700, 0, 0x00ff00, "selectedIndex:%d", _selectedIndex);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeAlpha));
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneTitle::TransitionIn(float t)
{
	float e = EaseInCubic(1 - t);
	_fadeAlpha = e * 255.0f;
}

void SceneTitle::TransitionOut(float t)
{
	_fadeAlpha = EaseOutCubic(t) * 255.0f;
}
