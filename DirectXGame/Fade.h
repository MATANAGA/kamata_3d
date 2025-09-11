#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Fade {
public:
	enum class Status { None, FadeIn, FadeOut };
	Status status_ = Status::None;
	float duration_ = 0.0f;
	float counter_ = 0.0f;

	void Start(Status status, float duration);
	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	void Stop();
	bool IsFading() const;

private:
	bool isFading_ = false; 
	KamataEngine::Sprite* sprite_ = nullptr;
	Status mode_ = Status::None;
	float alpha_ = 1.0f;
	float fadeSpeed_ = 0.02f;
	bool InFinshed() const;

};
