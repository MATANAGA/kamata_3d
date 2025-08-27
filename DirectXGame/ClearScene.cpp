#include "ClearScene.h"

using namespace KamataEngine;

void ClearScene::Initialize() {
	finished_ = false;
	timer_ = 0.0f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	camera_.Initialize();

	clearTextModel_ = Model::CreateFromOBJ("clearText");
	clearTextTransform_.Initialize();
	clearTextTransform_.translation_ = {0.0f, 3.0f, 0.0f};
	clearTextTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 加载BGM
	bgmHandle_ = Audio::GetInstance()->LoadWave("clear_bgm.wav");
	bgmPlaying_ = false;
}

void ClearScene::Update() {
	timer_ += 1.0f / 60.0f;

	if (!bgmPlaying_) {
		Audio::GetInstance()->PlayWave(bgmHandle_, false);
		bgmPlaying_ = true;
	}

	if (fade_)
		fade_->Update();

	if (timer_ >= kWaitTime)
		finished_ = true;

	camera_.UpdateMatrix();
	camera_.TransferMatrix();
}

void ClearScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	if (clearTextModel_)
		clearTextModel_->Draw(clearTextTransform_, camera_);
	if (fade_)
		fade_->Draw();
	Model::PostDraw();
}

ClearScene::~ClearScene() {
	Audio::GetInstance()->StopWave(bgmHandle_);
	delete clearTextModel_;
	delete fade_;
}
