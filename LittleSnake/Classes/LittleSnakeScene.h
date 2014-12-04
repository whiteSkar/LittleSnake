#ifndef __LITTLESNAKE_SCENE_H__
#define __LITTLESNAKE_SCENE_H__

#include "cocos2d.h"

const int BLOCK_PIXEL_SIZE = 40;	// width and height of the snake body & the each grid block
const int MAX_MAP_X = 48;	// HD screen ratio with 40 * 40 blocks
const int MAX_MAP_Y = 27;	// TODO: remove / 2

const float SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE = 0.03;
const float SNAKE_MOVE_INTERVAL = 0.2;
const float GAME_FINISH_DELAY = 0.5;
const int INITIAL_SNAKE_BODY_COUNT = 4;
const int MAX_SNAKE_BODY_COUNT = 20;
const int EASY_MODE_RASPBERRY_COUNT = 1;

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
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
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

    void loadSnakeFaces();
    void hideSnakeFaces();
    void addSnakeFacesAsChild();
    void removeSnakeFacesAsChild();

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

    bool isSnakeEatingRaspberry();
    bool isSnakeCollidingWithRaspberry();
    bool isSnakeEatingItsOwnBody();
    bool isSnakeDead();

    void playAgain(cocos2d::Ref *sender);
    void exitScene(cocos2d::Ref *sender);

	cocos2d::Point getSpritePosWithBlockPos(int blockPosX, int blockPosY);
};

#endif // __LITTLESNAKE_SCENE_H__
