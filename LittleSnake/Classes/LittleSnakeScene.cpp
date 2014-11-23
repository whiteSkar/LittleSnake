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
    
    directorSize = Director::getInstance()->getVisibleSize();
    directorOrigin = Director::getInstance()->getVisibleOrigin();

	this->clearMap();

	snake = Sprite::create("SnakeBody.png");
	snake->setPosition(directorOrigin);
	this->addChild(snake);
    
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

	this->scheduleUpdate();
    
    return true;
}

void LittleSnake::update(float dt)
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

void LittleSnake::clearMap()
{
	for (int i = 0; i < MAX_MAP_X; ++i)
		for (int j = 0; j < MAX_MAP_Y; ++j)
			map[i][j] = new std::vector<Sprite>();
}