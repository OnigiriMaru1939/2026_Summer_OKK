#pragma once
// ゲームコントローラーの認識番号
// DxLib定数、DX_INPUT_PAD1等に対応
enum class JOYPAD_NO
{
	KEY_PAD1,			// キー入力とパッド１入力
	PAD1,				// パッド１入力
	PAD2,				// パッド２入力
	PAD3,				// パッド３入力
	PAD4,				// パッド４入力
	INPUT_KEY = 4096	// キー入力
};

// ゲームコントローラータイプ
// DxLib定数、DX_OTHER等に対応
enum class JOYPAD_TYPE
{
	OTHER = 0,
	XBOX_360,
	XBOX_ONE,
	DUAL_SHOCK_4,
	DUAL_SENSE,
	SWITCH_JOY_CON_L,
	SWITCH_JOY_CON_R,
	SWITCH_PRO_CTRL,
	MAX
};

// マウスボタンの種類
enum class MouseButton
{
	Left = 0,
	Right,
	Middle
};

// コントローラーのボタンの種類
enum class PadButton
{
	Face_Left = 0,
	Face_Right,
	Face_Top,
	Face_Down,
	Top,
	Down,
	Left,
	Right,
	L_Shoulder,
	R_Shoulder,
	L_Trigger,
	R_Trigger,
	Max
};

class InputManager
{
private:
	InputManager();
	~InputManager() {};

public:
	void DrawDebug(int x, int y) const;
	const char* GetPadName(int type) const;
	// シングルトン
	static InputManager& GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	void Update();

	// キー入力
	bool IsKeyPressed(int keyCode) const { return key[keyCode] > 0; }
	bool IsKeyTriggered(int keyCode) const { return key[keyCode] == 1; }
	bool IsKeyReleased(int keyCode) const { return prevKey[keyCode] > 0 && key[keyCode] == 0; }

	// マウス
	bool IsMousePressed(MouseButton button) const { return mouseButton[(int)button] > 0; }
	bool IsMouseTriggered(MouseButton button) const { return mouseButton[(int)button] == 1; }
	bool IsMouseReleased(MouseButton button) const { return mouseButton[(int)button] == 0 && prevMouseButton[(int)button] > 0; }
	int GetMouseX() const { return mouseX; }
	int GetMouseY() const { return mouseY; }
	int GetMouseDX() const { return mouseX - prevMouseX; }
	int GetMouseDY() const { return mouseY - prevMouseY; }

	// コントローラー
	bool IsPadPressed(int no, int btn) const { return padButton[no][btn] > 0; }
	bool IsPadTriggered(int no, int btn) const { return padButton[no][btn] == 1; }
	bool IsPadReleased(int no, int btn) const { return padButton[no][btn] == 0 && prevPadButton[no][btn] > 0; }
	// スティック値取得 (-32768 〜 32767)
	int GetPadStickLX(int padIndex) const { return padLX[padIndex]; }
	int GetPadStickLY(int padIndex) const { return padLY[padIndex]; }
	int GetPadStickRX(int padIndex) const { return padRX[padIndex]; }
	int GetPadStickRY(int padIndex) const { return padRY[padIndex]; }
private:
	// キー入力
	int key[256] = {};
	int prevKey[256] = {};

	// マウスボタンの入力状態
	int mouseButton[3] = {}; // 0:左 1:右 2:中 
	int prevMouseButton[3] = {};

	int mouseX;
	int mouseY;
	int prevMouseX;
	int prevMouseY;

	// コントローラーの入力状態
	static const int MAX_JOYPADS = 4;
	int padButton[MAX_JOYPADS][(int)PadButton::Max] = {};
	int prevPadButton[MAX_JOYPADS][(int)PadButton::Max] = {};
	int padLX[MAX_JOYPADS] = {};
	int padLY[MAX_JOYPADS] = {};
	int padRX[MAX_JOYPADS] = {};
	int padRY[MAX_JOYPADS] = {};

	// コントローラーごとのボタンマッピングを解決するための関数
	bool GetRawState(int no, PadButton btn);
};
