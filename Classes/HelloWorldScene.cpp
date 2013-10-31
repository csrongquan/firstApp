#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
	if ( !LayerColor::initWithColor(ccc4(255,255,255,255)))
    {
        return false;
    }
    
	_targets = new Array();
	_projectiles = new Array();

	setTouchEnabled(true);
	setTouchMode(Touch::DispatchMode::ONE_BY_ONE);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);


	Size winSize = Director::sharedDirector()->getWinSize();
	Sprite *player = Sprite::create("Player.png",
		CCRectMake(0, 0, 27, 40) );
	player->setPosition( ccp(player->getContentSize().width/2, winSize.height/2) );
	this->addChild(player);
    
	schedule( schedule_selector(HelloWorld::gameLogic), 1.0 );
    return true;
}

void HelloWorld::addTraget()
{
	Sprite *sprite = Sprite::create("Target.png",Rect::Rect(0,0,27,40));

	Size winSize = Director::getInstance()->getWinSize();
	int minY = sprite->getContentSize().height/2;
	int maxY = winSize.height - minY;
	int rangY = maxY - minY;
	int actualY = (rand()%rangY)+minY;
	sprite->setPosition(Point::Point(winSize.width + sprite->getContentSize().width/2,actualY));
	addChild(sprite);

	sprite->setTag(1);
	_targets->addObject(sprite);

	int minDuration = 2;
	int maxDuration = 4;
	int rangDuration = maxDuration - minDuration;
	int actualDuration = (rand()%rangDuration) + minDuration;

	FiniteTimeAction* actionMove = MoveTo::create((float)actualDuration,Point::Point(0 - sprite->getContentSize().width/2,actualY));

	FiniteTimeAction* actionMoveDone = CallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished));
	sprite->runAction( Sequence::create(actionMove, 
		actionMoveDone, NULL) );
}

void HelloWorld::spriteMoveFinished(Node* sender)
{
  Sprite *sprite = (Sprite *)sender;
  this->removeChild(sprite, true);
  
  if(1==sprite->getTag())
  {
	  _targets->removeObject(sprite);
  }
  else if(2==sprite->getTag())
  {
	  _projectiles->removeObject(sprite);
  }
}

void HelloWorld::gameLogic(float dt)
{
	addTraget();
}

bool HelloWorld::onTouchBegan(Touch *touch, Event *event)
{
	return true;
}

void HelloWorld::onTouchEnded(Touch *touch, Event *event)
{
     // Choose one of the touches to work with
	Point location = touch->getLocation();
	//location = Director::getInstance()->convertToGL(location);

    // Set up initial location of projectile
    Size winSize = Director::getInstance()->getWinSize();
    Sprite *projectile = Sprite::create("Projectile.png", 
		Rect(0, 0, 20, 20));
	projectile->setPosition( Point(20, winSize.height/2) );

    // Determinie offset of location to projectile
    int offX = location.x - projectile->getPosition().x;
    int offY = location.y - projectile->getPosition().y;

    // Bail out if we are shooting down or backwards
    if (offX <= 0) return;

    // Ok to add now - we've double checked position
    this->addChild(projectile);
	projectile->setTag(2);
	_projectiles->addObject(projectile);

    // Determine where we wish to shoot the projectile to
    int realX = winSize.width + (projectile->getContentSize().width/2);
    float ratio = (float)offY / (float)offX;
    int realY = (realX * ratio) + projectile->getPosition().y;
	Point realDest = Point(realX, realY);

    // Determine the length of how far we're shooting
    int offRealX = realX - projectile->getPosition().x;
    int offRealY = realY - projectile->getPosition().y;
    float length = sqrtf((offRealX * offRealX) + (offRealY*offRealY));
    float velocity = 480/1; // 480pixels/1sec
    float realMoveDuration = length/velocity;

    // Move projectile to actual endpoint
    projectile->runAction( CCSequence::create(
        CCMoveTo::create(realMoveDuration, realDest),
        CCCallFuncN::create(this, 

        callfuncN_selector(HelloWorld::spriteMoveFinished)), 
        NULL) );

}

void HelloWorld::update(float dt)
{
    Array *projectilesToDelete = new Array;
    Array* targetsToDelete =new Array;
    Object* it = NULL;
    Object* jt = NULL;

	CCARRAY_FOREACH(_projectiles, it)
    {
        Sprite *projectile = dynamic_cast<Sprite*>(it);
		Rect projectileRect = Rect(
                                           projectile->getPosition().x - (projectile->getContentSize().width/2),
                                           projectile->getPosition().y - (projectile->getContentSize().height/2),
                                           projectile->getContentSize().width,
                                           projectile->getContentSize().height);

		CCARRAY_FOREACH(_targets, jt)
        {
            Sprite *target = dynamic_cast<Sprite*>(jt);
			Rect targetRect = RECT(
                                           target->getPosition().x - (target->getContentSize().width/2),
                                           target->getPosition().y - (target->getContentSize().height/2),
                                           target->getContentSize().width,
                                           target->getContentSize().height);

            if (projectileRect.intersectsRect(targetRect))
            {
                targetsToDelete->addObject(target);
                projectilesToDelete->addObject(projectile);
            }
        }
    }

    CCARRAY_FOREACH(targetsToDelete, jt)
    {
        Sprite *target = dynamic_cast<Sprite*>(jt);
        _target->removeObject(target);
        this->removeChild(target, true);
    }

    CCARRAY_FOREACH(projectilesToDelete, it)
    {
        Sprite* projectile = dynamic_cast<Sprite*>(it);
        _bullet->removeObject(projectile);
        this->removeChild(projectile, true);
    }

    projectilesToDelete->release();
    targetsToDelete->release();
}    

void HelloWorld::menuCloseCallback(Object* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

HelloWorld::HelloWorld():_targets(NULL),_projectiles(NULL)
{
}

HelloWorld::~HelloWorld()
{
	if(_targets)
	{
		_targets->release();
		_targets = NULL;
	}
	if(_projectiles)
	{
		_projectiles->release();
		_projectiles = NULL;
	}
}
