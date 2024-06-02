#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_SOUND
#include "olcPGEX_Sound.h"

#include "pong.hpp"

class Pong : public olc::PixelGameEngine
{
public:
	Pong()
	{
		sAppName = "Pong";
	}

public:
	bool OnUserCreate() override
	{
		// Setup Sounds
		olc::SOUND::InitialiseAudio(44100, 1, 8, 512);
		game.sndPong = olc::SOUND::LoadAudioSample("pong.wav");
		game.sndBotScore = olc::SOUND::LoadAudioSample("bot_score.wav");
		game.sndPlayerScore = olc::SOUND::LoadAudioSample("player_score.wav");

		// Get Game screen size
		game.w = ScreenWidth();
		game.h = ScreenHeight();

		// Create player paddle and set its properties
		std::unique_ptr<PongPaddle> playerPaddle = std::make_unique<PongPaddle>();
		playerPaddle->paddleX = game.paddleW;
		playerPaddle->paddleY = game.h * 0.5f - game.paddleH;
		playerPaddle->paddleSpeed = 300.0f;
		playerPaddle->score = 0;

		// Create bot paddle and set its properties
		std::unique_ptr<PongPaddle> botPaddle = std::make_unique<BotPaddle>();
		botPaddle->paddleX = float(game.w - game.paddleW * 2.0f);
		botPaddle->paddleY = game.h * 0.5f - game.paddleH;
		botPaddle->paddleSpeed = 300.0f;
		botPaddle->score = 0;

		// Add paddles to the vector
		game.paddles.push_back(std::move(playerPaddle));
		game.paddles.push_back(std::move(botPaddle));

		return true;
	}

	float soundTimer;

	void playPongSound() {
		if (soundTimer <= 0.f) {
			olc::SOUND::PlaySample(game.sndPong, false);
			soundTimer = 0.1f;
		}
	}

	void LockFPS()
	{
		if (GetElapsedTime() < 0.066f)
			Sleep(1);
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		LockFPS();
		Clear(olc::Pixel(0, 0, 0, 255));

		if (GetKey(olc::Key::SPACE).bReleased)
			game.paused = !game.paused;

		if (IsFocused() && !game.paused) {
			// Do the paddle stuff :D
			for (const auto& paddle : game.paddles) {
				paddle->run(fElapsedTime);
				
				// Play a sound effect if our paddle collides with the ball
				if (paddle->colliding) {
					playPongSound();
					paddle->colliding = false;
				}
			}

			// bounce off ceiling
			if (game.y + game.pongBallY <= game.y || game.pongBallY + game.ballSize >= game.h) { // bounce floors
				playPongSound();
				game.pongVelX *= 1.25f;
				game.pongVelY *= -1.50f;
			}
		}

		// middle line
		for (int i = 0; i < game.h; i += game.h / 20)
			DrawLine((int16_t)(game.w * 0.5f - 1), 2 + i, (int16_t)(game.w * 0.5f - 1), game.h / 20 + i - 4, olc::Pixel(255, 255, 255, 50));

		// ball
		FillRectDecal({ game.pongBallX, game.pongBallY }, { float(game.ballSize), float(game.ballSize) }, olc::WHITE);

		// draw paddles
		for (const auto& paddle : game.paddles) {
			FillRectDecal({ paddle->paddleX, paddle->paddleY }, { float(game.paddleW), float(game.paddleH) }, olc::WHITE);
		}
		
		const std::string playerScoreT = std::to_string(game.paddles[0]->score);
		const int TextW = playerScoreT.size() * 32;

		// draw scores
		DrawString({ game.w / 4 + 10, 5 }, playerScoreT, olc::WHITE, 3);
		DrawString({ game.w - game.w / 4 - TextW, 5 }, std::to_string(game.paddles[1]->score), olc::WHITE, 3);

		if (!IsFocused() || game.paused) {
			const std::string text1 = "Game paused";
			const int TextW1 = text1.size() * 8 / 2;

			const std::string text2 = "Press space to unpause";
			const int TextW2 = text2.size() * 8 / 2;

			DrawString({ game.w / 2 - TextW1 - 1, game.h / 2 }, text1, olc::WHITE);
			DrawString({ game.w / 2 - TextW2 - 1, game.h / 2 + 10 }, text2, olc::WHITE);
		}

		soundTimer -= 1.f * fElapsedTime;
		return true;
	}
};

// Hide console window
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
	//FreeConsole();
	Pong demo;
	if (demo.Construct(320, 240, 4, 4, false, true, false))
		demo.Start();

	return 0;
}