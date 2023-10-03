#pragma once

constexpr float DISPLAY_WIDTH{ 1280 };
constexpr float DISPLAY_HEIGHT{ 720 };
constexpr int DISPLAY_SCALE{ 1 };
constexpr int BALL_RADIUS{ 48 };
constexpr int SCREEN_START_HEIGHT{ 0 };
constexpr float WEB_HEIGHT{ 550 };
constexpr int CHESTS_PER_ROW{ 12 };

enum GameObjectType
{
	TYPE_BALL = 0,
	TYPE_AGENT, 
	TYPE_CHEST,
	TYPE_COIN
};

struct HUD
{
	const int SCORE_POS_X{ 1080 };
	const int SCORE_POS_Y{ 680 };
	const int LIVES_POS_X{ 100 };
	const int LIVES_POS_Y{ 680 };

};
HUD display;

struct GameState
{
	int lives{ 3 };
	int score{ 0 };
};
GameState game;

struct Player
{
	const float PLAYER_STARTING_X{ DISPLAY_WIDTH / 2 };
	const int PLAYER_STARTING_Y{ 625 };
	const Vector2D PLAYER_AABB{ 100.f, 100.f };

};

Player playerObj;

struct Ball
{
	const float BALL_STARTING_X{ DISPLAY_WIDTH / 2 };
	const float BALL_STARTING_Y{ DISPLAY_HEIGHT / 2 };
	const int BALL_RADIUS{ 48 };
	const Vector2D BALL_AABB{ 50.f, 50.f };

};
Ball ballObj;

struct Chest
{
	const int CHEST_START_X{ 60 };
	const int CHEST_START_Y{ 60 };
	int chestSpacing{ 58 };
	int chestHeight{ 0 };
	int chestCounter{ 0 };
	bool chestCreated{ false };
	const Vector2D CHEST_AABB{ 50.f, 50.f };
	const int CHEST_VALUE{ 50 };
	int chestIndex{ 0 };
};
Chest chestObj;

struct Coin
{
	const float COIN_STARTING_X{ DISPLAY_WIDTH / 2 };
	const float COIN_STARTING_Y{ DISPLAY_HEIGHT / 2 };
	const Vector2D COIN_AABB{ 35.f, 35.f };
	const Vector2D pos{ 0,0 };
	const Vector2D COIN_DROP_VELOCITY{ 1, 12 };
	const int COIN_RADIUS{ 48 };
	int coinIndex{ 0 };
	const float COIN_SPEED{ 4.0f };
	const int COIN_VALUE{ 200 };
};
Coin coinObj;

enum class levelState
{
	STATE_NULL = -1,
	STATE_START,
	STATE_PLAY,
	STATE_GAMEOVER,
	STATE_PAUSE,
	STATE_WIN
};
levelState currentLevelState = levelState::STATE_START;

// Forward-declaration of Draw
void Draw();
void DrawHud();
void DrawObjects();
void SidesAndTop();
boolean BallCollision();
boolean ChestCollision();
void BallBounce();
void HandlePlayerControls();
void getStartingValues();
void InitialCreation();
void BallChangeDirection();
void CoinMovement();
void LoseCondition();
boolean OutOfBoundsChecker();
void ResetBallPosition();
void ResetGame();
void ResetGameWin();
void WinCondition();

void GameStart();
void GamePlay();
void GameOver();
void GamePause();
void GameWin();

boolean CoinCollision();
void CollectCoin();
boolean ChestChecker();