#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <math.h>
#include <iostream>
#include "Menu.h"
#include "mysql.h"
#include "HighScore.h"


#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif


#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

using namespace sf;
using namespace std;

const float pi = 3.14159f;

RenderWindow window;

Font font;
Text gameoverText;
Text lifeText;
Text scoreText;

Clock gameClock;
float deltaTime;

float frameWidth = 800;
float frameHeight = 800;

bool isPlaying = false;
bool gameover = false;
bool win = false;

int life = 3;
int level = 0;
int score = 0;
int combo = 0;

const float startposX = 55;
const float startposY = 70;

const enum BallDirection { UP, BOTTOM, LEFT, RIGHT };

Paddle paddle;
Ball ball;

Texture textureBall;
RectangleShape background;
Texture textureBack;
Texture texturePaddle;
Texture textureBrick;

SoundBuffer hitPaddleBuf;
SoundBuffer destroyBrickBuf;
SoundBuffer damageBrickBuf;
SoundBuffer bounceWallBuf;
SoundBuffer dieBuf;
SoundBuffer winBuf;
SoundBuffer loseBuf;
SoundBuffer BGMbuf;
Sound hitPaddleSound;
Sound destroyBrickSound;
Sound damageBrickSound;
Sound bounceWallSound;
Sound dieSound;
Sound winSound;
Sound loseSound;
Sound BGMSound;



vector<Brick*> bricks;

void gameInitiate();
void menuInitiate();
void gameReset();
void gameUpdate();
void gameRender();
void menuRender();
void gameHandleInput();
void menuHandleInput();
void highscoreHandleInput();
void gameLoadLevel(int level);
void gameHandleWin();
void gameDrawBrick(Brick* brick);
void gameHandleWallHitting();
void gameHandlePaddleHitting();
void gameHandleBrickHitting();
void gameHandleGameOver();
void gameHandleKeyboardPaddleMovement();
void getHighScores();

bool BallDir(RectangleShape rect, BallDirection direction);

const enum GAME_STATE { MENU, GAME, HIGHSCORE, EXIT };
GAME_STATE currentState = GAME_STATE::MENU;
bool isGameInitialized = false;
bool isHighScoreLoaded = false;

string highscores[5];
int main()
{
	
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |
		_CRTDBG_LEAK_CHECK_DF);
	window.create(VideoMode(frameWidth, frameHeight), "BREAKOUT");

	Menu menu(window.getSize().x, window.getSize().y);
	getHighScores();
	for (int i = 0; i < 5; i++) {
		cout << highscores[i] << endl;
	}
		while (window.isOpen())
		{
			deltaTime = gameClock.restart().asSeconds();
			

			if (currentState == GAME_STATE::GAME) {
				if (!isGameInitialized) {
					gameInitiate();
					gameLoadLevel(0);
					isGameInitialized = true;
				}
				gameHandleInput();
				if (isPlaying && !gameover && !win)
				{
					gameUpdate();
				}
				gameRender();

				//RenderMenu();
			}
			if (currentState == GAME_STATE::MENU) {
					window.clear(Color::Black);
					textureBack.loadFromFile("back.png");
					background.setSize(Vector2f(frameWidth, frameHeight));
					background.setPosition(0, 0);
					background.setTexture(&textureBack);
					window.draw(background);
					menu.draw(window);
					window.display();
					menuHandleInput();							
			}
			if (currentState == GAME_STATE::HIGHSCORE) {
				
					window.clear(Color::Black);
					textureBack.loadFromFile("back.png");
					background.setSize(Vector2f(frameWidth, frameHeight));
					background.setPosition(0, 0);
					background.setTexture(&textureBack);
					window.draw(background);

					HighScore highscore;
					highscore.draw(window, highscores);
					isHighScoreLoaded = true;
					window.display();
			
				highscoreHandleInput();
			}
			if (currentState == GAME_STATE::EXIT) {
				return EXIT_SUCCESS;
			}
		}
	
	return EXIT_SUCCESS;
}

void gameInitiate()
{
	font.loadFromFile("consola.ttf");
	textureBall.loadFromFile("ball.png");
	textureBack.loadFromFile("back.png");
	texturePaddle.loadFromFile("paddle.png");
	textureBrick.loadFromFile("brick.png");
	BGMbuf.loadFromFile("BGM.flac");
	BGMSound.setBuffer(BGMbuf);
	BGMSound.setLoop(true);
	BGMSound.play();
	hitPaddleBuf.loadFromFile("hitPaddle.wav");
	destroyBrickBuf.loadFromFile("destroyBrick.wav");
	damageBrickBuf.loadFromFile("damageBrick.wav");
	bounceWallBuf.loadFromFile("bounceWall.wav");
	dieBuf.loadFromFile("die.wav");
	winBuf.loadFromFile("win.wav");
	loseBuf.loadFromFile("lose.wav");
	hitPaddleSound.setBuffer(hitPaddleBuf);
	destroyBrickSound.setBuffer(destroyBrickBuf);
	damageBrickSound.setBuffer(damageBrickBuf);
	bounceWallSound.setBuffer(bounceWallBuf);
	dieSound.setBuffer(dieBuf);
	winSound.setBuffer(winBuf);
	loseSound.setBuffer(loseBuf);
	background.setSize(Vector2f(frameWidth, frameHeight));
	background.setPosition(0, 0);
	background.setTexture(&textureBack);
	lifeText.setFont(font);
	lifeText.setCharacterSize(20);
	lifeText.setPosition(620, frameHeight - 30);
	lifeText.setString("HP:" + to_string(life));
	gameoverText.setFont(font);
	gameoverText.setCharacterSize(35);
	gameoverText.setPosition(100, 400);
	gameoverText.setString("");
	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setPosition(80, frameHeight - 30);
	scoreText.setString("Wynik:" + to_string(score));
}

void menuInitiate() {
	window.clear(Color::Black);
	textureBack.loadFromFile("back.png");
	background.setSize(Vector2f(frameWidth, frameHeight));
	background.setPosition(0, 0);
	background.setTexture(&textureBack);
	window.draw(background);
	window.display();
}

void gameReset()
{
	Vector2f paddlePosition = paddle.picture.getPosition();
	ball.setPosition(paddlePosition.x, paddlePosition.y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + rand() % 60 - 30) * 2 * pi / 360;
}

void gameUpdate()
{
	if (ball.angle < 0)
	{
		ball.angle += 2 * pi;
	}
	ball.angle = fmod(ball.angle, 2 * pi);

	float factor = ball.speed * deltaTime;
	ball.picture.move(cos(ball.angle) * factor, sin(ball.angle) * factor);
	gameHandleWallHitting();
	gameHandlePaddleHitting();
	gameHandleBrickHitting();
	gameHandleGameOver();
	gameHandleWin();
	lifeText.setString("HP:" + to_string(life));
	scoreText.setString("Wynik:" + to_string(score));
}

void menuRender() {
	window.clear(Color::Black);
	window.draw(background);
	window.display();
}
void gameRender()
{
	window.clear(Color::Black);
	window.draw(background);
	window.draw(paddle.picture);
	window.draw(ball.picture);
	for (Brick* brick : bricks)
	{
		gameDrawBrick(brick);
	}
	window.draw(lifeText);
	window.draw(gameoverText);
	window.draw(scoreText);
	window.display();
}

void highscoreHandleInput() {
	Event event;
	while (window.pollEvent(event)) {
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			currentState = GAME_STATE::MENU;
		}
	}
}
void menuHandleInput() {
	Event event;
	while (window.pollEvent(event)) {
		if (Keyboard::isKeyPressed(Keyboard::Num1))
		{
			currentState = GAME_STATE::GAME;
		}
		if (Keyboard::isKeyPressed(Keyboard::Num2)) {
			currentState = GAME_STATE::HIGHSCORE;
		}
		if (Keyboard::isKeyPressed(Keyboard::Num3)) {
			currentState = GAME_STATE::EXIT;
		}
	}
}

void getHighScores() {

	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES* res;
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "localhost", "root", "", "cpp", 3306, NULL, 0);

	if (conn) {
		puts("success");

		string query = "SELECT * FROM cpp.test ORDER BY test DESC LIMIT 5";
		const char* q = query.c_str();
		int qstate = mysql_query(conn, q);
		if (!qstate) {
			res = mysql_store_result(conn);
			int i = 0;
			while (row = mysql_fetch_row(res)) {
				highscores[i] = row[1];
				i++;
			}
		}
	}
}

void gameHandleInput()
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed)
		{
			window.close();
			for (int i = 0; i < bricks.size(); ++i)
			{
				delete bricks[i];
				bricks[i] = nullptr;
			}
			bricks.clear();
		}
		else if (event.type == Event::MouseMoved && !gameover && !win)
		{
			if (Mouse::getPosition(window).x < (frameWidth - 100.f) && Mouse::getPosition(window).x  > 100.f)
			{
				paddle.picture.setPosition(Vector2f(event.mouseMove.x, paddle.picture.getPosition().y));
			}
			if (!isPlaying)
			{
				ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
			}
		}
	}

	gameHandleKeyboardPaddleMovement();

	
		if (gameover)
		{
			life = 3;
			gameover = false;
			score = 0;
			combo = 0;
			gameLoadLevel(level);
			BGMSound.play();
		}
		else if (win)
		{
			win = false;
			level = (level + 1) % 3;
			gameLoadLevel(level);
			BGMSound.play();
		}
	
}


void gameLoadLevel(int level)
{
	isPlaying = false;
	gameover = false;
	gameoverText.setString("");
	paddle.initiate();
	paddle.setSize(150, 35);
	paddle.setPosition(frameWidth / 2, frameHeight - paddle.picture.getSize().y);
	paddle.picture.setTexture(&texturePaddle);

	ball.initiate();
	ball.setSize(10);
	ball.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	ball.angle = (270 + rand() % 60 - 30) * 2 * pi / 360;
	ball.picture.setTexture(&textureBall);

	for (int i = 0; i < bricks.size(); ++i)
	{
		delete bricks[i];
		bricks[i] = nullptr;
	}
	bricks.clear();

	if (level == 0)
	{

		for (int i = 0; i < 10; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 10; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 3 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + 6 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 10; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 9 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 1;
			bptr->speed = 400;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 4; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 11 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 4; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x + 6 * bptr->picture.getSize().x, startposY + 11 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}
	}
	else if (level == 1)
	{

		for (int i = 0; i < 5; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 10 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2 - i * bptr->picture.getSize().y);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 5; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x + 5 * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y + 6 * bptr->picture.getSize().y);
			bptr->hp = 1;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x + 4 * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x + 4 * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y + 4 * bptr->picture.getSize().y);
				bptr->hp = 2;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				Brick* bptr = new Brick;
				bptr->initiate();
				bptr->setSize(70, 30);
				bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y + 2 * bptr->picture.getSize().y);
				bptr->hp = 1;
				bptr->speed = 300;
				bricks.push_back(bptr);

			}
		}

		for (int i = 0; i < 4; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x, startposY + 11 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}

		for (int i = 0; i < 4; i++)
		{
			Brick* bptr = new Brick;
			bptr->initiate();
			bptr->setSize(70, 30);
			bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + i * bptr->picture.getSize().x + 6 * bptr->picture.getSize().x, startposY + 11 * bptr->picture.getSize().y + bptr->picture.getSize().y / 2);
			bptr->hp = 99999;
			bricks.push_back(bptr);

		}

	}
	else if (level == 2)
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				int temp = rand() % 5;
				if (temp == 0)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 1;
					bricks.push_back(bptr);
				}
				else if (temp == 1)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 2;
					bricks.push_back(bptr);
				}
				else if (temp == 2)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 99999;
					bricks.push_back(bptr);
				}
				else if (temp == 3)
				{
					Brick* bptr = new Brick;
					bptr->initiate();
					bptr->setSize(70, 30);
					bptr->setPosition(startposX + bptr->picture.getSize().x / 2 + j * bptr->picture.getSize().x, startposY + bptr->picture.getSize().y / 2 + i * bptr->picture.getSize().y);
					bptr->hp = 1;
					bptr->speed = 300;
					bricks.push_back(bptr);
				}

			}
		}

	}
}

bool BallDir(RectangleShape rect, BallDirection direction)
{
	Vector2f ballPosition = ball.picture.getPosition();
	float ballRadius = ball.picture.getRadius();
	Vector2f rectPosition = rect.getPosition();
	Vector2f rectSize = rect.getSize();

	switch (direction) {
	case BallDirection::LEFT: {
		return ballPosition.x + ballRadius > rectPosition.x - rectSize.x / 2 &&
			ballPosition.x + ballRadius < rectPosition.x + rectSize.x / 2 &&
			ballPosition.y + ballRadius >= rectPosition.y - rectSize.y / 2 &&
			ballPosition.y - ballRadius <= rectPosition.y + rectSize.y / 2;
	}
	case BallDirection::RIGHT: {
		return ballPosition.x - ballRadius > rectPosition.x - rectSize.x / 2 &&
			ballPosition.x - ballRadius < rectPosition.x + rectSize.x / 2 &&
			ballPosition.y + ballRadius >= rectPosition.y - rectSize.y / 2 &&
			ballPosition.y - ballRadius <= rectPosition.y + rectSize.y / 2;
	}
	case BallDirection::UP: {
		return ballPosition.x + ballRadius >= rectPosition.x - rectSize.x / 2 &&
			ballPosition.x - ballRadius <= rectPosition.x + rectSize.x / 2 &&
			ballPosition.y - ballRadius < rectPosition.y + rectSize.y / 2 &&
			ballPosition.y - ballRadius > rectPosition.y - rectSize.y / 2;
	}
	case BallDirection::BOTTOM: {
		return ballPosition.x + ballRadius >= rectPosition.x - rectSize.x / 2 &&
			ballPosition.x - ballRadius <= rectPosition.x + rectSize.x / 2 &&
			ballPosition.y + ballRadius < rectPosition.y + rectSize.y / 2 &&
			ballPosition.y + ballRadius > rectPosition.y - rectSize.y / 2;
	}
	}
}

void gameHandleWallHitting()
{
	CircleShape ballPicture = ball.picture;
	Vector2f position = ballPicture.getPosition();
	float radius = ballPicture.getRadius();
	if (position.y + radius > frameHeight)
	{
		isPlaying = false;
		life--;
		dieSound.play();
		gameReset();
	}
	else if (position.x - radius < 50.f)
	{
		ball.angle = pi - ball.angle;
		ball.picture.setPosition(radius + 50.1f, position.y);
		bounceWallSound.play();
	}
	else if (position.x + radius > frameWidth - 50)
	{
		ball.angle = pi - ball.angle;
		ball.setPosition(frameWidth - radius - 50.1f, position.y);
		bounceWallSound.play();
	}
	else if (position.y - radius < 50.f)
	{
		ball.angle = -ball.angle;
		ball.setPosition(position.x, radius + 50.1f);
		bounceWallSound.play();
	}
}

void gameHandleWin() {
	int count = 0;
	for (Brick* brick : bricks)
	{
		if (brick->enable && brick->hp < 3)
			count++;
	}

	if (count <= 0)
	{
		win = true;
		ball.speed += 100.f;
		BGMSound.pause();
		winSound.play();
		gameoverText.setString("Wygrana! Enter - kolejny poziom");
	}
}

void gameDrawBrick(Brick* brick) {

	if (!brick->enable)
	{
		return;
	}
	if (brick->hp == 1)
	{
		brick->picture.setTexture(&textureBrick);
		brick->picture.setFillColor(Color::Color(0, 255, 255, 255));
	}
	else if (brick->hp == 2)
	{
		brick->picture.setTexture(&textureBrick);
		brick->picture.setFillColor(Color::Color(255, 0, 0, 255));
	}
	else
	{
		brick->picture.setTexture(&textureBrick);
		brick->picture.setFillColor(Color::Color(255, 255, 255, 255));
	}
	window.draw(brick->picture);
}

void gameHandlePaddleHitting() {
	if (!BallDir(paddle.picture, BallDirection::BOTTOM))
	{
		return;
	}
	int dis = ball.picture.getPosition().x - paddle.picture.getPosition().x;
	if (dis > 30 && ball.angle > 1.f / 2.f * pi)
	{
		ball.angle = ball.angle - pi;
	}
	else if (dis < -30 && ball.angle < 1.f / 2.f * pi)
	{
		ball.angle = ball.angle - pi;
	}
	else
	{
		ball.angle = -ball.angle;
		if (ball.angle > 1.f / 2.f * pi && ball.angle < 7.f / 8.f * pi)
		{
			ball.angle += (rand() % 15) * pi / 180;
		}
		else if (ball.angle < 1.f / 2.f * pi && ball.angle > 1.f / 8.f * pi)
		{
			ball.angle -= (rand() % 15) * pi / 180;
		}
		else if (ball.angle <= 1.f / 8.f * pi)
		{
			ball.angle += (rand() % 15) * pi / 180;
		}
		else if (ball.angle >= 7.f / 8.f * pi)
		{
			ball.angle -= (rand() % 15) * pi / 180;
		}
	}

	combo = 0;
	ball.setPosition(ball.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
	hitPaddleSound.play();
}

void gameHandleBrickHitting() {
	for (Brick* brick : bricks)
	{
		if (!brick->enable) {
			continue;
		}
		if (brick->speed != 0.f)
		{
			float brickPositionX = brick->picture.getPosition().x;
			float brickSizeX = brick->picture.getSize().x;
			if (brickPositionX - brickSizeX / 2 < 50.f)
				brick->moveLeft = false;
			else if (brickPositionX + brickSizeX / 2 > frameWidth - 50.f)
				brick->moveLeft = true;

			if (brick->moveLeft)
				brick->picture.move(-brick->speed * deltaTime, 0.0f);
			else
				brick->picture.move(brick->speed * deltaTime, 0.0f);
		}
		if (BallDir(brick->picture, BallDirection::UP))
		{
			ball.angle = -ball.angle;
			ball.setPosition(ball.picture.getPosition().x, brick->picture.getPosition().y + brick->picture.getSize().y / 2 + ball.picture.getRadius() + 0.1f);
			if (brick->hit())
			{
				destroyBrickSound.play();
			}
			else
			{
				damageBrickSound.play();
			}
			combo++;
			score = score + combo * 10;
		}
		else if (BallDir(brick->picture, BallDirection::BOTTOM))
		{
			ball.angle = -ball.angle;
			ball.setPosition(ball.picture.getPosition().x, brick->picture.getPosition().y - brick->picture.getSize().y / 2 - ball.picture.getRadius() - 0.1f);
			if (brick->hit())
			{
				destroyBrickSound.play();
			}
			else
			{
				damageBrickSound.play();
			}
			combo++;
			score = score + combo * 10;
		}
		else if (BallDir(brick->picture, BallDirection::LEFT))
		{
			ball.angle = pi - ball.angle;
			ball.setPosition(brick->picture.getPosition().x + ball.picture.getRadius() + brick->picture.getSize().x / 2 + 0.1f, ball.picture.getPosition().y);
			if (brick->hit())
			{
				destroyBrickSound.play();
			}
			else
			{
				damageBrickSound.play();
			}
			combo++;
			score = score + combo * 10;
		}
		else if (BallDir(brick->picture, BallDirection::RIGHT))
		{
			ball.angle = pi - ball.angle;
			ball.setPosition(brick->picture.getPosition().x - ball.picture.getRadius() - brick->picture.getSize().x / 2 - 0.1f, ball.picture.getPosition().y);
			if (brick->hit())
			{
				destroyBrickSound.play();
			}
			else
			{
				damageBrickSound.play();
			}
			combo++;
			score = score + combo * 10;
		}
	}
}

void gameHandleGameOver() {
	if (life <= 0)
	{
		gameover = true;
		BGMSound.pause();
		loseSound.play();

		MYSQL* conn;
		MYSQL_ROW row;
		MYSQL_RES* res;
		conn = mysql_init(0);
		conn = mysql_real_connect(conn, "localhost", "root", "", "cpp", 3306, NULL, 0);

		string query1 = "INSERT INTO cpp.test (`id`, `test`) VALUES (NULL, '" + std::to_string(score) +"')";
		const char* d = query1.c_str();
		int state = mysql_query(conn, d);

		getHighScores();

		currentState = GAME_STATE::MENU;
		gameoverText.setString("Przegrana! Enter - nowa gra");
	}
}

void gameHandleKeyboardPaddleMovement() {
	if (gameover) {
		return;
	}
	if ((Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) &&
		(paddle.picture.getPosition().x - paddle.picture.getSize().x / 2.f > 50.f))
	{
		paddle.picture.move(-paddle.speed * deltaTime, 0.f);
	}
	if ((Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) &&
		(paddle.picture.getPosition().x + paddle.picture.getSize().x / 2.f < frameWidth - 50.f))
	{
		paddle.picture.move(paddle.speed * deltaTime, 0.f);
	}
	if (Keyboard::isKeyPressed(Keyboard::Space) || Mouse::isButtonPressed(Mouse::Left))
	{
		isPlaying = true;
	}
	if (!isPlaying)
	{
		ball.picture.setPosition(paddle.picture.getPosition().x, paddle.picture.getPosition().y - paddle.picture.getSize().y / 2 - ball.picture.getRadius());
	}
}
