#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "DisplayConsts.h"
#include "Declarations.h"
#include "EnumsAndStructs.h"

//Static, Part of the Hub
constexpr float WEB_HEIGHT{ 550 };

//booleans
boolean isColliding{ false };
boolean dropCoin{ false };

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	//Creates all starting objects 
	InitialCreation();
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	//Current Game State Switch
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
	//Quit if needed
	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

void Draw()
{
	//Clear Previos frame and draw background
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	//Combined to minimise code
	//On start and on play, draw the objects and the hud
	if (currentLevelState == levelState::STATE_START || currentLevelState == levelState::STATE_PLAY)
	{
		DrawObjects();
		CreateHud();
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


void DrawObjects() {
	//Drawing Web 
	Play::DrawLine({ DISPLAY_START, WEB_HEIGHT }, { DISPLAY_WIDTH, WEB_HEIGHT }, Play::cWhite);

	//Fetching all the chests and assigning them to ID's
	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };

	//Iterate over every chest ID
	//Get the chest ID and draw the chest
	for (int i : chestIds)
	{
		GameObject& chest = Play::GetGameObject(i);
		Play::DrawObject(chest);

		//Rectangle for testing collision
		Play::DrawRect(chest.pos - chestObj.CHEST_AABB, chest.pos + chestObj.CHEST_AABB, Play::cWhite);
	}

	//Fetching all the coins and assigning them to ID's
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };

	//Iterate over every coin ID
	for (int j : coinIds)
	{
		GameObject& coin = Play::GetGameObject(j);
		Play::DrawObject(coin);

		//Rectanggle for testing collision
		//Play::DrawRect(coin.pos - coinObj.COIN_AABB, coin.pos + coinObj.COIN_AABB, Play::cWhite);
	}

	//Drawing Objects that only have one instance
	Play::DrawObject(Play::GetGameObjectByType(TYPE_AGENT));
	
	//Allows ball to rotate during gameplay
	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_BALL));

	//AABB Box for Player
	GameObject& agent{ Play::GetGameObjectByType(TYPE_AGENT) };
	Play::DrawRect(agent.pos - playerObj.PLAYER_AABB, agent.pos + playerObj.PLAYER_AABB, Play::cWhite);

	//AABB Box for the ball
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	Play::DrawRect(ball.pos - ballObj.BALL_AABB, ball.pos + ballObj.BALL_AABB, Play::cWhite);
}

//Called in the inital creation
//Draw all chests across the top of the screen
void CreateChests()
{
	std::vector<int> chestIds = Play::CollectGameObjectIDsByType(TYPE_CHEST);
	std::vector<int> coinIds = Play::CollectGameObjectIDsByType(TYPE_COIN);

	//While there is less than 24, draw a chest and a coin 
	//Coin is currently drawn onto for testing but eventually will need to be swapped so is hidden.
	for (int i = 1; i <= 24; i++) {
		Play::CreateGameObject(TYPE_CHEST, { chestObj.chestSpacing, chestObj.CHEST_START_Y + chestObj.chestHeight }, 10, "box");
		Play::CreateGameObject(TYPE_COIN, { chestObj.chestSpacing, chestObj.CHEST_START_Y + chestObj.chestHeight }, coinObj.COIN_RADIUS, "coin");
		
		//Add the width and spacing, so next chest is drawn next to previous
		chestObj.chestSpacing += 105;

		//Could replace with a for loop? 
		//More efficent

		//If there is no remainder then the max amount of chests for that row has been reached
		//Start a new line 
		if (i % CHESTS_PER_ROW == 0 && !(i == 0)) {
			chestObj.chestHeight += 110;
			chestObj.chestSpacing = chestObj.CHEST_START_X;
		}
	}
}

//All objects that are drawn at the start of the game
void InitialCreation() {
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CentreAllSpriteOrigins();

	//Creation of Game Objects
	Play::CreateGameObject(TYPE_AGENT, { playerObj.PLAYER_STARTING_X, playerObj.PLAYER_STARTING_Y }, BALL_RADIUS, "agent8_hang_2");
	Play::CreateGameObject(TYPE_BALL, { ballObj.BALL_STARTING_X, ballObj.BALL_STARTING_Y }, ballObj.BALL_RADIUS, "ball");

	CreateChests();
}

//When space bar pressed and the game is started
void GameStart()
{
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	GameObject& player{ Play::GetGameObjectByType(TYPE_AGENT) };


	ball.pos = { ballObj.BALL_STARTING_X, ballObj.BALL_STARTING_Y };
	player.pos = { playerObj.PLAYER_STARTING_X, playerObj.PLAYER_STARTING_Y };
	player.velocity = Vector2D(0.0f, 0.0f);
	player.acceleration = Vector2D(0.0f, 0.f);

	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
		//Could cause issues?
		currentBallState = ballState::STATE_BALL_DROP;
	}
}

void GamePlay() 
{
	HandlePlayerControls();
	HandleBall();

	boolean BCollision = ballCollision();

	//If the paddle hits the spanner, bounce back up
	if (BCollision) {
		ballBounce();
	}

	boolean CCollision = chestCollision();

	if (CCollision) {
		ballBounce();
	}

	GameObject& player{ Play::GetGameObjectByType(TYPE_AGENT) };
	if (currentPlayerState == playerState::STATE_PLAYER_CLIMB)
	{
		Play::SetSprite(player, "agent8_climb", 0.25f);
	}
	else if (currentPlayerState == playerState::STATE_PLAYER_HALT)
	{
		Play::SetSprite(player, "agent8_halt", 0.333f);
	}
}

//While the game is paused
//If space is pressed return to play
void GamePause() 
{
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
	}
}

void CreateHud() 
{
	//Add Score and Life Counter
	Play::DrawFontText("64px", "Lives: " + std::to_string(game.lives), Point2D(display.LIVES_POS_X, display.LIVES_POS_Y), Play::CENTRE);
	Play::DrawFontText("64px", "Score: " + std::to_string(game.score), Point2D(display.SCORE_POS_X, display.SCORE_POS_Y), Play::CENTRE);
}

//Need to review here for animation frames
void HandlePlayerControls()
{
	GameObject& agent{ Play::GetGameObjectByType(TYPE_AGENT) };
	agent.acceleration = Vector2D(0.0f, 0.f);
	float accelerationIncrease = 0.5f;
	float deceleration = 0.2f;  // Adjust the deceleration rate as needed

	if (Play::KeyDown(VK_LEFT))
	{
		agent.acceleration.x -= accelerationIncrease;
	}

	if (Play::KeyDown(VK_RIGHT))
	{
		agent.acceleration.x += accelerationIncrease;
	}

	// Apply deceleration when no movement keys are pressed
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

	agent.velocity += agent.acceleration;
	agent.pos += agent.velocity;

	// Update animation state
	if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT))
	{
		currentPlayerState = playerState::STATE_PLAYER_CLIMB;
	}
	else
	{
		currentPlayerState = playerState::STATE_PLAYER_HALT;
	}

	if (agent.pos.x < 0 || agent.pos.x > DISPLAY_WIDTH)
	{
		agent.pos.x = std::clamp(agent.pos.x, 0.f, DISPLAY_WIDTH);
		//Stops the player object when at edge of screen
		agent.velocity.x = 0.0f;
	}

	//Is space pressed switch to pause
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PAUSE;
	}
}

void HandleBall()
{
	//Switch that handles current ball state
	switch (currentBallState)
	{

	//When dropping should have a higher velocity and acceleration
	case ballState::STATE_BALL_DROP:
		ballDown();
		break;

	
	//Could I do with another state here? or a rename? 
	case ballState::STATE_BALL_PLAY:
		ballBounce();
		break;
	}

	//Checks if out of bounds 
	boolean outOfBounds = outOfBoundsChecker();
	//If out of bounds start lose condition
	if (outOfBounds)
	{
		loseCondition();
	}
}

void ballDown() {
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	ball.pos += ballObj.BALL_DROP_VELOCITY;
	//Not sure if working
	ball.rotation += 0.05;

}

//Fix with new code (Maths slides)
void ballBounce() {
	GameObject& ball = Play::GetGameObjectByType(TYPE_BALL);

	// Set the ball's velocity to move upward 
	ball.velocity.y = -5.0f;

	// Update the ball's position based on the new velocity
	ball.pos += ball.velocity;
	
	ball.rotation += 0.05;

}

boolean ballCollision()
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
		currentBallState = ballState::STATE_BALL_PLAY;
		return true;
	}
	else 
	{
		return false;
	}

}

boolean chestCollision()
{
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };

	boolean xCollision = false;
	boolean yCollision = false;


	for (int i : chestIds) 
	{
		GameObject& chest = Play::GetGameObject(i);
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
		else {
			return false;
		}
	}
}

//Check if ball has left bottom of screen.
boolean outOfBoundsChecker()
{
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };

	if (ball.pos.y > DISPLAY_HEIGHT)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//If ball falls out of bounds
void loseCondition()
{

	//If no lives switch to GameOver
	if (game.lives == 0)
	{
		currentLevelState = levelState::STATE_GAMEOVER;
	}
	//Decrement Lives, switch game back to starting state.
	else
	{
		game.lives--;

		currentLevelState = levelState::STATE_START;
	}

}

//If Space pressed on Gameover screen
//Reset the game
void GameOver()
{
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_START;
		game.lives = MAX_LIVES;
		game.score = 0;
	}
}
