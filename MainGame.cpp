#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Header.h"

//Globals
int score = 0;
int chestCount = 24;

//Game Starting Point
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	//Starting Object Creation and Game Values Set
	InitialCreation();
	GetStartingValues();
}

//Called Every Frame
bool MainGameUpdate(float elapsedTime)
{
	//Game State Switch
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

	case levelState::STATE_WIN:
		GameWin();
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
void Draw()
{
	// Reset drawing buffer 
	Play::ClearDrawingBuffer(Play::cWhite);
	Play::DrawBackground();

	//If the game is in start or play, draw the HUD and Objects
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

	//If No More Chests 
	if (currentLevelState == levelState::STATE_WIN)
	{
		Play::DrawFontText("64px", "You Win! Congratulations", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2), Play::CENTRE);
		Play::DrawFontText("64px", "Current Score: " + std::to_string(game.score), Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 50), Play::CENTRE);
		Play::DrawFontText("64px", "Hit Space To Continue", Point2D(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 + 150), Play::CENTRE);
	}
	//Present Current Objects
	Play::PresentDrawingBuffer();
}

//Draw all the individual objects
void DrawObjects()
{
	//Draw Web
	Play::DrawLine({ 0, WEB_HEIGHT }, { DISPLAY_WIDTH, WEB_HEIGHT }, Play::cWhite);
	//Draw Agent / Player
	Play::DrawObject(Play::GetGameObjectByType(TYPE_AGENT));
	//Draw Ball
	Play::DrawObjectRotated(Play::GetGameObjectByType(TYPE_BALL));

	//Fetching all the chests and assigning them to ID's
	std::vector<int> chestIds{ Play::CollectGameObjectIDsByType(TYPE_CHEST) };

	//Fetching all the coins and assigning them to ID's
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };

	//Iterate over every coin ID
	//Get the coin ID and draw the chest
	for (int j : coinIds)
	{
		GameObject& coin = Play::GetGameObject(j);
		Play::DrawObject(coin);
	}

	//Iterate over every chest ID
	//Get the chest ID and draw the chest
	for (int i : chestIds)
	{
		GameObject& chest = Play::GetGameObject(i);
		Play::DrawObject(chest);
	}
}

//Creates the rows of chests
void CreateChests()
{
	//Needed for when called after game has been won and reset
	chestObj.chestHeight = 0;
	chestCount = 24;

	//Get chests vector
	std::vector<int> chestIds = Play::CollectGameObjectIDsByType(TYPE_CHEST);
	//While there is less than 24, draw a chest 
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

//Drawing the HUD objects
void DrawHud()
{
	//Add Score and Life Counter
	Play::DrawFontText("64px", "Lives: " + std::to_string(game.lives), Point2D(display.LIVES_POS_X, display.LIVES_POS_Y), Play::CENTRE);
	Play::DrawFontText("64px", "Score: " + std::to_string(game.score), Point2D(display.SCORE_POS_X, display.SCORE_POS_Y), Play::CENTRE);
}

//COLLISION CODE

//For if the ball collides with the top or sides of the screen
void SidesAndTop()
{
	GameObject& ballObj{ Play::GetGameObjectByType(TYPE_BALL) };

	//Clamp is used to bounce the ball back if it touches the sides of the screen.
	if (ballObj.pos.x < 0 || ballObj.pos.x > DISPLAY_WIDTH)
	{
		ballObj.pos.x = std::clamp(ballObj.pos.x, 0.f, DISPLAY_WIDTH);
		ballObj.velocity.x *= -1;

		ballObj.velocity += ballObj.acceleration;
		ballObj.pos += ballObj.velocity;
	}

	//If ball hits the top of the screen, invert the velocity so the ball now moves towards the player
	if (ballObj.pos.y < SCREEN_START_HEIGHT)
	{
		ballObj.velocity.y *= -1;
	}

	//Adjusts the velocity and rotation for when it touches 
	ballObj.pos += ballObj.velocity;
	ballObj.rotation += 0.05;
}

//Uses AABB on the ball and the player to detect collision
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

//Uses AABB on the ball and the chest to detect collision
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

			//If there is collision with the chest the score is increased
			//A coin is created and added to the coins vector
			//The chest is then destroyed and teh chest counter decremented
			game.score += chestObj.CHEST_VALUE;
			int coinId = Play::CreateGameObject(TYPE_COIN, chest.pos, RADIUS, "coin");
			coinIds.push_back(coinId);
			Play::DestroyGameObject(i);
			chestCount--;
			return true;
		}
	}
	// Return false only if no collisions
	return false;
}

//Uses AABB on the coin and the player to detect collision
boolean CoinCollision()
{
	GameObject& player{ Play::GetGameObjectByType(TYPE_AGENT) };
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };

	for (int i : coinIds)
	{
		GameObject& coin = Play::GetGameObject(i);
		boolean xCollision = false;
		boolean yCollision = false;

		if (coin.pos.y + coinObj.COIN_AABB.y > player.pos.y - playerObj.PLAYER_AABB.y
			&& coin.pos.y - coinObj.COIN_AABB.y < player.pos.y + playerObj.PLAYER_AABB.y)
		{
			if (coin.pos.x + coinObj.COIN_AABB.x > player.pos.x - playerObj.PLAYER_AABB.x
				&& coin.pos.x - coinObj.COIN_AABB.x < player.pos.x + playerObj.PLAYER_AABB.x)
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
			//If coin and the player collide destroy the coin
			Play::DestroyGameObject(i);
			return true;
		}
	}

	// Return false only if no collision
	return false;
}

//Used when the ball bounces with the player paddle
void BallBounce()
{
	//Set the bounce to right initially 
	GameObject& ballObj = Play::GetGameObjectByType(TYPE_BALL);
	float horizontalDirection = BOUNCE_RIGHT; 

	//If the ball is more to the right of the paddle then bounce it right
	if (ballObj.pos.x < Play::GetGameObjectByType(TYPE_AGENT).pos.x)
	{
		horizontalDirection = BOUNCE_LEFT;
	}

	//Velocity and direction changed when hitting player
	ballObj.velocity.y = -10.0f; 
	ballObj.velocity.x = horizontalDirection * 3.0f; 

	ballObj.pos += ballObj.velocity;
}

//Inverts the current direction
void BallChangeDirection()
{
	GameObject& ballObj = Play::GetGameObjectByType(TYPE_BALL);
	ballObj.velocity.y *= -1;
}

//PLAYER CONTROL
void HandlePlayerControls()
{
	//Get the player object and set the default values needed
	GameObject& agent{ Play::GetGameObjectByType(TYPE_AGENT) };
	agent.acceleration = Vector2D(0.0f, 0.f);
	float accelerationIncrease = 0.5f;
	float deceleration = 0.2f;

	//Direction control
	if (Play::KeyDown(VK_LEFT))
	{
		agent.acceleration.x -= accelerationIncrease;
	}

	if (Play::KeyDown(VK_RIGHT))
	{
		agent.acceleration.x += accelerationIncrease;
	}

	//Deacceleration if keys let go
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
		
	//Sprite changes for key presses
	if (Play::KeyDown(VK_LEFT) || Play::KeyDown(VK_RIGHT))
	{
		Play::SetSprite(agent, "agent8_climb", 0.25f);
	}
	else
	{
		Play::SetSprite(agent, "agent8_halt", 0.333f);
	}

	//Changes to velocity and position based on key press
	agent.velocity += agent.acceleration;
	agent.pos += agent.velocity;

	//Stops the player going off screen
	if (agent.pos.x < 0 || agent.pos.x > DISPLAY_WIDTH)
	{
		agent.pos.x = std::clamp(agent.pos.x, 0.f, DISPLAY_WIDTH);
		//Stops the player object when at edge of screen
		agent.velocity.x = 0.0f;
	}

	//If Space Pressed the game is paused
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PAUSE;
	}
}

//GAME START
//Creates all the starting objects and game window
void InitialCreation()
{
	Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
	Play::LoadBackground("Data\\Backgrounds\\background.png");
	Play::CentreAllSpriteOrigins(); // this function makes it so that obj.pos values represent the center of a sprite instead of its top-left corner
	Play::CreateGameObject(TYPE_AGENT, { playerObj.PLAYER_STARTING_X, playerObj.PLAYER_STARTING_Y }, RADIUS, "agent");
	Play::CreateGameObject(TYPE_BALL, { ballObj.BALL_STARTING_X, ballObj.BALL_STARTING_X}, RADIUS, "ball");
	
	CreateChests();
}

void GetStartingValues() {
	score = 0;
	GameObject& ball = Play::GetGameObjectByType(TYPE_BALL);
	ball.acceleration = Vector2D(0.f, 0.55f);
}

//Called By Switch
void GameStart()
{
	//IF space pressed then state changed to play
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
	}
}

//Called By Switch
void GamePlay()
{
	HandlePlayerControls();
	SidesAndTop();
	CoinMovement();


	//Boolean flag for collision
	//method checks each frame
	boolean collision = BallCollision();
	//If there is a collision then bounce the ball
	if (collision) {
		BallBounce();
	}

	//Boolean flag for collision
	//method checks each frame
	boolean CCollision = ChestCollision();
	//If there is a collision on the chest then the ball will change direction towards the player
	if (CCollision) {
		BallChangeDirection();
	}

	//Boolean flag for collision
	//method checks each fram
	boolean coinCollision = CoinCollision();
	//If the coin and player collide the player collects the coin
	if (coinCollision)
	{
		CollectCoin();
	}

	//Checks if out of bounds 
	boolean outOfBounds = OutOfBoundsChecker();
	//If out of bounds start lose condition
	if (outOfBounds)
	{
		LoseCondition();
	}
	
	//Boolean flag for if there are any chests left
	boolean hasThePlayerWon = ChestChecker();
	//If there are no chests left on screen win condition is called
	if (hasThePlayerWon)
	{
		WinCondition();
	}
}

//Called by Switch
void GamePause()
{
	//If space is pressed go backto game play
	if (Play::KeyPressed(VK_SPACE))
	{
		currentLevelState = levelState::STATE_PLAY;
	}
}

//Called by off screen checker
void LoseCondition()
{
	//If no lives switch to GameOver
	if (game.lives == 0)
	{
		currentLevelState = levelState::STATE_GAMEOVER;
	}
	//Decrement Lives, switch game back to starting state.
	//Put ball back in starting position
	else
	{
		game.lives--;
		ResetPositions();
		currentLevelState = levelState::STATE_START;
	}
}

//Called by Switch
void GameOver()
{
	//If space pressed reset the game 
	//Set state to start
	if (Play::KeyPressed(VK_SPACE))
	{
		ResetGame();
		currentLevelState = levelState::STATE_START;
	}
}

//Called when no chests are left on the screen
void GameWin()
{
	//If space pressed
	//Reset the game but score and lives remain
	//Set state to start
	if (Play::KeyPressed(VK_SPACE))
	{
		ResetGameWin();
		currentLevelState = levelState::STATE_START;
	}
}

//Controls the coins position
void CoinMovement()
{
	//Get all the coins and iterate over them
	std::vector<int> coinIds{ Play::CollectGameObjectIDsByType(TYPE_COIN) };
	for (int coinId : coinIds) {
		GameObject& coin = Play::GetGameObject(coinId);

		// Move the coin down the screen
		coin.pos.y += coinObj.COIN_SPEED; 

		// Remove off-screen coins
		// + 25 to account for sprite position being in the middle
		if (coin.pos.y > DISPLAY_HEIGHT + 25) {
			Play::DestroyGameObject(coinId);
		}
	}

}

//Checks if ball has left bottom og the screen
boolean OutOfBoundsChecker()
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

//Sets the ball and player poesition back to the centre
void ResetPositions() 
{
	GameObject& ball = Play::GetGameObjectByType(TYPE_BALL);
	ball.pos.x = ballObj.BALL_STARTING_X;
	ball.pos.y = ballObj.BALL_STARTING_Y;

	GameObject& player = Play::GetGameObjectByType(TYPE_AGENT);
	player.pos.x = playerObj.PLAYER_STARTING_X;
	player.pos.y = playerObj.PLAYER_STARTING_Y;
}

//Reset the game completely
void ResetGame() 
{
	game.lives = MAX_LIVES;
	game.score = 0;

	// Reset the ball's position
	ResetPositions();

	// Remove all existing coins
	std::vector<int> coinIds = Play::CollectGameObjectIDsByType(TYPE_COIN);
	for (int coinId : coinIds) {
		Play::DestroyGameObject(coinId);
	}

	//Remove all existing chests 
	std::vector<int> chestIds = Play::CollectGameObjectIDsByType(TYPE_CHEST);
	for (int chestId : chestIds) {
		Play::DestroyGameObject(chestId);
	}

	// Recreate chests and coins
	CreateChests();
}

//Called when the player clears all chests
//Score and lives remain
void ResetGameWin()
{
	// Reset the ball's position
	ResetPositions();

	// Remove all existing coins
	std::vector<int> coinIds = Play::CollectGameObjectIDsByType(TYPE_COIN);
	for (int coinId : coinIds) {
		Play::DestroyGameObject(coinId);
	}

	// Recreate chests 
	CreateChests();
}

//If a coin is collected increase the score 
void CollectCoin()
{
	game.score += coinObj.COIN_VALUE;
}

//If the player reaches the win condition, switch to the win 
void WinCondition()
{
	currentLevelState = levelState::STATE_WIN;
}

//Used to check how many chest remain
//If there are no chests then return true
boolean ChestChecker()
{
	if (chestCount == 0) {
		return true; // All chests are gone
	}
	else {
		return false; // There are still chests left
	}
}