# 实验报告：俄罗斯方块（游戏）

### 项目介绍

本项目为传统的俄罗斯方块游戏，由小方块组成的不同形状的板块陆续从屏幕上方落下来，玩家通过调整板块的位置和方向，使它们在屏幕底部拼出完整的一条或几条。这些完整的横条会随即消失，给新落下来的板块腾出空间，与此同时，玩家得到分数奖励。没有被消除掉的方块不断堆积起来，一但堆到屏幕顶端，游戏结束。

游戏可以用键盘操控，"←"左移一格；"→"右移一格；空格键旋转方块；"↓" 方块降落速度加快。当然，也可以点击游戏界面的按键进行操控。

### 运行环境

本项目已经打包成exe文件，所有执行组件都在压缩包里，解压并运行Tetris.exe即可运行游戏。

如果需要运行代码，则需要这些环境：**Microsoft Visual Studio，cocos2d-x，python2.7**

cocos2d-x在Windows环境下配置：

* 配置python2环境

* 在这里获取cocos2d-x：[cocos2d-x源码包](<https://cocos2d-x.org/download>) 

* 安装cocos2d-x：命令行进入cocos2d-x的目录下，运行python setup.py

* 命令行进入workspace(存放项目的路径，项目创建完成后在这个目录下)：运行命令 cocos new ProjectName -p com.games.GameName -l cpp，其中

  * ProjectName：项目名称
  * com.games.GameName：包名

   然后就可以看到目标路径下的工程文件了。

* 项目创建完成后，复制项目源码中的Classes和Resources文件夹，将新创建的项目中的Classes和Resources文件夹覆盖。
* 运行Proj.win32目录中的后缀为sln的文件，在解决方案资源管理器中，将Classes和Resources目录下的文件分别添加到src和resource中，单击调试器运行。第一次编译需要的时间较长（大概需要几分钟），请耐心等待。

### 项目展示

因为录了视频，所以不会过多说明，详情请查看视频或者玩游戏。

开始界面：

![1576849915106](assets\1.png)

游戏运行界面：

![1576849966187](assets\2.png)

游戏结束界面：

![1576850011873](assets\3.png)

### 项目制作

基本制作框架如下：

![1576860073225](assets\4.png)

首先制作开始界面主场景（开始界面）。这个主要时间花在界面布局上。这个游戏主菜单的界面很简单， 就一张背景，外加个游戏标题和开始游戏的按钮。因此这里主要就是添加背景，游戏标题以及按钮的制作，功能在StartScene.h和StartScene.cpp里面实现。

背景的添加以及大小调整：

```c++
auto homeBj = ui::ImageView::create("bg.jpg");			
homeBj->setPosition(Vec2(0,0));	
homeBj->setAnchorPoint(Vec2(0, 0));							
float uiX = homeBj->getContentSize().width;					
float uiY = homeBj->getContentSize().height;				
homeBj->setScaleX(480 / uiX);													
homeBj->setScaleY(600 / uiY);								
this->addChild(homeBj);
```
游戏标题：

```c++
auto gameBt = Sprite::create("title.png");
gameBt->setPosition(Point(visibleSize.width / 2, visibleSize.height - 50));
this->addChild(gameBt);
```
开始游戏按钮：

```c++
auto start = Label::create();
start->setString(str->getCString());
start->setSystemFontSize(40);
start->setColor(Color3B::BLUE);

auto startmenu = MenuItemLabel::create(start, CC_CALLBACK_1(StartScene::startMenu, this));
startmenu->setPosition(Point(visibleSize.width / 2, visibleSize.height / 6));

auto menu = Menu::create(startmenu, NULL);
menu->setPosition(Vec2::ZERO);
this->addChild(menu);
```
另外还添加了bgm：

```c++
auto music = SimpleAudioEngine::getInstance();
music->preloadBackgroundMusic("sound/bgyy.mp3");
music->playBackgroundMusic("sound/bgyy.mp3", true);
```
然后再说下Game Over界面，其实与开始界面差不多，主要都是布局的问题。与开始界面不同的是，Game Over界面需要获取本地的最高分数据，这里用到了简单数据存储的Userdefault类：

```c++
//获取最高分
auto score_win = UserDefault::getInstance()->getIntegerForKey("tallscore");
auto score_dq = UserDefault::getInstance()->getIntegerForKey("score");

//显示最高分
Gfjl->setString(String::createWithFormat("%d", score_win)->getCString());
//显示当前分
Fsjl->setString(String::createWithFormat("%d", score_dq)->getCString());
```
然后是最核心的部分，游戏部分的设计，即系统功能的设计。包括方块下落、旋转功能：整个游戏中，方块是核心，系统每次随机产生一个方块，一共7种不同的方块，方块可以根据玩家的操作进行左移、右移、加速下落、顺时针旋转等等。

游戏界面布局设计：中间是方块下落区域，下面摆放操作按键。游戏区域的侧边栏，左侧会显示出分数；右侧会显示出下一个方块的形状，并提供暂停和关闭bgm的按钮。

整个GameScene类的定义如下：
```c++
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
```
#### 方块的定义

预制7钟形状的方块：

```c++
int boxes[7][4][2]{
	{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } },	//"田" 字形方块
	{ { 1, 0 }, { 0, 0 }, { 2, 0 }, { 3, 0 } },	// "1" 字形方块
	{ { 1, 1 }, { 0, 0 }, { 1, 0 }, { 2, 1 } },	//"闪电"形方块
	{ { 1, 1 }, { 0, 1 }, { 1, 0 }, { 2, 0 } },	//"反闪电" 形方块
	{ { 1, 0 }, { 0, 0 }, { 2, 0 }, { 1, 1 } },	//"凸"	字形方块
	{ { 1, 0 }, { 0, 0 }, { 0, 1 }, { 2, 0 } },	//"反L" 形方块
	{ { 1, 0 }, { 0, 0 }, { 2, 0 }, { 2, 1 } }	//"L"  形方块
```
这里提供了6钟方块的颜色：

```c++
//设置方块的颜色
Colors[0] = Color3B(255, 0, 0);
Colors[1] = Color3B(0, 255, 0);
Colors[2] = Color3B(0, 0, 255);
Colors[3] = Color3B(255, 255, 0);
Colors[4] = Color3B(255, 0, 255);
Colors[5] = Color3B(0, 255, 255);
```
至于会拿到什么颜色的什么形状的方块，是随机的，采用随机数的方法随机抽取一种。同理，预览的方块也是随机的：

```c++
shape = preShape;
preShape = rand() % 7;
preColor = rand() % 6;
PreviewBox(boxes[preShape]);
```
#### 移动和控制

分为键盘控制和按键控制。键盘监听事件已经在头文件中定义：

```c++
void onKeyPressed(EventKeyboard::KeyCode keycode, Event *event);
```

其中KeyCode定义了键盘上所有的按键。键盘按键通过与KeyCode的定义值进行比较，可以判断出键盘是否按出来我们所需要的键。要得出按下了哪一个键，通过输出KeyCode的值就可以得到。
```c++
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
```

判断按了屏幕哪个键则可以通过触摸事件实现。利用cocos2d-x提供的方法获取触摸的位置，当出没的位置与我们设定的位置相符合时，则进行相应的操作。
```c++
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
```

#### 碰撞检测

边界碰撞检测：整个游戏有三个边界分别是：左侧、右侧、底侧。

将正在移动的方块的像素坐标转换为格子坐标，左移一格的话格子坐标减1。当格子坐标减1后小于0时，则代表会出界。右移下移同理。

```c++
for (int i = 0; i < 4; i++)
{
	if (M_x[i] - 1 < 0)				
	{
		return false;
	}
}
```
判断左右移是否会碰撞到其他方块：获取已经停止的方块，将其像素坐标转换为格子坐标，随后进行碰撞判断。下移同理。

```c++
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
```
方块旋转的碰撞检测：同理，如果旋转后的方块的横/纵坐标经过判断后发现与某已经停止的方块相同，则不能旋转。关于旋转规则，本游戏采用的是传统的俄罗斯方块旋转算法。现代俄罗斯方块大多采用的是SRS旋转系统，这种旋转系统更加灵活，有了更多的转法，也因此出现了T-spin，L-spin，I-spin等等，甚至衍生出了相关的开局定式，如DT炮，信天翁等。不过SRS旋转系统涉及到踢墙的判断，相对复杂，本游戏采用相对简单的旋转变换，不同类型的方块的旋转操作改变坐标即可，如果旋转后的位置被占用就返回原状态。

```c++
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
```
消行：变量sum作为累加一行的方块数目。每次落下方块时，对每一行进行判断。俄罗斯方块每行10格，对当前行已经停止的方块进行遍历，如果在对应位置能够获取到方块，则筛选并保留方块，同时sum加1；当sum=10时，将此行方块消除并加分：消除之后，将上面的方块给移下来。

```c++
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
```
整体框架大致就这样，其他功能比如bgm暂停按钮，分数记录等等，都是在游戏基本功能实现之后添加的功能，这里不累述，基本上各个代码段实现什么功能基本都注释了，各个模块的实现在注释上都有标注。