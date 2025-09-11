#include "DeathScene.h"
#include "Fade.h"

using namespace KamataEngine;

void DeathScene::Initialize() {
	finished_ = false;
	timer_ = 0.0f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	camera_.Initialize();

	deathTextModel_ = Model::CreateFromOBJ("deathText");
	deathTextTransform_.Initialize();
	deathTextTransform_.translation_ = {0.0f, 3.0f, 0.0f};
	deathTextTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 加载 BGM
	bgmHandle_ = Audio::GetInstance()->LoadWave("dead_bgm.wav");
	bgmPlaying_ = false;
}

void DeathScene::Update() {
	timer_ += 1.0f / 60.0f;

	// BGM 循环播放
	if (!bgmPlaying_) {
		bgmHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
		bgmPlaying_ = true;
	}

	if (fade_)
		fade_->Update();
	if (timer_ >= kWaitTime)
		finished_ = true;

	camera_.UpdateMatrix();
	camera_.TransferMatrix();
}

void DeathScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	if (deathTextModel_)
		deathTextModel_->Draw(deathTextTransform_, camera_);
	if (fade_)
		fade_->Draw();
	Model::PostDraw();
}

void DeathScene::StopBGM() {
	if (bgmPlaying_) {
		Audio::GetInstance()->StopWave(bgmHandle_);
		bgmPlaying_ = false;
	}
}

DeathScene::~DeathScene() {
	StopBGM();
	delete deathTextModel_;
	delete fade_;
}
