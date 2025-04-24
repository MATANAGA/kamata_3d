#include "Player.h"

using namespace KamataEngine;

void Player::Initialize() {
	textureHandle_ = TextureManager::Load("uvChecker.png");
	model_ = Model::Create();
	worldTransform_.Initialize();
}

void Player::Update() {
	// 玩家更新逻辑
}

void Player::Draw(Camera& camera) { model_->Draw(worldTransform_, camera, textureHandle_); }

Player::~Player() { delete model_; }
