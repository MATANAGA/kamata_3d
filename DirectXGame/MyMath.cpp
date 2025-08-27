#include "MyMath.h"
#include <algorithm> // std::clamp
#include <cmath>
#include <numbers>
#include <random>

using namespace KamataEngine;
using namespace MathUtility;


KamataEngine::Matrix4x4 MakeAffineMatrrix(KamataEngine::Vector3& scale, KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation) {
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	// 回転行列の作成（Z → X → Y の順）
	Matrix4x4 matRotX = MakeRotateXMatrix(rotation.x);
	Matrix4x4 matRotY = MakeRotateYMatrix(rotation.y);
	Matrix4x4 matRotZ = MakeRotateZMatrix(rotation.z);
	Matrix4x4 matRot = matRotZ * matRotX * matRotY;

	// 平行移動行列の作成
	Matrix4x4 matTrans = MakeTranslateMatrix(translation);

	// スケーリング → 回転 → 平行移動 の合成
	Matrix4x4 matWorld = matScale * matRot * matTrans;

	return matWorld;
}

float ElseInOut(float x1, float x2, float t) { 
	
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f; 
	return Lerp(x1, x2, easedT);
}
float RandomRange(float min, float max) {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}
