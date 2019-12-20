#ifndef _GameOver_H_
#define _GameOver_H_

#include "cocos2d.h"
USING_NS_CC;

class GameOver :public Layer{
public:
	static Scene* createScene();
	virtual bool init();
	
	void startMenu(Ref* ref);   //重新开始按钮
	void exitMenu(Ref* ref);    //退出游戏按钮
	void FsRecord();            //分数记录

	CREATE_FUNC(GameOver);
};

#endif