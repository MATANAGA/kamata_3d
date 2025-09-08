#pragma once
#include "KamataEngine.h"
#include "Player.h"

class Enemy {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool CheckCollisionWithPlayer(const Player& player) const;
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	static inline const float kHeight = 0.8f;
	bool isAlive_ = true; // 初始是活的

private:
	void UpdateMatrix();

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::Vector3 velocity_ = {};

	static inline const float kWalkSpeed = 0.05f;
	static inline const float kWidth = 0.8f;

	float walkTimer_ = 0.0f;
	float selfRotateAngle_ = 0.0f;
	float selfRotateX_ = 0.0f;
	float selfRotateY_ = 0.0f;

	static constexpr float kPatrolMinX = 5.0f;
	static constexpr float kPatrolMaxX = 15.0f;

	static inline const float kWalkMotionAngleStart = -20.0f;
	static inline const float kWalkMotionAngleEnd = 20.0f;
	static inline const float kWalkMotionTime = 1.0f;
};
