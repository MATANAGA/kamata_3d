#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Initialize() {
	uint32_t textureHandle = TextureManager::Load("white1x1.png");

	sprite_ = Sprite::Create(textureHandle, {0, 0}, {0.0f, 0.0f});
	sprite_->SetSize(Vector2(1980, 1080));
	sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)); // ← 最初は真っ黒
}

void Fade::Update() {

	switch (status_) {
	case Fade::Status::None:
		isFading_ = false;
		break;
	case Fade::Status::FadeIn: {
		counter_ += 1.0f / 60.0f;

		if (counter_ >= duration_) {
			counter_ = duration_;
			status_ = Status::None;
			isFading_ = false;

			sprite_->SetColor(Vector4(0, 0, 0, 0.0f)); // ← 明示的に完全透明にする
		} else {
			isFading_ = true;
			float alpha = 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f);
			sprite_->SetColor(Vector4(0, 0, 0, alpha));
		}
		break;
	}
	case Fade::Status::FadeOut:
		counter_ += 1.0f / 60.0f;

		if (counter_ >= duration_) {
			counter_ = duration_;
			isFading_ = false; // ←
		} else {
			isFading_ = true; // ←

		}
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	if (sprite_) {
		sprite_->Draw();
	}
	Sprite::PostDraw();
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFading() const { return isFading_; }

bool Fade::InFinshed() const {
	switch (status_) {
	case Fade::Status::None:
		break;
	case Fade::Status::FadeIn:
		break;
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
		break;
	default:
		break;
	}
	return true;
}
