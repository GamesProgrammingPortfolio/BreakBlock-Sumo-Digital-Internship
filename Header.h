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
	TYPE_CHEST
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
};
Chest chestObj;

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