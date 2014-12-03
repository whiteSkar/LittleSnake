#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"

class MenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();  
    
    CREATE_FUNC(MenuScene);

private:
    void startEasyMode(cocos2d::Ref *sender);
    void startHardcoreMode(cocos2d::Ref *sender);
};

#endif // __MENU_SCENE_H__
