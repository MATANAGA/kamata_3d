#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Player;

class CameraController {

public:
	struct Rect {
		float left = 0.0f;   // 矩形左边界
		float right = 0.0f;  // 矩形右边界
		float bottom = 0.0f; // 矩形下边界
		float top = 0.0f;    // 矩形上边界
	};

	void Initialize(); // 初始化相机
	void Update();     // 更新相机位置
	void Reset();      // 重置相机状态

	void SetTarget(Player* target) { target_ = target; }    // 设置追踪对象
	void SetMovableArea(Rect area) { movableArea_ = area; } // 设置相机可移动区域

	const KamataEngine::Camera& GetViewProjection() const { return camera_; } // 获取视图投影矩阵

private:
	Camera camera_;                               // 相机对象
	Player* target_ = nullptr;                    // 追踪的玩家对象
	Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f}; // 相机与目标的偏移

	Rect movableArea_ = {0, 30, 0, 30}; // 相机可移动区域

	Vector3 targetPosition_;                             // ← 追踪目标的位置
	static inline const float kInterpolationRate = 0.1f; // 平滑追踪插值率
	static inline const float kVelocityBias = 5.0f;     // 速度偏置（用于平滑移动）

	// ←★ 为了确保追踪对象始终在屏幕内的边缘预留的空白边距
	static inline const Rect targetMargin_ = {-9.0f, 9.0f, -5.0f, 5.0f};
};
