#pragma once
#include "KamataEngine.h"

class MapChipField;

class Player {
	static inline const float kBlank = 0.01f;

	enum LRDirection { kRight, kLeft };
	LRDirection lrdirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;
	bool onGround_ = true;

	static inline const float kGravityAccleration = 0.03f;
	static inline const float kLimitFallSpeed = 0.3f;

public:
	~Player();
	void SetAlive(bool alive) { alive_ = alive; }
	bool IsAlive() const { return alive_; }

	KamataEngine::Vector3 velocity_{};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.4f;
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	static inline const float kJumpAcceleration = 0.5f; // 改成 public

	static inline const float kCollisionWidth = 0.8f;
	static inline const float kCollisionHeight = 0.8f;
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
	// 新增：切换皮肤（替换玩家使用的模型）
	void SetModel(KamataEngine::Model* model) { model_ = model; }

private:
	bool alive_ = true;
	static inline const float kAttenuationLanding = 0.3f;
	static inline const float kAttenuationWall = 0.3f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	struct CollisionMapInfo {
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		KamataEngine::Vector3 move;
	};
	bool jumpKeyPrev_ = false; // 上一帧是否按下了跳跃键

	void InputMove();
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapMove(CollisionMapInfo& info);
	void CheckMapCeiling(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void CheckMapWall();
	void CheckMapLanding();
	void AnimateTurn();
	void UpdateMatrix();

	KamataEngine::Model* model_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* camera_;
};
