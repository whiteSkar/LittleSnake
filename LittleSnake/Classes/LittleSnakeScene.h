#ifndef __LITTLESNAKE_SCENE_H__
#define __LITTLESNAKE_SCENE_H__

#include "cocos2d.h"

const int BLOCK_PIXEL_SIZE = 40;	// width and height of the snake body & the each grid block
const int MAX_MAP_X = 48/2;	// HD screen ratio with 40 * 40 blocks
const int MAX_MAP_Y = 27/2;	// TODO: remove / 2

const float SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE = 0.10;
const float SNAKE_MOVE_INTERVAL = 0.5;
const int INITIAL_SNAKE_BODY_COUNT = 4;
const int MAX_SNAKE_BODY_COUNT = 20;

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
	std::vector<SpriteBody*> snakeBodies;

	cocos2d::Size directorSize;
	cocos2d::Point directorOrigin;

	SpriteBody *snakeHeadBody;
	cocos2d::Sprite *raspberry;

    Direction snakeDirection;

	void processTouch(float dt);
	void updateSnake(float dt);

	void renderSnake(float dt);

	cocos2d::Point getSpritePosWithBlockPos(int blockPosX, int blockPosY);
};

#endif // __LITTLESNAKE_SCENE_H__