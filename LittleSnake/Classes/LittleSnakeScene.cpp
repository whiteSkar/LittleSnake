#include "LittleSnakeScene.h"

USING_NS_CC;

Scene* LittleSnake::createScene(bool isEasyMode)
{
    auto scene = Scene::create();
    
    auto layer = LittleSnake::create();
    if (isEasyMode)
        layer->setupForEasyMode();
    else
        layer->setupForHardcoreMode();

    scene->addChild(layer);

    return scene;
}

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
    background->setPosition(directorOrigin.x + background->getBoundingBox().size.width/2, directorOrigin.y + background->getBoundingBox().size.height/2);
    this->addChild(background);

    loadSnakeFaces();
    addSnakeFacesAsChild();
    
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);

	touchListener->onTouchBegan = CC_CALLBACK_2(LittleSnake::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(LittleSnake::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(LittleSnake::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(LittleSnake::onTouchCancelled, this);
	
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

    auto menuItem1 = MenuItemFont::create("Go Back", CC_CALLBACK_1(LittleSnake::exitScene, this));
    auto menuItem2 = MenuItemFont::create("Play Again", CC_CALLBACK_1(LittleSnake::playAgain, this));
    menuItem1->setPosition(Director::getInstance()->getVisibleSize().width / 3 * 1, Director::getInstance()->getVisibleSize().height / 3 * 1);
    menuItem2->setPosition(Director::getInstance()->getVisibleSize().width / 3 * 2, Director::getInstance()->getVisibleSize().height / 3 * 1);

    menu = Menu::create(menuItem1, menuItem2, NULL);
    menu->setPosition(0, 0);
    menu->setVisible(false);
    this->addChild(menu);
    
    return true;
}

void LittleSnake::setupForEasyMode()
{
	snakeMoveInterval = SNAKE_MOVE_INTERVAL_EASY_MODE;
	raspberryToEatCount	= RASPBERRY_TO_EAT_COUNT_EASY_MODE;
	initialSnakeBodyCount = INITIAL_SNAKE_BODY_COUNT_EASY_MODE;

    setupCommon();
}

void LittleSnake::setupForHardcoreMode()
{
    snakeMoveInterval = SNAKE_MOVE_INTERVAL_HARDCORE_MODE;
	raspberryToEatCount	= RASPBERRY_TO_EAT_COUNT_HARDCORE_MODE;
	initialSnakeBodyCount = INITIAL_SNAKE_BODY_COUNT_HARDCORE_MODE;

    setupCommon();
}

void LittleSnake::setupCommon()
{
    log("Game State: INITIALIZED");

    raspberryBody = nullptr;
    raspberryAteCount = 0;
	dtCount = 0.0;

    isTouchDown = false;
	initialTouchPos = Point::ZERO;
	this->currentTouchPos = Point::ZERO;

    initializeSnake();
    spawnRaspberry();

    gameState = INITIALIZED;
    this->scheduleUpdate();
}

void LittleSnake::showGameFinishLabel(std::string text, Color3B color)
{
    gameFinishLabel = Label::createWithTTF(text, "fonts/Schwarzwald Regular.ttf", 400);
    gameFinishLabel->setColor(color);  // TODO: ask designer for color
    gameFinishLabel->setPosition(directorOrigin.x + directorSize.width/2, directorOrigin.y + directorSize.height / 3 * 2);
    gameFinishLabel->setScale(0.2f);
    gameFinishLabel->setZOrder(999);
    this->addChild(gameFinishLabel);

    auto scaleAction = ScaleTo::create(GAME_FINISH_DELAY, 1.0f);
    gameFinishLabel->runAction(scaleAction);
}

void LittleSnake::initializeSnake()
{
    snakeDirection = RIGHT;
    snakeNewDirection = RIGHT;
    snakeBodies = std::vector<SpriteBody*>();

	snakeHeadBody = new SpriteBody();
    snakeHeadBody->row = MAX_MAP_Y / 2;
	snakeHeadBody->col = MAX_MAP_X / 2;

    updateSnakeFace(snakeStandardFace);
    rotateSnakeHead(snakeDirection);

    for (int i = 0; i < initialSnakeBodyCount; ++i)
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
        delete snakeBodies[i];
    }

    delete snakeHeadBody;

    hideSnakeFaces();
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

	updateSnake(dt);
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
	dtCount += dt;
	if (dtCount >= snakeMoveInterval)
	{
		dtCount -= snakeMoveInterval;
	}
	else
	{
		return;
	}

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
        log("Game State: DEAD");
        updateSnakeFace(snakeDeadFace);
        rotateSnakeHead(snakeDirection);

        snakeHeadBody->row = prevBodyRow;
        snakeHeadBody->col = prevBodyCol;

        renderSnake(dt);

        showGameFinishLabel("Defeat!", Color3B::RED);

        gameState = DEAD;
        setGameStateToPlayAgainWithDelay();

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
        log("Snake is eating raspberry");
        raspberryAteCount++;

        updateSnakeFace(snakeYummyFace);
        rotateSnakeHead(snakeDirection);

        SpriteBody *lastSnakeBody = snakeBodies.back();
        addSnakeBodySpriteBody(lastSnakeBody->row, lastSnakeBody->col);

        if (raspberryAteCount >= raspberryToEatCount)
        {
            log("Game State: WIN");

            renderSnake(dt);

            showGameFinishLabel("Victory!", Color3B::GREEN);

            gameState = WIN;
            setGameStateToPlayAgainWithDelay();

            return;
        }

        spawnRaspberry();
    }
    else
    {
        updateSnakeFace(snakeStandardFace);
        rotateSnakeHead(snakeDirection);
    }

    renderSnake(dt);
}

void LittleSnake::setGameStateToPlayAgain()
{
    log("Game State: PLAYAGAIN");
    gameState = PLAYAGAIN;
    updateSnakeFace(snakePlayAgainFace);
    rotateSnakeHead(snakeDirection);
    renderSnake(0);

    menu->setVisible(true);
}

void LittleSnake::setGameStateToPlayAgainWithDelay()
{
    auto setGameStateAction = CallFunc::create(CC_CALLBACK_0(LittleSnake::setGameStateToPlayAgain, this));
    auto delayAction = DelayTime::create(GAME_FINISH_DELAY);
    this->runAction(Sequence::createWithTwoActions(delayAction, setGameStateAction));
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

    hideSnakeFaces();
}

void LittleSnake::addSnakeFacesAsChild()
{
    this->addChild(snakeStandardFace);
    this->addChild(snakeYummyFace);
    this->addChild(snakeDeadFace);
    this->addChild(snakePlayAgainFace);
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
    log("Spawning Raspberry");

    if (!raspberryBody)
    {
        auto raspberry = Sprite::create("Raspberry.png");
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
    if (snakeHeadBody->row == raspberryBody->row && snakeHeadBody->col == raspberryBody->col)
        return true;

    for (auto snakeBody : snakeBodies)
    {
        if (snakeBody->row == raspberryBody->row && snakeBody->col == raspberryBody->col)
            return true;
    }

    return false;
}

// This method is called before snake bodies are moved. Therefore, need to let last snake body to return false unless there are two snake bodies there
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
    if (gameState == INITIALIZED || gameState == REINITIALIZED)
    {
        log("Game State: PLAYING");
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

void LittleSnake::playAgain(Ref *sender)
{
    log("Game State: REINITIALIZED");

    menu->setVisible(false);
    
    raspberryAteCount = 0;
    this->removeChild(gameFinishLabel);

    deleteSnake();
    initializeSnake();
    spawnRaspberry();
    gameState = REINITIALIZED;
}

void LittleSnake::exitScene(Ref *sender)
{
    deleteSnake();
    this->removeChild(raspberryBody->sprite);
    delete raspberryBody;

    auto previousScene = Director::getInstance()->getPreviousScene();
    Director::getInstance()->popScene(TransitionMoveInL::create(1.0f, previousScene));
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
