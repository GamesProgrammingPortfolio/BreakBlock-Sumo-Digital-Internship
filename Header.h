#pragma once

//Constants
constexpr float DISPLAY_WIDTH{ 1280 };
constexpr float DISPLAY_HEIGHT{ 720 };
constexpr int DISPLAY_SCALE{ 1 };
constexpr int SCREEN_START_HEIGHT{ 0 };

constexpr int RADIUS{ 48 };
constexpr float BOUNCE_LEFT{ -2.0f };
constexpr float BOUNCE_RIGHT{ 2.0f };

constexpr float WEB_HEIGHT{ 550 };
constexpr int CHESTS_PER_ROW{ 12 };
constexpr int MAX_LIVES{ 3 };

//Game Objects
enum GameObjectType
{
	TYPE_BALL = 0,
	TYPE_AGENT, 
	TYPE_CHEST,
	TYPE_COIN
};

//Level States
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

//Heads Up Display
struct HUD
{
	const int SCORE_POS_X{ 1080 };
	const int SCORE_POS_Y{ 680 };
	const int LIVES_POS_X{ 100 };
	const int LIVES_POS_Y{ 680 };
};
HUD display;

//Default GameState
struct GameState
{
	int lives{ 3 };
	int score{ 0 };
};
GameState game;

//Player Struct
struct Player
{
	const float PLAYER_STARTING_X{ DISPLAY_WIDTH / 2 };
	const int PLAYER_STARTING_Y{ 625 };
	const Vector2D PLAYER_AABB{ 100.f, 100.f };
};
Player playerObj;

//Ball Struct
struct Ball
{
	const float BALL_STARTING_X{ DISPLAY_WIDTH / 2 };
	const float BALL_STARTING_Y{ DISPLAY_HEIGHT / 2 };
	const Vector2D BALL_AABB{ 50.f, 50.f };
};
Ball ballObj;

//Chest Struct
struct Chest
{
	const int CHEST_START_X{ 60 };
	const int CHEST_START_Y{ 60 };
	const Vector2D CHEST_AABB{ 50.f, 50.f };
	const int CHEST_VALUE{ 50 };
	int chestSpacing{ 58 };
	int chestHeight{ 0 };
	int chestCounter{ 0 };
	int chestIndex{ 0 };
};
Chest chestObj;

//Coin Struct
struct Coin
{
	const float COIN_STARTING_X{ DISPLAY_WIDTH / 2 };
	const float COIN_STARTING_Y{ DISPLAY_HEIGHT / 2 };
	const float COIN_SPEED{ 4.0f };
	const int COIN_VALUE{ 200 };
	const Vector2D COIN_AABB{ 35.f, 35.f };
	const Vector2D pos{ 0,0 };
	const Vector2D COIN_DROP_VELOCITY{ 1, 12 };
	int coinIndex{ 0 };
};
Coin coinObj;

//Declarations

//Draw Declarations
void Draw();
void DrawHud();
void DrawObjects();

//Inital Declarations
void InitialCreation();
void GetStartingValues();

//Void Declarations
void SidesAndTop();
void BallBounce();
void HandlePlayerControls();
void BallChangeDirection();
void CoinMovement();
void LoseCondition();
void ResetBallPosition();
void ResetGame();
void ResetGameWin();
void WinCondition();
void CollectCoin();

//Game State Declarations
void GameStart();
void GamePlay();
void GameOver();
void GamePause();
void GameWin();

//Boolean Declarations
boolean BallCollision();
boolean ChestCollision();
boolean CoinCollision();
boolean ChestChecker();
boolean OutOfBoundsChecker();