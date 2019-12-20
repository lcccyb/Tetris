#ifndef _StartScene_H_
#define _StartScene_H_

#include "cocos2d.h"
USING_NS_CC;

class StartScene : public Layer{
public:
	static Scene*createScene();
	virtual bool init();
	void startMenu(Ref* ref);
	CREATE_FUNC(StartScene);
};

#endif