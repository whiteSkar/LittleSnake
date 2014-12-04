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

    auto background = Sprite::create("background.png"); // use Different Background. Transition seems weird
    //background->setScale(0.5);  // TODO: remove later
    background->setPosition(directorOrigin.x + background->getBoundingBox().size.width/2, directorOrigin.y + background->getBoundingBox().size.height/2);
    this->addChild(background);

    auto menuItem1 = MenuItemFont::create("Easy Mode", CC_CALLBACK_1(MenuScene::startEasyMode, this));
    auto menuItem2 = MenuItemFont::create("Hardcore Mode", CC_CALLBACK_1(MenuScene::startHardcoreMode, this));
    menuItem1->setPosition(Director::getInstance()->getVisibleSize().width / 3 * 1, Director::getInstance()->getVisibleSize().height / 2);
    menuItem2->setPosition(Director::getInstance()->getVisibleSize().width / 3 * 2, Director::getInstance()->getVisibleSize().height / 2);

    auto menu = Menu::create(menuItem1, menuItem2, NULL);
    menu->setPosition(0, 0);
    this->addChild(menu);
    
    return true;
}

void MenuScene::startEasyMode(Ref *sender)
{
    auto gameScene = LittleSnake::createScene();

    Director::getInstance()->pushScene(TransitionMoveInR::create(1.0f, gameScene));
}

void MenuScene::startHardcoreMode(Ref *sender)
{
    auto gameScene = LittleSnake::createScene();

    Director::getInstance()->pushScene(TransitionMoveInR::create(1.0f, gameScene));
}