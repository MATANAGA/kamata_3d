#include "TitleScene.h"
#include "Fade.h"
#include "MyMath.h"

using namespace KamataEngine;

void TitleScene::Initialize() {
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeTime);

	camera_.Initialize();

	// 加载BGM
	bgmHandle_ = Audio::GetInstance()->LoadWave("title_bgm.mp3");
	bgmPlaying_ = false;

	// 加载标题模型和玩家模型
	modelTitle_ = Model::CreateFromOBJ("titleFont");
	modelPlayer_ = Model::CreateFromOBJ("player");

	// 标题位置与缩放
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 7.0f, 0.0f};
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};

	// 玩家位置与缩放
	playerTransform_.Initialize();
	playerTransform_.translation_ = {0.0f, -5.0f, 0.0f}; // 屏幕正中偏下
	playerTransform_.scale_ = {13.0f, 13.0f, 13.0f};
	playerTransform_.rotation_.y = 3.14159f; // 180度

	// 颜色
	objectColor_.Initialize();
	objectColor_.SetColor({1, 1, 1, 1});
}

void TitleScene::Update() {
	frameCount_++;

	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_) {
			fade_->Update();
			if (!fade_->IsFading()) {
				phase_ = Phase::kMain;
			}
		}
		break;

	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			if (fade_) {
				fade_->Start(Fade::Status::FadeOut, kFadeTime);
			}
			phase_ = Phase::kFadeOut;
		}
		break;

	case Phase::kFadeOut:
		if (fade_) {
			fade_->Update();
			if (!fade_->IsFading()) {
				finished_ = true;
			}
		}
		break;
	}

	// 标题缩放动画
	float scaleBase = 2.0f;
	float scaleAmplitude = 0.5f;
	float scaleSpeed = 0.05f;
	float scale = scaleBase + scaleAmplitude * std::sin(frameCount_ * scaleSpeed);
	worldTransform_.scale_ = {scale, scale, scale};

	// 更新世界矩阵
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	playerTransform_.matWorld_ = MakeAffineMatrrix(playerTransform_.scale_, playerTransform_.rotation_, playerTransform_.translation_);
	playerTransform_.TransferMatrix();

	camera_.UpdateMatrix();
	camera_.TransferMatrix();

	// 播放BGM（第一次播放）
	if (!bgmPlaying_) {
		Audio::GetInstance()->PlayWave(bgmHandle_, true); // 循环播放
		bgmPlaying_ = true;
	}
}

void TitleScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	if (modelTitle_) {
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	}
	if (modelPlayer_) {
		modelPlayer_->Draw(playerTransform_, camera_, &objectColor_);
	}

	// Fade绘制
	if (fade_) {
		fade_->Draw();
	}

	Model::PostDraw();
}

TitleScene::~TitleScene() {
	// 停止BGM
	Audio::GetInstance()->StopWave(bgmHandle_);

	delete modelTitle_;
	delete modelPlayer_;
	delete fade_;
}
