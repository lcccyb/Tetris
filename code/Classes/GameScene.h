#ifndef _GameScene_H_
#define _GameScene_H_

#include "cocos2d.h"
USING_NS_CC;

class GameScene :public Scene{
public:
	static Scene*createScene();
	virtual bool init();
	//创建预览方块
	void PreviewBox(int box[4][2]);
	//创建下落图案
	void createFix();

	//获取像素坐标
	float getX(int x);
	float getY(int y);
	//获取坐标
	int getIx(float x);
	int getIy(float y);

	cocos2d::Vector<cocos2d::Sprite*> StopList;//存储已经停止移动的方块
	cocos2d::Vector<cocos2d::Sprite*> MoveList;//存储正在移动的方块

	//获取最高高度
	int Highest();

	//单击事件
	bool onTouchBegan(Touch* touch, Event* event);
	//键盘事件
	void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event);

	void moveFks(int num);//移动方块
	bool isCollSion(int tag);//判断是否碰撞	
	bool isBoxStopLeft();//判断能否左移
	bool isBoxStopRight();//判断能否右移
	bool isRotate(int x, int y);//判断方块能否旋转

	void leftMove();//左移
	void rightMove();//右移
	void downMove(float delta);//下移
	void fastDrop(float f);//快速下落
	void boxMove(float delta);//移动
	void fkRotate(int num);//旋转变形
	void Eliminate();//消除方块
	void menuMusicTogg(Ref* pSender);//音乐开关
	void Suspend(Ref* pSender);//暂停按钮
	void Continue(Ref* pSender);//继续游戏按钮
	void Restart(Ref* pSender);
	Menu* menu_on;//继续游戏按钮
	Menu* restart_on;//重新开始按钮s
	
	void ScoreSystem();//显示分数和等级
	Dictionary* dic;//读取XML文件
	Size visibleSize;
	int score = 0;//初始化分数
	Label* Fs;//当前分数
	Label* Gf;//最高分数

	CREATE_FUNC(GameScene);
private:
	Color3B Colors[6];//设置方块的颜色
	int bSize = 31;//主场景方块大小
	int preColor = -1;//预设图案颜色
	int preShape = -1;//预设图案
	int shape = -1;;//图案

};

#endif