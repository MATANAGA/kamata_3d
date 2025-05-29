#include "Player.h"

using namespace KamataEngine;

void Player::Initialize() {
	player_ = Model::CreateFromOBJ("player", true);

	worldTransform_.Initialize();

}

void Player::Update() {
	// 玩家更新逻辑
}

void Player::Draw(Camera& camera) { player_->Draw(worldTransform_, camera); }

Player::~Player() { delete player_; }
