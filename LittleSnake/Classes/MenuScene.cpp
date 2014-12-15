#include "MenuScene.h"
#include "LittleSnakeScene.h"


USING_NS_CC;

Scene* MenuScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MenuScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool MenuScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    directorSize = Director::getInstance()->getVisibleSize();
    directorOrigin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("MenuBackground.png");
    background->setPosition(directorOrigin.x + directorSize.width/2, directorOrigin.y + directorSize.height/2);
    background->setZOrder(-999);
    this->addChild(background);

    auto backgroundBoundary = Sprite::create("BackgroundBoundary.png");
    backgroundBoundary->setPosition(directorOrigin.x + directorSize.width/2, directorOrigin.y + directorSize.height/2);
    backgroundBoundary->setZOrder(-999);
    this->addChild(backgroundBoundary);

    auto menuItem1 = MenuItemImage::create("MenuButton.png", "MenuButton.png", "MenuButton.png", CC_CALLBACK_1(MenuScene::startEasyMode, this));
    auto menuItem2 = MenuItemImage::create("MenuButton.png", "MenuButton.png", "MenuButton.png", CC_CALLBACK_1(MenuScene::startHardcoreMode, this));
    menuItem1->setPosition(Director::getInstance()->getVisibleSize().width / 100 * 30, Director::getInstance()->getVisibleSize().height / 3);
    menuItem2->setPosition(Director::getInstance()->getVisibleSize().width / 100 * 70, Director::getInstance()->getVisibleSize().height / 3);

    auto menu = Menu::create(menuItem1, menuItem2, NULL);
    menu->setPosition(0, 0);
    this->addChild(menu);

    auto menuItemTitleColor = Color3B::Color3B(133, 96, 168);
    auto menuItemDescriptionColor = Color3B::Color3B(168, 100, 168);

    auto menuTitleFontSize = 80.0f;
    auto menuDescriptionFontSize = 48.0f;
    std::string menuFontPath = "fonts/Showcard Gothic.ttf";
    auto menuTitleVerticalOffset = 45.0f;

    auto menuItem1Title = Label::createWithTTF("Easy Mode", menuFontPath, menuTitleFontSize);
    menuItem1Title->setColor(menuItemTitleColor);
    menuItem1Title->setPosition(menuItem1->getPositionX(), 
        menuItem1->getPositionY() + menuItem1->getBoundingBox().size.height / 2 - menuItem1Title->getBoundingBox().size.height/2 - menuTitleVerticalOffset);
    menuItem1Title->setLocalZOrder(999);
    this->addChild(menuItem1Title);

    auto menuItem1Description = Label::createWithTTF("Eat three\nraspberries!", menuFontPath, menuDescriptionFontSize, Size::ZERO, TextHAlignment::CENTER);
    menuItem1Description->setColor(menuItemDescriptionColor);
    menuItem1Description->setPosition(menuItem1->getPositionX(), 
        menuItem1->getPositionY() - menuItem1Description->getBoundingBox().size.height/2);
    menuItem1Description->setLocalZOrder(999);
    this->addChild(menuItem1Description);

    auto menuItem2Title = Label::createWithTTF("Hard Mode", menuFontPath, menuTitleFontSize);
    menuItem2Title->setColor(menuItemTitleColor);
    menuItem2Title->setPosition(menuItem2->getPositionX(), 
        menuItem2->getPositionY() + menuItem2->getBoundingBox().size.height / 2 - menuItem2Title->getBoundingBox().size.height/2 - menuTitleVerticalOffset);
    menuItem2Title->setLocalZOrder(999);
    this->addChild(menuItem2Title);

    auto menuItem2Description = Label::createWithTTF("Fill the screen\nwith your snake!", menuFontPath, menuDescriptionFontSize, Size::ZERO, TextHAlignment::CENTER);
    menuItem2Description->setColor(menuItemDescriptionColor);
    menuItem2Description->setPosition(menuItem2->getPositionX(), 
        menuItem2->getPositionY() - menuItem2Description->getBoundingBox().size.height/2);
    menuItem2Description->setLocalZOrder(999);
    this->addChild(menuItem2Description);

    return true;
}

void MenuScene::startEasyMode(Ref *sender)
{
    auto gameScene = LittleSnake::createScene(true);

    Director::getInstance()->pushScene(TransitionMoveInR::create(0.5f, gameScene));
}

void MenuScene::startHardcoreMode(Ref *sender)
{
    auto gameScene = LittleSnake::createScene(false);

    Director::getInstance()->pushScene(TransitionMoveInR::create(0.5f, gameScene));
}