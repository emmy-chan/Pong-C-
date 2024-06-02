#include "pong.hpp"
#include "math.hpp"

Game game;

void PongPaddle::run(const float& fElapsedTime)
{
	//up
	if (GetAsyncKeyState(0x41) || GetAsyncKeyState(0x57) || GetAsyncKeyState(0x25) || GetAsyncKeyState(0x26)) {
		if (game.y + paddleY > game.y) {
			paddleY -= paddleSpeed * fElapsedTime;
		}
	}

	//down
	if (GetAsyncKeyState(0x44) || GetAsyncKeyState(0x53) || GetAsyncKeyState(0x27) || GetAsyncKeyState(0x28)) {
		if (paddleY + game.paddleH < game.h) {
			paddleY += paddleSpeed * fElapsedTime;
		}
	}

	// Check collision with player paddle
	if (isRectInside(game.pongBallX, game.pongBallY, game.ballSize, game.ballSize, paddleX, paddleY, game.paddleW, game.paddleH)) {
		if (game.pongBallX > paddleX) {
			game.CalculateAndApplyCollisionReflection(paddleX, paddleY);
			game.pongBallX = paddleX + game.paddleW + 1;
			this->colliding = true;
		}
	}

	// player score
	if (game.pongBallX > game.w - game.ballSize) {
		game.pongBallX = game.w * 0.5f - game.ballSize * 0.5f;
		game.pongBallY = game.h * 0.5f - game.ballSize * 0.5f;
		game.pongBallSpeed = 75.0f + rand() % 125;
		game.pongVelX = -game.pongBallSpeed;

		if (rand() % 2) game.pongVelY = rand() % 2 == 0 ? game.pongBallSpeed : -game.pongBallSpeed;
		else game.pongVelY = 0;

		//angle = RandomFloat(-pi / 4, pi / 4);

		if (rand() % 2)
			game.pongVelX *= -1;

		game.aiOffset = (rand() % (game.paddleH / 2)) + 1;
		if (rand() % 2) game.aiOffset *= -1;

		score += 1;
		game.adaptive_difficulty++;
		//olc::SOUND::PlaySample(sndPlayerScore, false);
	}

	game.pongBallX += (game.pongVelX * fElapsedTime); //cos(angle) * (pongVelX * fElapsedTime);
	game.pongBallY -= (game.pongVelY * fElapsedTime); //sin(angle) * (pongVelY * fElapsedTime);

	// clamp ball y
	Clamp(game.pongBallY, 0.0f, (float)(game.h - game.ballSize));

	// clamp ball speed
	const float maxBallSpeed = 300.0f + float(game.difficulty * 20.0f);
	Clamp(game.pongVelX, -maxBallSpeed, maxBallSpeed);
	Clamp(game.pongVelY, -maxBallSpeed, maxBallSpeed);
}

void BotPaddle::run(const float& fElapsedTime)
{
	//bot ai
	paddleY = lerp(paddleY, game.pongBallY + (game.ballSize / 2) - (game.paddleH / 2) + game.aiOffset, ((5.f + (std::max)(game.adaptive_difficulty, game.difficulty))) * fElapsedTime); // difficulty goes here (10.f)
	Clamp(paddleY, 0.0f, (float)(game.h - game.paddleH));

	// Check collision with bot paddle
	if (isRectInside(game.pongBallX, game.pongBallY, game.ballSize, game.ballSize, paddleX, paddleY, game.paddleW, game.paddleH)) {
		// Check if the right edge of the ball is at or past the left edge of the paddle
		// and the ball is not already completely past the paddle
		if (game.pongBallX + game.ballSize >= paddleX && game.pongBallX < paddleX + game.paddleW) {
			game.CalculateAndApplyCollisionReflection(paddleX, paddleY);
			game.pongBallX = paddleX - game.ballSize; // Adjust position to prevent sticking
			this->colliding = true;
		}
	}

	//bot score
	if (game.x + game.pongBallX < game.x) {
		game.pongBallX = game.w * 0.5f - game.ballSize * 0.5f;
		game.pongBallY = game.h * 0.5f - game.ballSize * 0.5f;
		game.pongBallSpeed = 75.0f + rand() % 125;
		game.pongVelX = -game.pongBallSpeed;

		if (rand() % 2) game.pongVelY = rand() % 2 == 0 ? game.pongBallSpeed : -game.pongBallSpeed;
		else game.pongVelY = 0;

		//angle = RandomFloat(-pi / 4, pi / 4);

		if (rand() % 2)
			game.pongVelX *= -1;

		score += 1;
		game.adaptive_difficulty--;
		game.adaptive_difficulty = std::max<int8_t>(game.adaptive_difficulty, -8);
		//olc::SOUND::PlaySample(sndBotScore, false);
	}
}

void Game::CalculateAndApplyCollisionReflection(float paddleX, float paddleY)
{
	// Adjust ball reflection angle based on collision point
	const float paddleCenterY = paddleY + (float)game.paddleH / 2;
	const float relativeIntersectY = (game.pongBallY - paddleCenterY);

	// Determine the desired reflection angle based on the collision point
	float bounceAngle = 0.0f;

	if (relativeIntersectY < -(float)paddleH / 6)  // Top third of the paddle
		bounceAngle = REFLECT_ANGLE * PI / 180.0f; // Reflect upwards
	else if (relativeIntersectY > (float)paddleH / 6) // Bottom third of the paddle
		bounceAngle = -REFLECT_ANGLE * PI / 180.0f; // Reflect downwards

	// Calculate new velocities
	float pongVelMagnitude = std::sqrt(pongVelX * pongVelX + pongVelY * pongVelY);
	pongVelX = (paddleX < (float)w / 2 ? -1 : 1) * pongVelMagnitude * std::cos(bounceAngle);
	pongVelY = pongVelMagnitude * std::sin(bounceAngle);

	// Increase velocity magnitude by the speed increment factor
	pongVelMagnitude += 0.025f;

	// Normalize the velocities and scale them by the new magnitude
	const float normalizationFactor = pongVelMagnitude / std::hypot(pongVelX, pongVelY);
	pongVelX *= normalizationFactor;
	pongVelY *= normalizationFactor;

	// Update ball position and adjust velocity
	pongVelX *= -1.75f;
}
