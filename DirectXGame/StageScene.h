#pragma once
#include "KamataEngine.h"

class StageScene {
public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 終了判定
	bool IsFinished() const { return finished_; }

private:
	bool finished_ = false;

	// カメラなど必要に応じて追加
	KamataEngine::Camera camera_;
};


