#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "DisplayConsts.h"
#include "Declarations.h"
#include "EnumsAndStructs.h"

//constant data
constexpr float WEB_HEIGHT{ 550 };
constexpr int BALL_RADIUS{ 48 };
constexpr int CHESTS_PER_ROW{ 12 };
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
	// Clear our screen white so we don't keep pixel data from the previous frame.
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();


	//Start Game Text
	if (currentLevelState == levelState::STATE_START)
	{
		DrawObjects();
		CreateHud();
		Play::DrawFontText("64px", "Hit Space to Start", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), Play::CENTRE);
	}

	if (currentLevelState == levelState::STATE_PLAY)
	{
		DrawObjects();
		CreateHud();
	}

	//Draw Pause Text
	if (currentLevelState == levelState::STATE_PAUSE)
	{
		Play::DrawFontText("64px", "PAUSE!", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), Play::CENTRE);
		Play::DrawFontText("64px", "Hit Space to Resume", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 50), Play::CENTRE);
	}

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

	//Iterating and drawing all the chests
	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };

	for (int i : chestIds)
	{
		GameObject& chest = Play::GetGameObject(i);
		Play::DrawObject(chest);
		Play::DrawRect(chest.pos - chestObj.CHEST_AABB, chest.pos + chestObj.CHEST_AABB, Play::cWhite);
	}
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };
	for (int j : coinIds)
	{
		GameObject& coin = Play::GetGameObject(j);
		Play::DrawObject(coin);
		//Play::DrawRect(chest.pos - chestObj.CHEST_AABB, chest.pos + chestObj.CHEST_AABB, Play::cWhite);
	}

	//Drawing GameObjects
	Play::DrawObject(Play::GetGameObjectByType(TYPE_AGENT));
	Play::DrawObject(Play::GetGameObjectByType(TYPE_BALL));

	//AABB Box for Player
	GameObject& agent{ Play::GetGameObjectByType(TYPE_AGENT) };
	Play::DrawRect(agent.pos - playerObj.PLAYER_AABB, agent.pos + playerObj.PLAYER_AABB, Play::cWhite);

	//AABB Box for the ball
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	Play::DrawRect(ball.pos - ballObj.BALL_AABB, ball.pos + ballObj.BALL_AABB, Play::cWhite);

}


void CreateChests()
{
	std::vector<int> chestIds = Play::CollectGameObjectIDsByType(TYPE_CHEST);
	std::vector<int> coinIds = Play::CollectGameObjectIDsByType(TYPE_COIN);

	for (int i = 1; i <= 24; i++) {
		Play::CreateGameObject(TYPE_CHEST, { chestObj.chestSpacing, chestObj.CHEST_START_Y + chestObj.chestHeight }, 10, "box");
		Play::CreateGameObject(TYPE_COIN, { chestObj.chestSpacing, chestObj.CHEST_START_Y + chestObj.chestHeight }, coinObj.COIN_RADIUS, "coin");
		chestObj.chestSpacing += 105;
		if (i % CHESTS_PER_ROW == 0 && !(i == 0)) {
			chestObj.chestHeight += 110;
			chestObj.chestSpacing = chestObj.CHEST_START_X;
		}
	}
}

void InitialCreation() {
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CentreAllSpriteOrigins();

	//Creation of Game Objects
	Play::CreateGameObject(TYPE_AGENT, { playerObj.PLAYER_STARTING_X, playerObj.PLAYER_STARTING_Y }, BALL_RADIUS, "agent8_hang_2");
	Play::CreateGameObject(TYPE_BALL, { ballObj.BALL_STARTING_X, ballObj.BALL_STARTING_Y }, ballObj.BALL_RADIUS, "ball");

	CreateChests();
}

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
		currentBallState = ballState::STATE_BALL_DROP;
	}

}
void GamePlay() {
	HandlePlayerControls();
	HandleBall();

	boolean collision = ballCollision();

	//If the paddle hits the spanner, bounce back up
	if (collision) {
		ballBounce();
	}

	chestCollision();

	if (isColliding) {
		ballBounce();
	}

}
void GamePause() {

	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
	}
}

void CreateHud() {
	//Add Score and Life Counter
	Play::DrawFontText("64px", "Lives: " + std::to_string(game.lives), Point2D(display.LIVES_POS_X, display.LIVES_POS_Y), Play::CENTRE);
	Play::DrawFontText("64px", "Score: " + std::to_string(game.score), Point2D(display.SCORE_POS_X, display.SCORE_POS_Y), Play::CENTRE);
}

void HandlePlayerControls()
{
	GameObject& agent{ Play::GetGameObjectByType(TYPE_AGENT) };
	agent.acceleration = Vector2D(0.0f, 0.f);
	float accelerationIncrease = 0.5f;

	if (Play::KeyDown(VK_LEFT))
	{
		agent.acceleration.x -= accelerationIncrease;
	}

	if (Play::KeyDown(VK_RIGHT))
	{
		agent.acceleration.x += accelerationIncrease;
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

void HandleBall()
{

	switch (currentBallState)
	{
	case ballState::STATE_BALL_DROP:
		ballDown();
		break;


	case ballState::STATE_BALL_PLAY:
		ballBounce();
		break;
	}

	boolean outOfBounds = outOfBoundsChecker();

	if (outOfBounds)
	{
		loseCondition();
	}
}

void ballDown() {
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };
	ball.pos += ballObj.BALL_DROP_VELOCITY;

}

void ballBounce() {
	GameObject& ball = Play::GetGameObjectByType(TYPE_BALL);

	// Set the ball's velocity to move upward (you can adjust the value as needed)
	ball.velocity.y = -5.0f;

	// Update the ball's position based on the new velocity
	ball.pos += ball.velocity;
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

	if (yCollision && xCollision) {
		currentBallState = ballState::STATE_BALL_PLAY;
		return true;
	}
	else {
		return false;
	}

}

void chestCollision()
{
	GameObject& ball{ Play::GetGameObjectByType(TYPE_BALL) };

	boolean xCollision = false;
	boolean yCollision = false;

	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };

	for (int i : chestIds) {
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

			isColliding = true;
			game.score += chestObj.CHEST_VALUE;
			Play::DestroyGameObject(i);


			break;

		}
		else {
			isColliding = false;
		}
	}

}

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

void loseCondition()
{

	if (game.lives == 0)
	{
		currentLevelState = levelState::STATE_GAMEOVER;
	}
	else
	{
		game.lives--;

		currentLevelState = levelState::STATE_START;
	}

}

void GameOver()
{
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_START;
		game.lives = MAX_LIVES;
		game.score = 0;
	}
}
