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

/*
 * - Menu that has eays mode and hardcore mode
 * -- Easy mode starts with slower speed, less snake bodies, and fixed # of raspberries to eat
 * -- Hardcore mode starts faster pseed, longer snake bodies, and needs to eat until the snake fills the entire screen
 * --- When the number of snake bodies + 1 (head) >= number of grids, they win (make sure spawn raspberry method is not called this case. it will go into infinite loop)
 * - A way to go back to the menu possibly
 * - When win, show something.
 * - show number of raspberries eaten
 * - Do no automatically start the game. It starts while transitioning.. Or give it more delay
 */

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
    //background->setScale(0.5);  // TODO: remove later
    background->setPosition(directorOrigin.x + background->getBoundingBox().size.width/2, directorOrigin.y + background->getBoundingBox().size.height/2);
    this->addChild(background);

    loadSnakeFaces();
    addSnakeFacesAsChild();

    initializeSnake();

    raspberryAteCount = 0;

    raspberryBody = nullptr;
    spawnRaspberry();
    
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

    gameState = PLAYING;
    
    return true;
}

void LittleSnake::initializeSnake()
{
    snakeDirection = RIGHT;
    snakeNewDirection = RIGHT;
    snakeBodies = std::vector<SpriteBody*>();

	snakeHeadBody = new SpriteBody();   // check if i need to delete these when restarting
    snakeHeadBody->row = MAX_MAP_Y / 2;
	snakeHeadBody->col = MAX_MAP_X / 2;

    updateSnakeFace(snakeStandardFace);
    rotateSnakeHead(snakeDirection);

    for (int i = 0; i < INITIAL_SNAKE_BODY_COUNT; ++i)
    {
        addSnakeBodySpriteBody(snakeHeadBody->row, snakeHeadBody->col - (i+1));
    }

    renderSnake(0);
}

void LittleSnake::deleteSnake()
{
    for (int i = 0; i < snakeBodies.size(); ++i)
    {
        this->removeChild(snakeBodies[i]->sprite);
        delete(snakeBodies[i]);
    }

    hideSnakeFaces();   // should delete. Refactor to have both hide && remove
    snakeBodies.clear();
}

void LittleSnake::update(float dt)
{
    if (gameState != PLAYING) 
    {
        return;
    }

	processSwipe(dt);

    rotateSnakeHead(snakeNewDirection);
}

void LittleSnake::processSwipe(float dt)
{
	if (isTouchDown)
	{
		float swipeHorizontalThresholdDistance = directorSize.width * SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE;
		float swipeVerticalThresholdDistance = directorSize.height * SWIPE_GESTURE_THRESHOLD_SCREEN_PERCENTAGE;

		if (currentTouchPos.x - initialTouchPos.x > swipeHorizontalThresholdDistance)
		{
			log("Swipe Right");
			isTouchDown = false;
            if (snakeDirection != LEFT)
                snakeNewDirection = RIGHT;
		}
		else if (currentTouchPos.x - initialTouchPos.x < -swipeHorizontalThresholdDistance)
		{
			log("Swipe Left");
			isTouchDown = false;
            if (snakeDirection != RIGHT)
                snakeNewDirection = LEFT;
		}
		else if (currentTouchPos.y - initialTouchPos.y > swipeVerticalThresholdDistance)
		{
			log("Swipe Up");
			isTouchDown = false;
            if (snakeDirection != DOWN)
                snakeNewDirection = UP;
		}
		else if (currentTouchPos.y - initialTouchPos.y < -swipeVerticalThresholdDistance)
		{
			log("Swipe Down");
			isTouchDown = false;
            if (snakeDirection != UP)
                snakeNewDirection = DOWN;
		}
	}
}

void LittleSnake::updateSnake(float dt)
{
	if (gameState != PLAYING) return;

    int prevBodyRow = snakeHeadBody->row;
    int prevBodyCol = snakeHeadBody->col;

    snakeDirection = snakeNewDirection;

    if (snakeDirection == RIGHT)
    {
        snakeHeadBody->col++;
    }
    else if (snakeDirection == LEFT)
    {
        snakeHeadBody->col--;
    }
    else if (snakeDirection == UP)
    {
        snakeHeadBody->row++;
    }
    else
    {
        snakeHeadBody->row--;
    }

    if (isSnakeDead())
    {
        updateSnakeFace(snakeDeadFace);
        rotateSnakeHead(snakeDirection);

        snakeHeadBody->row = prevBodyRow;
        snakeHeadBody->col = prevBodyCol;

        renderSnake(dt);

        gameFinishLabel = Label::createWithTTF("Defeat!", "fonts/Schwarzwald Regular.ttf", 400);    // TODO: Duplicate code. Refactor
        gameFinishLabel->setColor(Color3B::RED);  // TODO: ask designer for color
        gameFinishLabel->setPosition(directorOrigin.x + directorSize.width/2, directorOrigin.y + directorSize.height / 3 * 2);
        gameFinishLabel->setScale(0.2f);
        gameFinishLabel->setZOrder(999);
        this->addChild(gameFinishLabel);

        auto scaleAction = ScaleTo::create(GAME_FINISH_DELAY, 1.0f);
        gameFinishLabel->runAction(scaleAction);

        gameState = DEAD;
        auto setGameStateAction = CallFunc::create(CC_CALLBACK_0(LittleSnake::setGameStateToPlayAgain, this));
        auto delayAction = DelayTime::create(GAME_FINISH_DELAY);
        this->runAction(Sequence::createWithTwoActions(delayAction, setGameStateAction));

        return;
    }

	for (auto snakeBody : snakeBodies)
	{
        int tempRow = snakeBody->row;
        int tempCol = snakeBody->col;
		snakeBody->row = prevBodyRow;
        snakeBody->col = prevBodyCol;
        prevBodyRow = tempRow;
        prevBodyCol = tempCol;
	}

    if (isSnakeEatingRaspberry())
    {
        raspberryAteCount++;

        updateSnakeFace(snakeYummyFace);

        SpriteBody *lastSnakeBody = snakeBodies.back();
        addSnakeBodySpriteBody(lastSnakeBody->row, lastSnakeBody->col);

        if (raspberryAteCount >= EASY_MODE_RASPBERRY_COUNT)
        {
            renderSnake(dt);

            gameFinishLabel = Label::createWithTTF("Victory!", "fonts/Schwarzwald Regular.ttf", 400);
            gameFinishLabel->setColor(Color3B::GREEN);  // TODO: ask designer for color
            gameFinishLabel->setPosition(directorOrigin.x + directorSize.width/2, directorOrigin.y + directorSize.height / 3 * 2);
            gameFinishLabel->setScale(0.2f);
            gameFinishLabel->setZOrder(999);
            this->addChild(gameFinishLabel);

            auto scaleAction = ScaleTo::create(GAME_FINISH_DELAY, 1.0f);
            gameFinishLabel->runAction(scaleAction);

            gameState = WIN;
            auto setGameStateAction = CallFunc::create(CC_CALLBACK_0(LittleSnake::setGameStateToPlayAgain, this));
            auto delayAction = DelayTime::create(GAME_FINISH_DELAY);
            this->runAction(Sequence::createWithTwoActions(delayAction, setGameStateAction));

            return;
        }

        spawnRaspberry();
    }
    else
    {
        updateSnakeFace(snakeStandardFace);
    }

    renderSnake(dt);
}

void LittleSnake::setGameStateToPlayAgain()
{
    gameState = PLAYAGAIN;
    updateSnakeFace(snakePlayAgainFace);
    rotateSnakeHead(snakeDirection);
    renderSnake(0);
}

 void LittleSnake::rotateSnakeHead(int angle)
 {
     snakeHeadBody->sprite->setRotation(angle);
 }

void LittleSnake::loadSnakeFaces()
{
    snakeStandardFace = Sprite::create("SnakeHeadStandard.png");
    snakeYummyFace = Sprite::create("SnakeHeadYummy.png");
    snakeDeadFace = Sprite::create("SnakeHeadDead.png");
    snakePlayAgainFace = Sprite::create("SnakeHeadPlayAgain.png");

    snakeStandardFace->setLocalZOrder(2);
    snakeYummyFace->setLocalZOrder(2);
    snakeDeadFace->setLocalZOrder(2);
    snakePlayAgainFace->setLocalZOrder(2);

    snakeStandardFace->setVisible(false);
    snakeYummyFace->setVisible(false);
    snakeDeadFace->setVisible(false);
    snakePlayAgainFace->setVisible(false);
}

void LittleSnake::addSnakeFacesAsChild()
{
    this->addChild(snakeStandardFace);
    this->addChild(snakeYummyFace);
    this->addChild(snakeDeadFace);
    this->addChild(snakePlayAgainFace);
}

void LittleSnake::removeSnakeFacesAsChild()
{
    this->removeChild(snakeStandardFace);
    this->removeChild(snakeYummyFace);
    this->removeChild(snakeDeadFace);
    this->removeChild(snakePlayAgainFace);
}

void LittleSnake::hideSnakeFaces()
{
    snakeStandardFace->setVisible(false);
    snakeYummyFace->setVisible(false);
    snakeDeadFace->setVisible(false);
    snakePlayAgainFace->setVisible(false);
}

void LittleSnake::updateSnakeFace(Sprite *snakeFace)
{
    if (snakeHeadBody->sprite)
        snakeHeadBody->sprite->setVisible(false);

    snakeHeadBody->sprite = snakeFace;
    snakeHeadBody->sprite->setVisible(true);
}

void LittleSnake::spawnRaspberry()
{
    if (!raspberryBody)
    {
        auto raspberry = Sprite::create("Raspberry.png");
	    //check collision with snake body when spawning
	    raspberryBody = new SpriteBody();
	    raspberryBody->sprite = raspberry;
        this->addChild(raspberry);
    }

    do
    {
	    raspberryBody->row = rand() % MAX_MAP_Y;
	    raspberryBody->col = rand() % MAX_MAP_X;
    } while (isSnakeCollidingWithRaspberry());

	raspberryBody->sprite->setPosition(getSpritePosWithBlockPos(raspberryBody->row, raspberryBody->col));
}

bool LittleSnake::isSnakeEatingRaspberry()
{
    if (snakeHeadBody->row == raspberryBody->row && snakeHeadBody->col == raspberryBody->col)
        return true;
    return false;
}

bool LittleSnake::isSnakeCollidingWithRaspberry()
{
    for (auto snakeBody : snakeBodies)
    {
        if (snakeBody->row == raspberryBody->row && snakeBody->col == raspberryBody->col)
            return true;
    }

    return false;
}

// This method is called before snake bodies are moved. Therefore, last snake body theoretically cannot be eaten
bool LittleSnake::isSnakeEatingItsOwnBody()
{
    auto lastSnakeBody = snakeBodies.back();
    for (auto snakeBody : snakeBodies)
    {
        if (snakeBody->row == snakeHeadBody->row && snakeBody->col == snakeHeadBody->col)
        {
            if (snakeBody == lastSnakeBody)
            {
                auto secondLastSnakeBody = snakeBodies.at(snakeBodies.size()-2);
                if (lastSnakeBody->row == secondLastSnakeBody->row && lastSnakeBody->col == secondLastSnakeBody->col)
                {
                    // case where snake just ate raspberry. So there are two snake bodies at the same position
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    return false;
}

bool LittleSnake::isSnakeDead()
{
    if (snakeHeadBody->row < 0 || snakeHeadBody->row >= MAX_MAP_Y || snakeHeadBody->col < 0 || snakeHeadBody->col >= MAX_MAP_X)
        return true;

    if (isSnakeEatingItsOwnBody())
        return true;

    return false;
}

bool LittleSnake::onTouchBegan(Touch* touch, Event* event)
{
    if (gameState == PLAYAGAIN)
    {
        raspberryAteCount = 0;  // refactor to put in some init method
        this->removeChild(gameFinishLabel);

        deleteSnake();
        initializeSnake();
        gameState = REINITIALIZED;
    }
    else if (gameState == REINITIALIZED)
    {
        gameState = PLAYING;
    }

	initialTouchPos = touch->getLocation();
	currentTouchPos = touch->getLocation();
	
	isTouchDown = true;

	return true;
}
void LittleSnake::renderSnake(float dt)
{
    snakeHeadBody->sprite->setPosition(getSpritePosWithBlockPos(snakeHeadBody->row, snakeHeadBody->col));

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

void LittleSnake::addSnakeBodySpriteBody(int row, int col)
{
    auto snakeBody = Sprite::create("SnakeBody.png");
    snakeBody->setLocalZOrder(1);
    this->addChild(snakeBody);

    auto *snakeBodyBody = new SpriteBody();
    snakeBodyBody->sprite = snakeBody;
    snakeBodies.push_back(snakeBodyBody);

    snakeBodyBody->row = row;
    snakeBodyBody->col = col;
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
