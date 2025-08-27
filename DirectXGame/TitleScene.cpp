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

	// 加载模型
	modelTitle_ = Model::CreateFromOBJ("titleFont");
	modelPlayer_ = Model::CreateFromOBJ("player");

	// 标题
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 7.0f, 0.0f};
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};

	// 玩家
	playerTransform_.Initialize();
	playerTransform_.translation_ = {0.0f, -5.0f, 0.0f};
	playerTransform_.scale_ = {13.0f, 13.0f, 13.0f};
	playerTransform_.rotation_.y = 3.14159f;

	objectColor_.Initialize();
	objectColor_.SetColor({1, 1, 1, 1});
}

void TitleScene::Update() {
	frameCount_++;

	// BGM循环播放
	if (!bgmPlaying_) {
		bgmHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true); // 第二个参数 true 循环
		bgmPlaying_ = true;
	}

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

	float scaleBase = 2.0f;
	float scaleAmplitude = 0.5f;
	float scaleSpeed = 0.05f;
	float scale = scaleBase + scaleAmplitude * std::sin(frameCount_ * scaleSpeed);
	worldTransform_.scale_ = {scale, scale, scale};

	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	playerTransform_.matWorld_ = MakeAffineMatrrix(playerTransform_.scale_, playerTransform_.rotation_, playerTransform_.translation_);
	playerTransform_.TransferMatrix();

	camera_.UpdateMatrix();
	camera_.TransferMatrix();
}

void TitleScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	if (modelTitle_)
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	if (modelPlayer_)
		modelPlayer_->Draw(playerTransform_, camera_, &objectColor_);
	if (fade_)
		fade_->Draw();

	Model::PostDraw();
}

void TitleScene::StopBGM() {
	if (bgmPlaying_) {
		Audio::GetInstance()->StopWave(bgmHandle_);
		bgmPlaying_ = false;
	}
}

TitleScene::~TitleScene() {
	StopBGM();
	delete modelTitle_;
	delete modelPlayer_;
	delete fade_;
}
