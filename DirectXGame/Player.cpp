#include "Player.h"

using namespace KamataEngine;

void Player::Initialize() {
	player_ = Model::CreateFromOBJ("player", true);

	// 先初始化（以防内部有清空逻辑）
	worldTransform_.Initialize();

	// 再设置缩放和位置
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f}; // 放大玩家
	worldTransform_.translation_ = {80.0f, 0.0f, 5.0f};

	// 最后生成变换矩阵
	worldTransform_.TransferMatrix();
}





void Player::Update() {

}

void Player::Draw(Camera& camera_) { player_->Draw(worldTransform_, camera_); }

Player::~Player() { delete player_; }
