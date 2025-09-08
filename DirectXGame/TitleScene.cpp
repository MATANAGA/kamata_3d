#include "TitleScene.h"
#include "MyMath.h"
#include <cmath>

using namespace KamataEngine;

void TitleScene::Initialize() {
	// Fade初始化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeTime);

	// Camera初始化
	camera_.Initialize();

	// BGM
	bgmHandle_ = Audio::GetInstance()->LoadWave("game_bgm.mp3");
	bgmPlaying_ = false;

	// 标题模型
	modelTitle_ = Model::CreateFromOBJ("titleFont");

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 7.0f, 0.0f};
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};
	objectColor_.Initialize();
	objectColor_.SetColor({1, 1, 1, 1});

	// 玩家模型
	modelPlayer_ = Model::CreateFromOBJ("player2");
	playerTransform_.Initialize();
	playerTransform_.translation_ = {0.0f, -10.0f, 0.0f};
	playerTransform_.scale_ = {13.0f, 13.0f, 13.0f};
	playerTransform_.rotation_.y = 3.14159f;

	title_ = Model::CreateFromOBJ("title");
	// 底部标题模型
	titleTransform_.Initialize();
	titleTransform_.translation_ = {0.0f, -18.0f, 0.0f}; // 放到底部

	// 四个角落模型
	const char* cornerNames[kCornerCount] = {"player", "player2", "gost", "zonbiface"};
	Vector3 cornerPositions[kCornerCount] = {
	    {-28.0f, 15.0f,  0.0f},
        {28.0f,  15.0f,  0.0f},
        {-28.0f, -15.0f, 0.0f},
        {28.0f,  -15.0f, 0.0f}
    };
	for (int i = 0; i < kCornerCount; i++) {
		cornerModels_[i] = Model::CreateFromOBJ(cornerNames[i]);
		cornerTransforms_[i].Initialize();
		cornerTransforms_[i].translation_ = cornerPositions[i];
		cornerTransforms_[i].scale_ = {11.0f, 11.0f, 11.0f};
		cornerTransforms_[i].rotation_.y = 3.14159f;
	}

	// 说明页Sprite
	instructionTextureHandle_ = TextureManager::GetInstance()->Load("playguid.png");
	instructionSprite_ = Sprite::Create(instructionTextureHandle_, {0, 0});
	showInstruction_ = false;
}

void TitleScene::Update() {
	frameCount_++;

	// BGM循环播放
	if (!bgmPlaying_) {
		bgmHandle_ = Audio::GetInstance()->PlayWave(bgmHandle_, true);
		bgmPlaying_ = true;
	}

	// Fade & Phase
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_) {
			fade_->Update();
			if (!fade_->IsFading())
				phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (!showInstruction_ && Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			showInstruction_ = true;
		} else if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (fade_)
				fade_->Start(Fade::Status::FadeOut, kFadeTime);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		if (fade_) {
			fade_->Update();
			if (!fade_->IsFading())
				finished_ = true;
		}
		break;
	}

	// 标题模型缩放动画
	float scaleBase = 2.0f;
	float scaleAmplitude = 0.5f;
	float scaleSpeed = 0.05f;
	float scale = scaleBase + scaleAmplitude * std::sin(frameCount_ * scaleSpeed);
	worldTransform_.scale_ = {scale, scale, scale};

	// 更新矩阵
	worldTransform_.matWorld_ = MakeAffineMatrrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();

	playerTransform_.matWorld_ = MakeAffineMatrrix(playerTransform_.scale_, playerTransform_.rotation_, playerTransform_.translation_);
	playerTransform_.TransferMatrix();

	titleTransform_.matWorld_ = MakeAffineMatrrix(titleTransform_.scale_, titleTransform_.rotation_, titleTransform_.translation_);
	titleTransform_.TransferMatrix();

	for (int i = 0; i < kCornerCount; i++) {
		cornerTransforms_[i].matWorld_ = MakeAffineMatrrix(cornerTransforms_[i].scale_, cornerTransforms_[i].rotation_, cornerTransforms_[i].translation_);
		cornerTransforms_[i].TransferMatrix();
	}

	camera_.UpdateMatrix();
	camera_.TransferMatrix();
}

void TitleScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	if (modelTitle_)
		modelTitle_->Draw(worldTransform_, camera_, &objectColor_);
	if (modelPlayer_)
		modelPlayer_->Draw(playerTransform_, camera_, &objectColor_);
	for (int i = 0; i < kCornerCount; i++)
		if (cornerModels_[i])
			cornerModels_[i]->Draw(cornerTransforms_[i], camera_, &objectColor_);
	
	if (title_)
		title_->Draw(titleTransform_, camera_, &objectColor_);

	Model::PostDraw();

	if (showInstruction_ && instructionSprite_) {
		Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
		instructionSprite_->Draw();
		Sprite::PostDraw();
	}

	if (fade_)
		fade_->Draw();
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
	delete title_;
	for (int i = 0; i < kCornerCount; i++)
		delete cornerModels_[i];
	delete fade_;
	delete instructionSprite_;
}
