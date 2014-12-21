#ifndef __LITTLESNAKE_SCENE_H__
#define __LITTLESNAKE_SCENE_H__

#include "cocos2d.h"

const int BLOCK_PIXEL_SIZE = 40;	// width and height of the snake body & the each grid block
const int MAX_SCREEN_X = 44;    // 1760px
const int MAX_SCREEN_Y = 27;
const int MAP_LENGTH = 42;	// HD screen ratio with 40 * 40 blocks - offset.
const int MAP_HEIGHT = 23;

const int MIN_MAP_X = (MAX_SCREEN_X - MAP_LENGTH) / 2;
const int MAX_MAP_X = MIN_MAP_X + MAP_LENGTH;
const int MIN_MAP_Y = (MAX_SCREEN_Y - MAP_HEIGHT) / 2;
const int MAX_MAP_Y = MIN_MAP_Y + MAP_HEIGHT;

const float SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE = 0.02f;
const float GAME_FINISH_DELAY = 0.5f;
const int MAX_SNAKE_BODY_COUNT = 20;

const float SNAKE_MOVE_INTERVAL_EASY_MODE = 0.2f;
const int INITIAL_SNAKE_BODY_COUNT_EASY_MODE = 4;
const int RASPBERRY_TO_EAT_COUNT_EASY_MODE = 3;

const float SNAKE_MOVE_INTERVAL_HARDCORE_MODE = 0.1f;
const int INITIAL_SNAKE_BODY_COUNT_HARDCORE_MODE = 20;
const int RASPBERRY_TO_EAT_COUNT_HARDCORE_MODE = MAP_LENGTH * MAP_HEIGHT - (INITIAL_SNAKE_BODY_COUNT_HARDCORE_MODE + 1);  // 1 for head

typedef struct {
	cocos2d::Sprite *sprite;
	int row;
	int col;
} SpriteBody;

enum Direction {
    RIGHT = -90,
    LEFT = 90,
    UP = 180,
    DOWN = 0
};

enum GameState {
    INITIALIZED,
    PLAYING,
    DEAD,
    WIN,
    PLAYAGAIN,
    REINITIALIZED
};

class LittleSnake : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene(bool isEasyMode);
    virtual bool init();
    CREATE_FUNC(LittleSnake);

	void update(float dt);

	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

	bool isTouchDown;
	cocos2d::Point initialTouchPos;
	cocos2d::Point currentTouchPos;

private:
    GameState gameState;

	std::vector<SpriteBody*> snakeBodies;

	cocos2d::Size directorSize;
	cocos2d::Point directorOrigin;

    cocos2d::Menu *menu;

	SpriteBody *snakeHeadBody;
	SpriteBody *raspberryBody;

    Direction snakeDirection;
    Direction snakeNewDirection;

    cocos2d::Sprite *snakeStandardFace;
    cocos2d::Sprite *snakeYummyFace;
    cocos2d::Sprite *snakeDeadFace;
    cocos2d::Sprite *snakePlayAgainFace;

    int raspberryAteCount;
    cocos2d::Label *gameFinishLabel;

	float dtCount;

	float snakeMoveInterval;
	float raspberryToEatCount;
	float initialSnakeBodyCount;


    void setupForEasyMode();
	void setupForHardcoreMode();
    void setupCommon();

    void showGameFinishLabel(std::string text, cocos2d::Color3B color);

    void loadSnakeFaces();
    void addSnakeFacesAsChild();
    void hideSnakeFaces();

    void initializeSnake();
    void deleteSnake();

    void addSnakeBodySpriteBody(int row, int col);
    void spawnRaspberry();

	void processSwipe(float dt);
	void updateSnake(float dt);

	void renderSnake(float dt);
    void rotateSnakeHead(int angle);

    void updateSnakeFace(cocos2d::Sprite *snakeFace);

    void setGameStateToPlayAgain();
    void setGameStateToPlayAgainWithDelay();

    bool isSnakeEatingRaspberry();
    bool isSnakeCollidingWithRaspberry();
    bool isSnakeEatingItsOwnBody();
    bool isSnakeDead();

    void playAgain(cocos2d::Ref *sender);
    void exitScene(cocos2d::Ref *sender);

	cocos2d::Point getSpritePosWithBlockPos(int blockPosX, int blockPosY);
};

#endif // __LITTLESNAKE_SCENE_H__
