#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"

constexpr float DISPLAY_WIDTH{ 1280 };
constexpr float DISPLAY_HEIGHT{ 720 };
constexpr int DISPLAY_SCALE{ 1 };
constexpr int BALL_RADIUS{ 48 };
constexpr int SCREEN_START_HEIGHT{ 0 };
constexpr float WEB_HEIGHT{ 550 };

int score = 0;

// Define a value for our ball's default velocity
const Vector2D BALL_VELOCITY_DEFAULT(10.0f, 0.f);

// AABB's 
const Vector2D BALL_AABB{ 40.f, 45.f };
const Vector2D PLAYER_AABB{ 100.f, 50.f };

enum GameObjectType
{
	TYPE_BALL = 0,
	TYPE_AGENT = 1
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

// Forward-declaration of Draw
void Draw();
void DrawHud();
void DrawObjects();
void SidesAndTop();
boolean BallCollision();
void BallBounce();
void HandlePlayerControls();
void loseCondition();
void getStartingValues();
void InitialCreation();

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	InitialCreation();
	getStartingValues();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	HandlePlayerControls();
	SidesAndTop();

	boolean collision = BallCollision();

	//If the paddle hits the spanner, bounce back up
	if (collision) {
		BallBounce();
	}

	loseCondition();

	Draw();

	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

// Our draw function. Called by MainGameUpdate to render each frame. 
void Draw()
{
	// Reset our drawing buffer so it is white
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	DrawObjects();
	DrawHud();

	Play::PresentDrawingBuffer();
}

void DrawObjects()
{
	Play::DrawLine({ 0, WEB_HEIGHT }, { DISPLAY_WIDTH, WEB_HEIGHT }, Play::cWhite);
	Play::DrawObject(Play::GetGameObjectByType(TYPE_AGENT));
	// Draw the AABB box for our paddle
	GameObject& paddleObj{ Play::GetGameObjectByType(TYPE_AGENT) };
	Play::DrawRect(paddleObj.pos - PLAYER_AABB, paddleObj.pos + PLAYER_AABB, Play::cWhite);

	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_BALL));
}

void DrawHud()
{
	//Add Score and Life Counter
	Play::DrawFontText("64px", "Lives: " + std::to_string(game.lives), Point2D(display.LIVES_POS_X, display.LIVES_POS_Y), Play::CENTRE);
	Play::DrawFontText("64px", "Score: " + std::to_string(game.score), Point2D(display.SCORE_POS_X, display.SCORE_POS_Y), Play::CENTRE);
}

void SidesAndTop()
{
	GameObject& ballObj{ Play::GetGameObjectByType(TYPE_BALL) };

	if (ballObj.pos.x < 0 || ballObj.pos.x > DISPLAY_WIDTH)
	{
		ballObj.pos.x = std::clamp(ballObj.pos.x, 0.f, DISPLAY_WIDTH);
		ballObj.velocity.x *= -1;

		ballObj.velocity += ballObj.acceleration;
		ballObj.pos += ballObj.velocity;
	}

	ballObj.pos += ballObj.velocity;
	ballObj.rotation += 0.05;

	if (ballObj.pos.y < SCREEN_START_HEIGHT)
	{
		ballObj.velocity.y *= -1;
	}

}

void HandlePlayerControls()
{
	GameObject& agent{ Play::GetGameObjectByType(TYPE_AGENT) };
	agent.acceleration = Vector2D(0.0f, 0.f);
	float accelerationIncrease = 0.5f;
	float deceleration = 0.2f;

	if (Play::KeyDown(VK_LEFT))
	{
		agent.acceleration.x -= accelerationIncrease;
	}

	if (Play::KeyDown(VK_RIGHT))
	{
		agent.acceleration.x += accelerationIncrease;
	}

	if (!Play::KeyDown(VK_LEFT) && !Play::KeyDown(VK_RIGHT))
	{
		if (agent.velocity.x > 0)
		{
			agent.acceleration.x = -deceleration;
		}
		else if (agent.velocity.x < 0)
		{
			agent.acceleration.x = deceleration;
		}
	}
		if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT))
		{
			Play::SetSprite(agent, "agent8_climb", 0.25f);
		}
		else
		{
			Play::SetSprite(agent, "agent8_halt", 0.333f);
		}

	agent.velocity += agent.acceleration;
	agent.pos += agent.velocity;

	if (agent.pos.x < 0 || agent.pos.x > DISPLAY_WIDTH)
	{
		agent.pos.x = std::clamp(agent.pos.x, 0.f, DISPLAY_WIDTH);
		//Stops the player object when at edge of screen
		agent.velocity.x = 0.0f;
	}
}

//Uses AABB on both the ball and the paddle to detect collision
boolean BallCollision()
{
	GameObject& ballObj{ Play::GetGameObjectByType(TYPE_BALL) };
	GameObject& paddleObj{ Play::GetGameObjectByType(TYPE_AGENT) };


	boolean xCollision = false;
	boolean yCollision = false;


	if (paddleObj.pos.y + PLAYER_AABB.y > ballObj.pos.y - BALL_AABB.y
		&& paddleObj.pos.y - PLAYER_AABB.y < ballObj.pos.y + BALL_AABB.y)
	{
		if (paddleObj.pos.x + PLAYER_AABB.x > ballObj.pos.x - BALL_AABB.x
			&& paddleObj.pos.x - PLAYER_AABB.x < ballObj.pos.x + BALL_AABB.x)
		{
			xCollision = true;
		}
		else
		{
			xCollision = false;
		}
		yCollision = true;
	}
	else
	{
		yCollision = false;
	}

	if (yCollision && xCollision)
	{

		return true;
	}
	else
	{
		return false;
	}

}

void BallBounce()
{
	GameObject& ballObj = Play::GetGameObjectByType(TYPE_BALL);

	// Set the ball's velocity to move upward (you can adjust the value as needed)
	ballObj.velocity.y = -10.0f;

	// Update the ball's position based on the new velocity
	ballObj.pos += ballObj.velocity;

}

void loseCondition() {

	GameObject& ballObj{ Play::GetGameObjectByType(TYPE_BALL) };

	if (ballObj.pos.y > DISPLAY_HEIGHT)
	{
		Play::DestroyGameObjectsByType(TYPE_BALL);
		getStartingValues();

	}
}

void InitialCreation()
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CentreAllSpriteOrigins(); // this function makes it so that obj.pos values represent the center of a sprite instead of its top-left corner
	Play::CreateGameObject(TYPE_AGENT, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 100 }, BALL_RADIUS, "agent");
	Play::CreateGameObject(TYPE_BALL, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, BALL_RADIUS, "ball");
	Play::CentreAllSpriteOrigins();
}

void getStartingValues() {
	score = 0;
	
	// Set initial velocity for ball
	GameObject& ballObj = Play::GetGameObjectByType(TYPE_BALL);
	ballObj.velocity = BALL_VELOCITY_DEFAULT;
	ballObj.acceleration = Vector2D(0.f, 0.55f);
}