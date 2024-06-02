#pragma once
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <memory>
#include <cmath>

#include "math.h"

constexpr float REFLECT_ANGLE = 33.0f;

struct PongPaddle {
	float paddleX;
	float paddleY;
	float paddleSpeed;
	int score;
	bool colliding;

	virtual void run(const float& fElapsedTime);
};

struct BotPaddle : public PongPaddle {
	void run(const float& fElapsedTime) override;
};

struct Game {
	int8_t sndPong;
	int8_t sndBotScore;
	int8_t sndPlayerScore;
	int16_t w;
	int16_t h;
	int16_t y = 0;
	int16_t x = 0;
	uint8_t paddleW = 8;
	uint8_t paddleH = 36;
	float pongVelX = -100.0f + rand() % 100;
	float pongVelY = 100.0f + rand() % 100;
	uint8_t ballSize = 8;
	float pongBallX = w * 0.5f - ballSize * 0.5f;
	float pongBallY = h * 0.5f - ballSize * 0.5f;
	float pongBallSpeed;
	int8_t adaptive_difficulty = 0;
	int8_t difficulty = 5;
	bool paused;
	uint8_t aiOffset;

	std::vector<std::unique_ptr<PongPaddle>> paddles;

	void CalculateAndApplyCollisionReflection(float paddleX, float paddleY);
	void playPongSound();
}; extern Game game;