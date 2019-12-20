#include "GameScene.h"
#include "GameOver.h"
#include "StartScene.h"
#include <ui/UIImageView.h>
#include "SimpleAudioEngine.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* GameScene::createScene(){
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	if (!Scene::init()){
		return false;
	}
	visibleSize = Director::getInstance()->getVisibleSize();	

	auto gameBj = ui::ImageView::create("gamebg.png");		
	gameBj->setPosition(Vec2(0,0));                         
	gameBj->setAnchorPoint(Vec2(0, 0));						
	float uiX = gameBj->getContentSize().width;				
	float uiY = gameBj->getContentSize().height;			
	gameBj->setScaleX(480 / uiX);												
	gameBj->setScaleY(600 / uiY);							
	this->addChild(gameBj);

	//显示分数
	ScoreSystem();

	//随机方块颜色
	srand(time(NULL));
	createFix();
	this->schedule(schedule_selector(GameScene::boxMove), 0.5f);

	//关闭音乐按钮
	auto musicOn = MenuItemImage::create("menu_on.png", "menu_on.png");
	musicOn->setTag(1);
	auto musicOff = MenuItemImage::create("menu_off.png", "menu_off.png");
	musicOff->setTag(2);

	auto musicTogg = MenuItemToggle::createWithCallback(CC_CALLBACK_1(
		GameScene::menuMusicTogg, this), musicOn, musicOff, NULL);
	musicTogg->setPosition(Vec2(440, 550));

	Menu* mn = Menu::create(musicTogg, NULL);
	mn->setPosition(Vec2::ZERO);
	this->addChild(mn);

	//单击事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->setSwallowTouches(true);  
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

//音乐开关
void GameScene::menuMusicTogg(Ref* pSender){
	auto musicTogg = (MenuItemToggle*) pSender;

	if (musicTogg->getSelectedIndex() == 1){
		SimpleAudioEngine::getInstance()->pauseBackgroundMusic();//暂停播放背景音乐
	}
	else{
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();//继续播放背景音乐
	}
}

//触摸事件
bool GameScene::onTouchBegan(Touch* touch, Event* event){
	Point pos = touch->getLocation();

	if (pos.y > 0 && pos.y < 128){
		if (pos.x > 0 && pos.x < 120){         //左移
			leftMove();
		}
		else if (pos.x > 120 && pos.x < 240){  //右移
			rightMove();
		}
		else if (pos.x > 240 && pos.x < 360){  //旋转
			fkRotate(shape);
		}
		else if (pos.x > 360 && pos.x < 480){  //下移
			fastDrop(0.01f);
		}
	}

	return true;
}

// 键盘事件
void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:  //左移
		leftMove();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW: //右移
		rightMove();
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:       //旋转
		fkRotate(shape);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:  //下移
		fastDrop(0.01f);
		break;
	default:
		break;
	}
}

//预览方块
void GameScene::PreviewBox(int box[4][2]){
	int s, zx, zy;
	s = 20;
	zx = 410;
	zy = 460;

	//设置方块的颜色	
	Colors[0] = Color3B(255, 0, 0);       
	Colors[1] = Color3B(0, 255, 0);       
	Colors[2] = Color3B(0, 0, 255);       
	Colors[3] = Color3B(255, 255, 0);     
	Colors[4] = Color3B(255, 0, 255);     
	Colors[5] = Color3B(0, 255, 255);     

	auto texture = TextureCache::getInstance()->getTextureForKey("box.png");       //图片的纹理
	auto frame = SpriteFrame::createWithTexture(texture, Rect(0, 0, s - 1, s - 1));//创建精灵帧
	for (int i = 0; i < 4; i++){
		auto sp = Sprite::createWithSpriteFrame(frame);                            //通过纹理创建精灵
		sp->setPosition(Point(zx + box[i][0] * s, zy + box[i][1] * s));            //根据数组设置位置
		sp->setColor(Colors[preColor]);                                            //设置方块的颜色
		this->addChild(sp);
		sp->setTag(90 + i);
	}
}

//创建方块图案
void GameScene::createFix()
{
	int ctime[7] = { 0, 1, 1, 1, 3, 3, 3 };

	auto sframe = SpriteFrame::create("box.png", Rect(0, 0, bSize - 1, bSize - 1));
	//下落图案数组
	int boxes[7][4][2]{
		{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },	//"田" 字形方块
		{ { 1, 0 }, { 0, 0 }, { 2, 0 }, { 3, 0 } },	// "1" 字形方块
		{ { 1, 1 }, { 0, 0 }, { 1, 0 }, { 2, 1 } },	//"闪电"形方块
		{ { 1, 1 }, { 0, 1 }, { 1, 0 }, { 2, 0 } },	//"反闪电" 形方块
		{ { 1, 0 }, { 0, 0 }, { 2, 0 }, { 1, 1 } },	//"凸"	字形方块
		{ { 1, 0 }, { 0, 0 }, { 0, 1 }, { 2, 0 } },	//"反L" 形方块
		{ { 1, 0 }, { 0, 0 }, { 2, 0 }, { 2, 1 } }	//"L"  形方块
	};	
	//设置方块的颜色
	Colors[0] = Color3B(255, 0, 0);
	Colors[1] = Color3B(0, 255, 0);
	Colors[2] = Color3B(0, 0, 255);
	Colors[3] = Color3B(255, 255, 0);
	Colors[4] = Color3B(255, 0, 255);
	Colors[5] = Color3B(0, 255, 255);

	//取出颜色
	if (preColor == -1){
		preColor = rand() % 6;
	}
	
	//取出形状
	if (preShape == -1){
		shape = preShape;
		preShape = rand() % 7;
	}

	//获取形状
	for (int i = 0; i < 4; i++){
		Sprite*box = Sprite::createWithSpriteFrame(sframe);
		box->setColor(Colors[preColor]);//设置颜色
		box->setAnchorPoint(Vec2(0, 0));//设置锚点（方块下落终点）
		box->setPosition(Vec2(getX(4 + boxes[preShape][i][0]), getY(13 + boxes[preShape][i][1])));//设置位置
		this->addChild(box);
		box->setTag(110 + i);
		MoveList.pushBack(box); //保存到正在下落的方块数组内
	}

	Eliminate(); //消除方块
	shape = preShape;
	preShape = rand() % 7;
	preColor = rand() % 6;
	PreviewBox(boxes[preShape]);
}

//数字转换坐标        
float GameScene::getX(int x){
	return 85 + x * 31;
}
float GameScene::getY(int y){
	return 128 + y * 31;
}

//坐标转换数字 
int GameScene::getIx(float x){
	return (x - 85) / 31;
}
int GameScene::getIy(float y){
	return (y - 128) / 31;
}

//获取最高高度
int GameScene::Highest(){
	int h = 0;
	for (int i = 0; i<StopList.size(); i++)
	{
		Sprite* box = StopList.at(i);
		int y = getIy(box->getPositionY());
		if (y > h)
			h = y;
	}
	return h;

}

//快速下落
void GameScene::fastDrop(float f){
	moveFks(0);
}

//方块移动
void GameScene::boxMove(float delta){
	moveFks(0);
}

//方块下落
void GameScene::downMove(float delta)
{
	//获取正在下落的四个方块,改变它们y轴位置
	for (auto block : MoveList)
	{
		block->setPositionY(getY(getIy(block->getPositionY()) - 1));
	}
}
//判断能否下落
void GameScene::moveFks(int num){
	//先获取4个方块的信息
	Sprite *temp[4] = { NULL };
	for (int i = 0; i < 4; i++)
	{
		temp[i] = (Sprite *)this->getChildByTag(110 + i);
	}
	//如果下落的方块碰到了底部或别的方块
	if (isCollSion(110) || isCollSion(111) || isCollSion(112) || isCollSion(113))
	{
		this->unschedule(schedule_selector(GameScene::downMove));
		for (int i = 0; i < 4; i++)
		{
			temp[i]->setTag(120);
			MoveList.eraseObject(temp[i]);
			this->StopList.pushBack(temp[i]);	//保存到停止下落队列中
		}
		if (Highest() < 13)
		{
			for (int i = 0; i < 4; i++)
			{
				Sprite* remove = (Sprite*)this->getChildByTag(90 + i);
				remove->removeFromParent();
			}
			createFix();
		}
		else
		{
			//存储当前分数变量
			UserDefault::getInstance()->setIntegerForKey("score", score);
			//记录最高分
			int top_gf = UserDefault::getInstance()->getIntegerForKey("tallscore");
			if (score > top_gf){
				UserDefault::getInstance()->setIntegerForKey("tallscore", score);
			}

			Director::getInstance()->replaceScene(GameOver::createScene());

			SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
		}
	}
	else
	{
		downMove(0);
	}
}

//判断是否碰撞
bool GameScene::isCollSion(int tag){
	Sprite* block = (Sprite*)this->getChildByTag(tag);
	Point pos_a, pos_b;
	Sprite* _block;
	bool flag = false;
	if (block == NULL)
	{
		return false;
	}
	pos_a.x = getIx(block->getPositionX());		
	pos_a.y = getIy(block->getPositionY());		
	for (int i = 0; i < StopList.size(); i++)	//碰撞检测
	{
		_block = StopList.at(i);
		pos_b.x = getIx(_block->getPositionX());
		pos_b.y = getIy(_block->getPositionY());
		if (pos_a.y - pos_b.y == 1 && pos_a.x == pos_b.x)
		{
			flag = true;
		}
	}
	if (pos_a.y <= 0)
		flag = true;
	return flag;
}

//判断方块能否左移
bool GameScene::isBoxStopLeft()
{
	int M_x[4], M_y[4];
	int S_x, S_y;
	Point pos_stop;
	Sprite* stop_block;
	bool flag = true;

	//获取正在移动的方块，将其像素坐标转换为格子坐标
	for (int i = 0; i < 4; i++)
	{
		Sprite *box = (Sprite *)this->getChildByTag(110 + i);
		if (box == NULL)
		{
			return false;
		}
		M_x[i] = getIx(box->getPositionX());
		M_y[i] = getIy(box->getPositionY());
	}

	//判断此次左移会不会出界
	for (int i = 0; i < 4; i++)
	{
		if (M_x[i] - 1 < 0)				
		{
			return false;
		}
	}

	//获取已经停止的方块，将其像素坐标转换为格子坐标,随后进行碰撞判断
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < StopList.size(); j++)
		{
			stop_block = StopList.at(j);
			S_x = getIx(stop_block->getPositionX());
			S_y = getIy(stop_block->getPositionY());
			if (M_y[i] == S_y)
			{
				if (M_x[i] - 1 == S_x)
				{
					flag = false;
				}
			}
		}
	}
	return flag;
}

//方块左移
void GameScene::leftMove(){
	if (!isBoxStopLeft()){
		return;
	}
	Sprite*block = NULL;
	for (int i = 0; i < 4; i++){
		block = MoveList.at(i); 	
		block->setPositionX((block->getPositionX() - 31));
	}
}

//判断方块能否右移
bool GameScene::isBoxStopRight(){
	int M_x[4], M_y[4];
	int S_x, S_y;
	Point pos_stop;
	Sprite* stop_block;
	bool flag = true;

	//获取正在移动的方块，将其像素坐标转换为格子坐标
	for (int i = 0; i < 4; i++)
	{
		Sprite *box = (Sprite *)this->getChildByTag(110 + i);
		if (box == NULL)
		{
			return false;
		}
		M_x[i] = getIx(box->getPositionX());
		M_y[i] = getIy(box->getPositionY());
	}

	//判断此次右移会不会出界
	for (int i = 0; i < 4; i++)
	{
		if (M_x[i] + 1 >= 10)			//确保右移后不会出界
		{
			return false;
		}
	}

	//获取已经停止的方块，将其像素坐标转换为格子坐标,随后进行碰撞判断
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < StopList.size(); j++)
		{
			stop_block = StopList.at(j);
			S_x = getIx(stop_block->getPositionX());
			S_y = getIy(stop_block->getPositionY());
			if (M_y[i] == S_y)
			{
				if (M_x[i] + 1 == S_x)
				{
					flag = false;
				}
			}
		}
	}
	return flag;
}

//方块右移 
void GameScene::rightMove()
{
	if (!isBoxStopRight()){
		return;
	}
	Sprite*block = NULL;
	for (int i = 0; i < 4; i++){
		block = MoveList.at(i);
		block->setPositionX((block->getPositionX() + 31));
	}
}

//判断方块能否旋转 
bool GameScene::isRotate(int x, int y)
{
	Sprite*block;
	bool flag = true;

	for (int i = 0; i < StopList.size(); i++)
	{
		block = StopList.at(i);
		int bx = getIx(block->getPositionX());
		int by = getIy(block->getPositionY());
		if (x == bx&&y == by){
			flag = false;//不能旋转
		}
	}
	if (x < 0 || x>9 || y < 0)
		flag = false;
	return flag;
}

//旋转变形  
void GameScene::fkRotate(int num){
	static int boxs[] = { 0, 1, 1, 1, 3, 3, 3 };
	int flag = 0;
	Sprite*spBox[4] = { NULL };
	int bx[4], by[4];
	int fx[4], fy[4];
	for (int i = 0; i < 4; i++){
		spBox[i] = (Sprite*)this->getChildByTag(110 + i);
		if (spBox[i] == NULL){
			return;
		}
		bx[i] = getIx(spBox[i]->getPositionX());
		by[i] = getIy(spBox[i]->getPositionY());
	}
	//判断变形次数
	if (boxs[num] == 0){
		//不变形
	}
	else if (boxs[num] == 1){
		if (flag == 0)
		{
			//顺时针旋转
			for (int i = 0; i < 4; i++)
			{
				fx[i] = bx[0] + (by[0] - by[i]);
				fy[i] = by[0] - (bx[0] - bx[i]);
			}
			flag = 1;
		}
		else if (flag == 1)
		{
			//逆时针旋转
			for (int i = 0; i < 4; i++)
			{
				fx[i] = bx[0] - (by[0] - by[i]);
				fy[i] = by[0] + (bx[0] - bx[i]);
			}
			flag = 0;
		}
		//将方块旋转
		if (isRotate(fx[0], fy[0]) && isRotate(fx[1], fy[1]) && isRotate(fx[2], fy[2]) && isRotate(fx[3], fy[3]))
		{
			for (int i = 0; i < 4; i++){
				spBox[i]->setPositionX(getX(fx[i]));
				spBox[i]->setPositionY(getY(fy[i]));
			}
		}
		else
		{
			flag = (flag == 1) ? 0 : 1;
		}
	}
	else if (boxs[num] == 3){
		for (int i = 0; i < 4; i++){
			fx[i] = bx[0] - (by[0] - by[i]);
			fy[i] = by[0] + (bx[0] - bx[i]);
		}
	}
	if (isRotate(fx[0], fy[0]) && isRotate(fx[1], fy[1]) && isRotate(fx[2], fy[2]) && isRotate(fx[3], fy[3]))
	{
		for (int i = 0; i < 4; i++){
			spBox[i]->setPositionX(getX(fx[i]));
			spBox[i]->setPositionY(getY(fy[i]));
		}
	}
}

//消除方块  
void GameScene::Eliminate()
{
	int sum = 0;                                        //累加一行的方块数目
	int line;
	Sprite *Boxes[10] = { NULL };
	for (line = 0; line < 13; line++)
	{
		for (int j = 0; j < StopList.size(); j++)
		{
			Sprite* Box = StopList.at(j);
			if (getIy(Box->getPositionY()) == line)		//筛选第line行方块
			{
				Boxes[sum++] = Box;					    //保存此行方块
			}
		}
		if (sum == 10)
		{
			for (int i = 0; i < 10; i++)
			{
				StopList.eraseObject(Boxes[i]);
				Boxes[i]->removeFromParent();		    //消除并加分				

			}

			score = score + 1;			
			Fs->setString(String::createWithFormat("%d", score)->getCString());				
			
			for (int i = 0; i < StopList.size(); i++)
			{
				Sprite* Drop = StopList.at(i);
				int ThisLine = getIy(Drop->getPositionY());
				if (ThisLine > line)							//消除后，把位于第line行之上的方块移下来
				{
					Drop->setPositionY(getY(ThisLine - 1));
				}
			}
			Eliminate();
		}
		sum = 0;
	}
}

//记录分数 
void GameScene::ScoreSystem()
{	
	//读取XML文件
	dic = Dictionary::createWithContentsOfFile("fonts/Chanies.xml");

	//最高分数显示
	auto tall_score = Label::create();
	tall_score->setString(((__String*)(dic->objectForKey("tallscore")))->getCString());
	tall_score->setSystemFontSize(20);
	tall_score->setColor(Color3B::GREEN);
	tall_score->setAnchorPoint(Point(0.5, 0.5));
	tall_score->setPosition(Point(visibleSize.width - 440, visibleSize.height - 60));
	this->addChild(tall_score);

	Gf = Label::create();
	Gf->setSystemFontSize(20);
	Gf->setColor(Color3B::BLUE);
	Gf->setPosition(Point(visibleSize.width - 440, visibleSize.height - 90));
	this->addChild(Gf);

	//当前分数显示
	auto dq_score = Label::create();
	dq_score->setString(((__String*)(dic->objectForKey("score")))->getCString());
	dq_score->setSystemFontSize(20);
	dq_score->setColor(Color3B::GREEN);
	dq_score->setPosition(Point(visibleSize.width - 440, visibleSize.height - 120));
	this->addChild(dq_score);

	Fs = Label::create();
	Fs->setSystemFontSize(20);
	Fs->setColor(Color3B::BLUE);
	Fs->setPosition(Point(visibleSize.width - 440, visibleSize.height - 150));
	this->addChild(Fs);

	//UserDefault存储最高分
	int top_score = UserDefault::getInstance()->getIntegerForKey("tallscore");
	Gf->setString(String::createWithFormat("%d", top_score)->getCString());

	//当前分
	Fs->setString(String::createWithFormat("%d", score)->getCString());	

	//添加暂停按钮
	auto btn_label = Label::create();
	btn_label->setString(((__String*)(dic->objectForKey("suspend")))->getCString());
	btn_label->setSystemFontSize(30);

	auto zt_menu = MenuItemLabel::create(btn_label,
		CC_CALLBACK_1(GameScene::Suspend, this));

	Menu* btn_menu = Menu::create(zt_menu, NULL);
	btn_menu->setPosition(Vec2(visibleSize.width - btn_label->getContentSize().width / 2 - 13, visibleSize.height - 250));
	this->addChild(btn_menu);
}

//暂停按钮函数
void GameScene::Suspend(Ref* pSender)
{
	//读取XML文件
	dic = Dictionary::createWithContentsOfFile("fonts/Chanies.xml");
		
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	this->pause();
	for (const auto&node : this->getChildren())
	{
		node->pause();
	}

	//继续游戏按钮
	auto go_on = Label::create();
	go_on->setString(((__String*)(dic->objectForKey("goOn")))->getCString());
	go_on->setSystemFontSize(30);
	auto go_menu = MenuItemLabel::create(go_on, CC_CALLBACK_1(GameScene::Continue, this));
	
	menu_on = Menu::create(go_menu, NULL);
	menu_on->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 2 / 3));
	this->addChild(menu_on);	

	//重新开始按钮
	auto restart = Label::create();
	restart->setString(((__String*)(dic->objectForKey("restart")))->getCString());
	restart->setSystemFontSize(30);
	auto restart_menu = MenuItemLabel::create(restart, CC_CALLBACK_1(GameScene::Restart, this));

	restart_on = Menu::create(restart_menu, NULL);
	restart_on->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 2 / 4));
	this->addChild(restart_on);
}

//继续游戏按钮函数
void GameScene::Continue(Ref* pSender)
{	
	//继续播放背景音乐
	SimpleAudioEngine::getInstance()->resumeBackgroundMusic();

	//继续当前层的Node元素
	this->resume();

	//继续当前层的子Node元素
	for (const auto&node : this->getChildren())
	{
		node->resume();
	}
	//将菜单按钮移除
	removeChild(menu_on);
	removeChild(restart_on);
}

void GameScene::Restart(Ref* pSender){
	Director::getInstance()->replaceScene(StartScene::createScene());
}