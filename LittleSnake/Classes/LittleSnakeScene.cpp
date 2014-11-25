#include "LittleSnakeScene.h"

USING_NS_CC;

Scene* LittleSnake::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LittleSnake::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LittleSnake::init()
{
    if (!Layer::init())
    {
        return false;
    }

	srand(time(NULL));
    
    directorSize = Director::getInstance()->getVisibleSize();
    directorOrigin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("background.png");
    background->setScale(0.5);  // TODO: remove later
    background->setPosition(directorOrigin.x + background->getBoundingBox().size.width/2, directorOrigin.y + background->getBoundingBox().size.height/2);
    this->addChild(background);

	snakeBodies = std::vector<SpriteBody*>();

	snakeHead = Sprite::create("SnakeHeadStandard.png");
    snakeHead->setRotation(-90);
	this->addChild(snakeHead);

	SpriteBody *snakeHeadBody = new SpriteBody();   // check if i need to delete these when restarting
	snakeHeadBody->sprite = snakeHead;
	snakeBodies.push_back(snakeHeadBody);

	snakeHeadBody->row = MAX_MAP_Y / 2;
	snakeHeadBody->col = MAX_MAP_X / 2;
	snakeHeadBody->sprite->setPosition(getSpritePosWithBlockPos(snakeHeadBody->row, snakeHeadBody->col));

    for (int i = 0; i < INITIAL_SNAKE_BODY_COUNT; ++i)
    {
        auto snakeBody = Sprite::create("SnakeBody.png");
        this->addChild(snakeBody);

        auto *snakeBodyBody = new SpriteBody();
        snakeBodyBody->sprite = snakeBody;
        snakeBodies.push_back(snakeBodyBody);

        snakeBodyBody->row = snakeHeadBody->row;
        snakeBodyBody->col = snakeHeadBody->col - (i+1);
        snakeBodyBody->sprite->setPosition(getSpritePosWithBlockPos(snakeBodyBody->row, snakeBodyBody->col));
    }

	raspberry = Sprite::create("Raspberry.png");
	//check collision with snake body when spawning
	SpriteBody *raspberryBody = new SpriteBody();
	raspberryBody->sprite = raspberry;
	raspberryBody->row = rand() % MAX_MAP_Y;
	raspberryBody->col = rand() % MAX_MAP_X;
	raspberryBody->sprite->setPosition(getSpritePosWithBlockPos(raspberryBody->row, raspberryBody->col));
	this->addChild(raspberry);
    
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);

	touchListener->onTouchBegan = CC_CALLBACK_2(LittleSnake::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(LittleSnake::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(LittleSnake::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(LittleSnake::onTouchCancelled, this);
	
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
	
	isTouchDown = false;
	initialTouchPos = Point::ZERO;
	this->currentTouchPos = Point::ZERO;

	this->schedule(schedule_selector(LittleSnake::updateSnake), SNAKE_MOVE_INTERVAL, kRepeatForever, 1);
	this->scheduleUpdate();
    
    return true;
}

void LittleSnake::update(float dt)
{
	processTouch(dt);
}

void LittleSnake::processTouch(float dt)
{
	if (isTouchDown)
	{
		float swipeHorizontalThresholdDistance = directorSize.width * SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE;
		float swipeVerticalThresholdDistance = directorSize.height * SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE;

		if (currentTouchPos.x - initialTouchPos.x > swipeHorizontalThresholdDistance)
		{
			log("Swipe Right");
			isTouchDown = false;
		}
		else if (currentTouchPos.x - initialTouchPos.x < -swipeHorizontalThresholdDistance)
		{
			log("Swipe Left");
			isTouchDown = false;
		}
		else if (currentTouchPos.y - initialTouchPos.y > swipeVerticalThresholdDistance)
		{
			log("Swipe Up");
			isTouchDown = false;
		}
		else if (currentTouchPos.y - initialTouchPos.y < -swipeVerticalThresholdDistance)
		{
			log("Swipe Down");
			isTouchDown = false;
		}
	}
}

void LittleSnake::updateSnake(float dt)
{
	//Point curSnakePos = snake->getPosition();
	//snake->setPosition(curSnakePos.x + BLOCK_PIXEL_SIZE, curSnakePos.y);
	// take care of multiple bodies in one spot

	for (auto snakeBody : snakeBodies)
	{
		snakeBody->col++;
	}

	renderSnake(dt);	// temporary place
}

bool LittleSnake::onTouchBegan(Touch* touch, Event* event)
{
	initialTouchPos = touch->getLocation();
	currentTouchPos = touch->getLocation();
	
	isTouchDown = true;

	return true;
}

void LittleSnake::onTouchMoved(Touch* touch, Event* event)
{
	currentTouchPos = touch->getLocation();
}

void LittleSnake::onTouchEnded(Touch* touch, Event* event)
{
	isTouchDown = false;
}

void LittleSnake::onTouchCancelled(Touch* touch, Event* event)
{
	onTouchEnded(touch, event);
}

void LittleSnake::renderSnake(float dt)
{
	// remove map and iterate through snakebodies to render snake
	auto snakeHalfSize = snakeHead->getBoundingBox().size.width / 2;

	for (auto it = snakeBodies.begin(); it != snakeBodies.end(); ++it)
	{
		SpriteBody *snakeBody = *it;
		snakeBody->sprite->setPosition(getSpritePosWithBlockPos(snakeBody->row, snakeBody->col));
	}
}

Point LittleSnake::getSpritePosWithBlockPos(int row, int col)
{
	// Assumes all sprites have the same pixel size as the block
	int y = directorOrigin.y + row * BLOCK_PIXEL_SIZE + BLOCK_PIXEL_SIZE/2;
	int x = directorOrigin.x + col * BLOCK_PIXEL_SIZE + BLOCK_PIXEL_SIZE/2;
	return Point(x, y);
}
