#include "Player.h"

using namespace KamataEngine;

void Player::Initialize() {
	player_ = Model::CreateFromOBJ("player", true);
	worldTransform_.Initialize();

	worldTransform_.translation_ = {6.0f, 0.0f, 5.0f}; 
	worldTransform_.scale_ = {2.0f, 2.0f, 2.0f};       

	worldTransform_.TransferMatrix();
}


void Player::Update() {

}

void Player::Draw(Camera& camera) { player_->Draw(worldTransform_, camera); }

Player::~Player() { delete player_; }
