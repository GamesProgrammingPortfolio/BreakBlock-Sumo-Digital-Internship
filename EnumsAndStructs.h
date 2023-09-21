//Game Objects
enum GameObjectType
{
	TYPE_AGENT = 0,
	TYPE_CHEST,
	TYPE_BALL,
	TYPE_COIN
};

//Placeholder for chests objects for later
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
	const Vector2D pos{ 0,0 };
	const Vector2D BALL_DROP_VELOCITY{ 1, 12 };

};
Ball ballObj;

struct Coin
{
	const float COIN_STARTING_X{ DISPLAY_WIDTH / 2 };
	const float COIN_STARTING_Y{ DISPLAY_HEIGHT / 2 };
	const Vector2D COIN_AABB{ 35.f, 35.f };
	const Vector2D pos{ 0,0 };
	const Vector2D COIN_DROP_VELOCITY{ 1, 12 };
	const int COIN_SCORE_VALUE{ 200 };
	const int COIN_RADIUS{ 48 };
};

Coin coinObj;

struct HUD
{
	const int SCORE_POS_X{ 1080 };
	const int SCORE_POS_Y{ 680 };
	const int LIVES_POS_X{ 100 };
	const int LIVES_POS_Y{ 680 };
};
HUD display;

enum class levelState
{
	STATE_NULL = -1,
	STATE_START,
	STATE_PLAY,
	STATE_GAMEOVER,
	STATE_PAUSE
};

levelState currentLevelState = levelState::STATE_START;

enum class ballState
{
	STATE_BALL_NULL = -1,
	STATE_BALL_DROP,
	STATE_BALL_PLAY
};

ballState currentBallState = ballState::STATE_BALL_DROP;

enum class playerState
{
	STATE_PLAYER_NULL = -1,
	STATE_PLAYER_HALT,
	STATE_PLAYER_CLIMB
};

playerState currentPlayerState = playerState::STATE_PLAYER_HALT;

struct GameState
{
	int lives{ MAX_LIVES };
	int score{ 0 };
	levelState state{ levelState::STATE_START };
	bool gameIsPaused{ false };
};

GameState game;