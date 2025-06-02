#include "Player.h"

using namespace KamataEngine;

void Player::Initialize() {
	player_ = Model::CreateFromOBJ("player", true);
	worldTransform_.Initialize();

}


void Player::Update() {

}

void Player::Draw(Camera& camera) { player_->Draw(worldTransform_, camera); }

Player::~Player() { delete player_; }
