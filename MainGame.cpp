#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Header.h"

boolean drawCoinIndex{ false };
int score = 0;
const float BOUNCE_LEFT{ -2.0f };
const float BOUNCE_RIGHT{ 2.0f };

//MAIN FUNCTIONS
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	InitialCreation();
	getStartingValues();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	switch (currentLevelState)
	{
	case levelState::STATE_START:
		GameStart();
		break;

	case levelState::STATE_PLAY:
		GamePlay();
		break;

	case levelState::STATE_GAMEOVER:
		GameOver();
		break;

	case levelState::STATE_PAUSE:
		GamePause();
		break;
	}

	Draw();

	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}


//DRAW FUNCTIONS
// Our draw function. Called by MainGameUpdate to render each frame. 
void Draw()
{
	// Reset our drawing buffer so it is white
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	if (currentLevelState == levelState::STATE_START || currentLevelState == levelState::STATE_PLAY)
	{
		DrawObjects();
		DrawHud();
	}
	//Draw additional start game text
	if (currentLevelState == levelState::STATE_START)
	{
		Play::DrawFontText("64px", "Hit Space to Start", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), Play::CENTRE);
	}

	//Draw Pause Text
	if (currentLevelState == levelState::STATE_PAUSE)
	{
		Play::DrawFontText("64px", "PAUSE!", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), Play::CENTRE);
		Play::DrawFontText("64px", "Hit Space to Resume", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 50), Play::CENTRE);
	}

	//If Ball Out of Bounds
	if (currentLevelState == levelState::STATE_GAMEOVER)
	{
		Play::DrawFontText("64px", "Game Over!", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), Play::CENTRE);
		Play::DrawFontText("64px", "Final Score: " + std::to_string(game.score), Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 50), Play::CENTRE);
		Play::DrawFontText("64px", "Hit Space To Restart", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 150), Play::CENTRE);

	}
	Play::PresentDrawingBuffer();
}

void DrawObjects()
{
	Play::DrawLine({ 0, WEB_HEIGHT }, { DISPLAY_WIDTH, WEB_HEIGHT }, Play::cWhite);
	Play::DrawObject(Play::GetGameObjectByType(TYPE_AGENT));

	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_BALL));

	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };

	//Fetching all the coins and assigning them to ID's
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };

	//Iterate over every coin ID
	for (int j : coinIds)
	{
		GameObject& coin = Play::GetGameObject(j);
		Play::DrawObject(coin);

		//Rectangle for testing collision
		//Play::DrawRect(coin.pos - coinObj.COIN_AABB, coin.pos + coinObj.COIN_AABB, Play::cWhite);
	}

	//Iterate over every chest ID
	//Get the chest ID and draw the chest
	for (int i : chestIds)
	{
		GameObject& chest = Play::GetGameObject(i);
		Play::DrawObject(chest);
	}

}

void CreateChests()
{
	std::vector<int> chestIds = Play::CollectGameObjectIDsByType(TYPE_CHEST);

	//While there is less than 24, draw a chest and a coin 
	for (int i = 1; i <= 24; i++) {
		Play::CreateGameObject(TYPE_CHEST, { chestObj.chestSpacing, chestObj.CHEST_START_Y + chestObj.chestHeight }, 10, "box");
		//Add the width and spacing, so next chest is drawn next to previous
		chestObj.chestSpacing += 105;

		//If there is no remainder then the max amount of chests for that row has been reached
		//Start a new line 
		if (i % CHESTS_PER_ROW == 0 && !(i == 0)) {
			chestObj.chestHeight += 110;
			chestObj.chestSpacing = chestObj.CHEST_START_X;
		}
	}
}


void DrawHud()
{
	//Add Score and Life Counter
	Play::DrawFontText("64px", "Lives: " + std::to_string(game.lives), Point2D(display.LIVES_POS_X, display.LIVES_POS_Y), Play::CENTRE);
	Play::DrawFontText("64px", "Score: " + std::to_string(game.score), Point2D(display.SCORE_POS_X, display.SCORE_POS_Y), Play::CENTRE);
}

//COLLISION CODE
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

//Uses AABB on both the ball and the paddle to detect collision

boolean BallCollision()
{
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	GameObject& player{ Play::GetGameObjectByType(TYPE_AGENT) };

	boolean xCollision = false;
	boolean yCollision = false;


	if (player.pos.y + playerObj.PLAYER_AABB.y > ball.pos.y - ballObj.BALL_AABB.y
		&& player.pos.y - playerObj.PLAYER_AABB.y < ball.pos.y + ballObj.BALL_AABB.y)
	{
		if (player.pos.x + playerObj.PLAYER_AABB.x > ball.pos.x - ballObj.BALL_AABB.x
			&& player.pos.x - playerObj.PLAYER_AABB.x < ball.pos.x + ballObj.BALL_AABB.x)
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

boolean ChestCollision()
{
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };

	for (int i : chestIds)
	{
		GameObject& chest = Play::GetGameObject(i);
		boolean xCollision = false;
		boolean yCollision = false;

		if (chest.pos.y + chestObj.CHEST_AABB.y > ball.pos.y - ballObj.BALL_AABB.y
			&& chest.pos.y - chestObj.CHEST_AABB.y < ball.pos.y + ballObj.BALL_AABB.y)
		{
			if (chest.pos.x + chestObj.CHEST_AABB.x > ball.pos.x - ballObj.BALL_AABB.x
				&& chest.pos.x - chestObj.CHEST_AABB.x < ball.pos.x + ballObj.BALL_AABB.x)
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

		if (yCollision && xCollision) {

			game.score += chestObj.CHEST_VALUE;
			Play::DestroyGameObject(i);
			
			return true;
		}
	}

	// Return false only if none of the chests collide
	return false;
}

void BallBounce()
{
	GameObject& ballObj = Play::GetGameObjectByType(TYPE_BALL);
	float horizontalDirection = BOUNCE_RIGHT; 

	if (ballObj.pos.x < Play::GetGameObjectByType(TYPE_AGENT).pos.x)
	{
		horizontalDirection = BOUNCE_LEFT;
	}

	ballObj.velocity.y = -10.0f; 
	ballObj.velocity.x = horizontalDirection * 3.0f; // Adjust the horizontal velocity as needed

	// Update the ball's position based on the new velocity
	ballObj.pos += ballObj.velocity;

}

void BallChangeDirection()
{
	GameObject& ballObj = Play::GetGameObjectByType(TYPE_BALL);
	ballObj.velocity.y *= -1;
}

//PLAYER CONTROL

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

	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PAUSE;
	}
}


//GAME START

void InitialCreation()
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CentreAllSpriteOrigins(); // this function makes it so that obj.pos values represent the center of a sprite instead of its top-left corner
	Play::CreateGameObject(TYPE_AGENT, { playerObj.PLAYER_STARTING_X, playerObj.PLAYER_STARTING_Y }, BALL_RADIUS, "agent");
	Play::CreateGameObject(TYPE_BALL, { ballObj.BALL_STARTING_X, ballObj.BALL_STARTING_X}, BALL_RADIUS, "ball");
	
	CreateChests();
}

void getStartingValues() {
	score = 0;
	
	// Set initial velocity for ball
	GameObject& ball = Play::GetGameObjectByType(TYPE_BALL);
	
	ball.acceleration = Vector2D(0.f, 0.55f);
}


void GameStart()
{
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
	}
}

void GamePlay()
{
	HandlePlayerControls();
	SidesAndTop();

	boolean collision = BallCollision();

	//If the paddle hits the spanner, bounce back up
	if (collision) {
		BallBounce();
	}

	boolean CCollision = ChestCollision();

	if (CCollision) {
		BallChangeDirection();
	}
}

void GamePause()
{
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
	}
}

void GameOver()
{

}