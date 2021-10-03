#include <sfml/graphics.hpp>
#include <SFML/Audio.hpp>
#include "View.h"
#include <iostream>
#include "Mission.h"
#include "iostream"
#include "Level.h"
#include <vector>
#include <list>
#include <memory>
#include <fstream>
using std::endl;
using std::cout;
using namespace sf;
int Menu = 1,gamerun=1,continuee=0 , gg=1, AdvMenu = 0, RaznMenu = 0,nummer=0,level=1;
bool gamestart = true;
int LvUpD = 0, LvUpS = 0;
int bought[2] = { 0,0 };
int x=1920, y=1080;
RenderWindow window(VideoMode(x, y), "2D Shooter");
////////////////////////////////////Общий класс-родитель//////////////////////////
class Entity {
public:
	bool movement =false;
	enum { left, right, up, down, jump, stay, right_Top } state;
	std::vector<Object> obj,obj2;
	float dx, dy, x, y, speed, moveTimer;
	int w, h,armour=100, health = 1000;
	bool life, isMove, onGround;
	Texture texture;
	Sprite sprite;
	String name;
	Entity(Image& image, String Name, float X, float Y, int W, int H) {
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; armour = 100; dx = 0; dy = 0;
		life = true;  onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}

	FloatRect getRect() {
		return FloatRect(x, y, w, h);
	}

	virtual void update(float time) = 0;//все потомки переопределяют эту ф-цию

};

class Player :public Entity
{
public:
	enum { left, right, up, down, jump, stay, right_Top } state;
	float playerScore;
	bool isShoot;
	float CurrentFrame = 0;
	int shoot = 200000;
	int weapon = 2;
	Player(Image& image, String Name, Level& lev, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		playerScore = 100; isShoot = 1; state = stay; obj = lev.GetAllObjects();
		if (name == "Player1") {
			sprite.setTextureRect(IntRect(315, 250, 45,50));
			
		}
	}

	

	void control()
	{
		Clock clock;
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 800;
		if(onGround==true)
		sprite.setTextureRect(IntRect(315, 250, 45, 50));
		if (Keyboard::isKeyPressed) {
			if (Keyboard::isKeyPressed(Keyboard::Left)||Keyboard::isKeyPressed(Keyboard::A)) {
				state = left; speed = 0.2;
					if (CurrentFrame > 6) CurrentFrame -= 6;
					
					sprite.setTextureRect(IntRect(45 * int(CurrentFrame), 300, 45, 50));
					
					if (life == false)speed = 0;
				
			}
			if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) {
				state = right; speed = 0.2;
				
				if (CurrentFrame > 6) CurrentFrame -= 6;
				sprite.setTextureRect(IntRect(45 * int(CurrentFrame), 250, 45, 50));
				
				if (life == false)speed = 0;
			}

			if ((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) && (onGround)) {
				state = jump; dy = -0.45; onGround = false;
				sprite.setTextureRect(IntRect(270, 345, 45, 50));
				if (life == false)dy=0;
				if (dy == 0);
			}

			if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) 
			{
				state = down;
				
				if (CurrentFrame > 6) CurrentFrame -= 6;
				sprite.setTextureRect(IntRect(135 , 0, 45, 50));
			}

			if ((Keyboard::isKeyPressed(Keyboard::Right)) && (Keyboard::isKeyPressed(Keyboard::Up)) || Keyboard::isKeyPressed(Keyboard::D)&& (Keyboard::isKeyPressed(Keyboard::W))) 
			{
				state = right_Top;
			}

			/////выстрел
			if (Keyboard::isKeyPressed(Keyboard::Space))
			{
				isShoot = true;
			}
		}
		
	}

	void checkCollisionWithMap(float Dx, float Dy)
	{
		
		for (int i = 0; i < obj.size(); i++)

			if (getRect().intersects(obj[i].rect))
			{
				if (obj[i].name == "solid")
				{
					if (Dy > 0) { y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy < 0) { y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx > 0) { x = obj[i].rect.left - w;}
					if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width;}
				}
				if (obj[i].name == "FinishLevel") 
				{
					level += 1;
				}
				 //else { onGround = false; }
			}
	}

	void update(float time)
	{
		control();
		switch (state)
		{
		case right:dx = speed; CurrentFrame += 0.005 * time; break;
		case left:dx = -speed; CurrentFrame += 0.005 * time;  break;
		case up: break;
		case down: dx = 0; CurrentFrame += 0.005 * time; break;
		case stay: break;
		case right_Top: dx = speed; break;//состояние вправо вверх, просто продолжаем идти вправо
		}
		x += dx * time;
		checkCollisionWithMap(dx, 0);
		y += dy * time;
		checkCollisionWithMap(0, dy);
		sprite.setPosition(x + w / 2, y + h / 2);
		if (health <= 0) 
		{ 
			health = 0;
			life = false;
		}
		if (armour <= 0)
			armour = 0;
		if (life == false)
		{
			speed = 0;
			sprite.setTextureRect(IntRect(315, 350, 45, 50));
			sprite.setColor(Color::Red);
			isShoot = false;
		}
		if (!isMove) { speed = 0; }
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0015 * time;
	}
};

class Enemy :public Entity {
public:
	float CurrentFrame1 = 0; bool isShoot = 1, onGround = true;;

	Enemy(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		obj = lvl.GetObjects("solid");//инициализируем.получаем нужные объекты для взаимодействия врага с картой
		obj = lvl.GetObjects("solidInvis");
		if (name == "EasyEnemy") {
			dx = 0.21;
		}
		if (name == "EasyShootEnemy") {
			isShoot;
			dx = 0.21;
		}
	}

	void checkCollisionWithMap(float Dx, float Dy, float time)
	{
		if (name == "EasyEnemy" || name == "EasyShootEnemy") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (CurrentFrame1 > 6) CurrentFrame1 -= 6;
				if (name == "EasyEnemy")
					sprite.setColor(Color(200, 190, 75));
				sprite.setTextureRect(IntRect(45 * int(CurrentFrame1), 200, 45, 50));

				sprite.getTransform();
				if (getRect().intersects(obj[i].rect))
				{
					if (obj[i].name == "solid" || obj[i].name == "solidInvis")//если встретили препятствие
					{
						if (Dy > 0)
						{
							y = obj[i].rect.top - h;  dy = 0; onGround = true;
						}
						if (Dy < 0)
						{
							y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
						}
						if (Dx > 0)
						{
							x = obj[i].rect.left - w;
							dx = -0.21;
							sprite.scale(-1, 1);

						}
						if (Dx < 0)
						{
							x = obj[i].rect.left + obj[i].rect.width;
							dx = 0.21;
							sprite.scale(-1, 1);
							moveTimer += time;

						}
					}
				}
			}
		}
	}


	void update(float time)
	{

		if (name == "EasyEnemy") {
			CurrentFrame1 += 0.005 * time;
			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек(альтернативная версия смены направления)
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
			if (health <= 0)
			{
				life = false;
			}
			/*if (life = false)
			{
				sprite.setColor(Color::Red);
			}*/
		}
		if (name == "EasyShootEnemy") {
			CurrentFrame1 += 0.005 * time;
			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек(альтернативная версия смены направления)
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
			if (health <= 0)
			{
				life = false;
			}


		}
	}
};

class Weapon :public Entity
{
public:
	float CurrentFrame = 0;
	Weapon(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H)
	{
		obj = lvl.GetObjects("solid");
		if (name == "ShootGun")
		{

		}
		if (name == "AWP")
		{

		}
		if (name == "AutoGun")
		{

		}
	}

	void checkCollisionWithMap(float Dx, float Dy, float time)
	{
		if (name == "ShootGun") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "AWP") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "AutoGun") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
	}

	void update(float time)
	{

		if (name == "ShootGun") {
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "AWP") {
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "AutoGun") {
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
	}
};

class Bonus :public Entity
{
public:
	float CurrentFrame = 0;
	//int ammo = 10;
	Bonus(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H)
	{
		obj = lvl.GetObjects("solid");
		if (name == "Money1")
		{
			sprite.setTextureRect(IntRect(0, 0, 32, 32));
		}
		if (name == "Money2")
		{
			sprite.setTextureRect(IntRect(0, 0, 32, 32));
		}
		if (name == "Money3")
		{
			sprite.setTextureRect(IntRect(0, 0, 32, 32));
		}
		if (name == "Ammo")
		{
			sprite.setTextureRect(IntRect(0, 0, 43, 41));
			sprite.scale(0.7, 0.7);

		}
		if (name == "Heal")
		{
			sprite.setTextureRect(IntRect(0, 0, 29, 38));

		}
		if (name == "Armour")
		{
			sprite.setTextureRect(IntRect(0, 0, 37, 48));

		}
	}

	void checkCollisionWithMap(float Dx, float Dy, float time)
	{
		if (name == "Money1")
		{
			for (int i = 0; i < obj.size(); i++)
			{
				if (CurrentFrame > 8) CurrentFrame -= 8;
				sprite.setTextureRect(IntRect(32 * int(CurrentFrame), 0, 32, 32));
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "Money2")
		{
			for (int i = 0; i < obj.size(); i++)
			{
				if (CurrentFrame > 8) CurrentFrame -= 8;
				sprite.setTextureRect(IntRect(32 * int(CurrentFrame), 0, 32, 32));
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "Money3")
		{
			for (int i = 0; i < obj.size(); i++)
			{
				if (CurrentFrame > 8) CurrentFrame -= 8;
				sprite.setTextureRect(IntRect(32 * int(CurrentFrame), 0, 32, 32));
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "Ammo") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "Heal") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}
		if (name == "Armour") {
			for (int i = 0; i < obj.size(); i++)
			{
				if (obj[i].name == "solid")//если встретили препятствие
				{
					if (Dy > 0)
					{
						y = obj[i].rect.top - h;  dy = 0; onGround = true;
					}
					if (Dy < 0)
					{
						y = obj[i].rect.top + obj[i].rect.height;   dy = 0;
					}
				}

			}
		}

	}
	void update(float time)
	{

		if (name == "Money1") {
			CurrentFrame += 0.005 * time;
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "Money2") {
			CurrentFrame += 0.005 * time;
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "Money3") {
			CurrentFrame += 0.005 * time;
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "Ammo") {
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "Heal") {
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
		if (name == "Armour") {
			checkCollisionWithMap(dx, 0, time);
			x += dx * time;
			sprite.setPosition(x + w / 2, y + h / 2);
		}
	}
};

class Bullet :public Entity//класс пули
{
public:
	float targetX, targetY;//отвечают за направление движения пули к цели
	float vx, vy, distance;
	Bullet(Image& image, String Name, Level& lvl, float X, float Y, int W, int H, float tX, float tY) :Entity(image, Name, X, Y, W, H) {//всё так же, только взяли в конце состояние игрока (int dir)

		obj = lvl.GetObjects("solid");
		obj2 = lvl.GetObjects("solidInvis");//инициализируем .получаем нужные объекты для взаимодействия пули с картой
		x = X;
		y = Y;
		targetX = tX;
		targetY = tY;
		speed = 0.2;
		w = h = 16;
		life = true;
		//distance = sqrt((targetX - x) - (targetY - y));
		vx = (targetX - x) / 30; // 100 - дистанция
		vy = (targetY - y) / 30;
		//выше инициализация в конструкторе
	}


	void update(float time)
	{

		//x += speed*time*(targetX - x) /20;//само движение пули по х
		//y += speed*time*(targetY - y) /20;//по у

		x += vx * time * speed;
		y += vy * time * speed;

		if (x <= 0) x = 1;// задержка пули в левой стене, чтобы при проседании кадров она случайно не вылетела за предел карты и не было ошибки
		if (y <= 0) y = 1;

		for (int i = 0; i < obj.size(); i++) {//проход по объектам solid
			for (int i = 0; i < obj2.size(); i++) {
				if (getRect().intersects(obj[i].rect) || getRect().intersects(obj2[i].rect)) //если этот объект столкнулся с пулей,
				{
					if (obj[i].name == "solid" || obj2[i].name == "solidInvis")
					{
						life = false;// то пуля умирает
					}
				}
			}
		}
		sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
	}
};

class MovingPlatform : public Entity {//класс движущейся платформы
public:
	MovingPlatform(Image& image, String Name, Level& lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H) {
		sprite.setTextureRect(IntRect(0, 0, W, H));//прямоугольник 
		dx = 0.08;//изначальное ускорение по Х
	}

	void update(float time)//функция обновления платформы.
	{
		x += dx * time;//реализация движения по горизонтали
		moveTimer += time;//наращиваем таймер
		if (moveTimer > 2000) { dx *= -1; moveTimer = 0; }//если прошло примерно 2 сек, то меняется направление движения платформы, а таймер обнуляется
		sprite.setPosition(x + w / 2, y + h / 2);//задаем позицию спрайту
	}
};

void set_profiles_balance(float arr)
{
	std::fstream file_record("money.txt");

	if (!file_record.is_open())
	{
		exit(0);
	}

	file_record.clear();

	file_record << arr;
	file_record.close();
}

void get_profile_balance(float arr)
{
	std::fstream file_profiles("money.txt");
	if (!file_profiles.is_open())
	{
		exit(0);
	}
	std::string number_string;
	getline(file_profiles, number_string);
	if (number_string.empty())
	{
		arr = 0;
	}
	else
	{
		arr = stoi(number_string);
	}
	number_string.clear();
	file_profiles.close();

}

void get_profile_goods(int arr[])
{
	std::fstream file_profiles("goods.txt");
	if (!file_profiles.is_open())
	{
		exit(0);
	}
	std::string number_string;
	for (int i = 0; i < 1; i++)
	{
		getline(file_profiles, number_string);
		if (number_string.empty())
		{
			arr[i] = 0;
		}
		else
		{
			arr[i] = stoi(number_string);
		}
		number_string.clear();
	}
	file_profiles.close();

}

void set_profiles_goods(int arr[])
{
	std::fstream file_record("goods.txt");

	if (!file_record.is_open())
	{
		exit(0);
	}

	file_record.clear();

	for (int i = 0; i < 2; i++)
	{
		file_record << arr[i] << "\n";
	}
	file_record.close();
}

void set_pos_player(Player p, float xHP, float xA, float xSC, int& damage, int& LvUpS, int& LvUpD)
{
	std::fstream file_record("pos_player.txt");

	if (!file_record.is_open())
	{
		exit(0);
	}

	file_record.clear();
	file_record << p.x << " ";
	file_record << p.y << " ";
	file_record << p.health << " ";
	file_record << p.armour << " ";
	file_record << p.shoot << " ";
	file_record << p.playerScore << " ";
	file_record << p.weapon << " ";
	file_record << xHP << " ";
	file_record << xA << " ";
	file_record << xSC << " ";
	file_record << damage << " ";
	file_record << LvUpD << " ";
	file_record << LvUpS << " ";
	file_record << p.life << "\n";
	
	file_record.close();
}

void get_pos_player(Player& p, float& xHP, float& xA, float& xSC, int& damage, int& LvUpS, int& LvUpD)
{
	std::fstream file_profiles("pos_player.txt");
	std::vector<double>vec;
	if (!file_profiles.is_open())
	{
		exit(0);
	}
	std::string number_string;
	for (int i = 0; i<=13; i++)
	{

		file_profiles >> number_string;

		vec.push_back(stod(number_string));
		
		number_string.clear();
	}

	file_profiles.close();
	p.x = vec[0];
	p.y = vec[1];
	p.health = vec[2];
	p.armour = vec[3];
	p.shoot = vec[4];
	p.playerScore = vec[5];
	p.weapon = vec[6];
	xHP = vec[7];
	xA = vec[8];
	xSC = vec[9];
	damage = vec[10];
	LvUpD = vec[11];
	LvUpS = vec[12];
	p.life = vec[13];
}

void purchase(int num, RenderWindow& window, int& damage, Player& p,int& LvUpS, int& LvUpD,int bought[])
{
	/*Text no_money;
	no_money.set_text("NOT ENOUGH MONEY!");
	no_money.set_pos(490, 260);

	text purch;
	purch.set_text("ALREADY PURCHASED!");
	purch.set_pos(490, 260);*/

	//int price[4] = { 1500,3000,5000,0 };

	get_profile_goods(bought);
	get_profile_balance(p.playerScore);

	if (num == 1)
	{
		if (damage < 3)
		{
			if (p.playerScore >= 3 && bought[0] <3)
			{
				
				p.playerScore -= 3;
				
				damage += 1;
				LvUpD += 1;
				bought[0] = damage;
				set_profiles_balance(p.playerScore);
				set_profiles_goods(bought);
				sleep(seconds(0.5));
			}
			else if (p.playerScore < 3)
			{
				/*no_money.draw_user(window);*/
				window.display();
				cout << "No money" << endl;
				/*sleep(seconds(0.5));*/
			}

			else if (bought[0] <3)
			{
				/*purch.draw_user(window);*/
				damage += 1;
				cout << "--" << endl;
				cout << damage << endl;
				cout << "--" << endl;
				LvUpD += 1;
				window.display();
				sleep(seconds(0.5));
			}
		}
		else if(bought[0]==3)
		{
			cout << "Full damage" << endl;
		}

	}

	else if (num == 2)
	{
		if (p.speed < 3)
		{
			if (p.playerScore >= 2 && bought[1] < 3)
			{
				LvUpS += 1;
				bought[1] = LvUpS;
				p.playerScore -= 2;
				set_profiles_balance(p.playerScore);
				set_profiles_goods(bought);
				p.speed += 0.05;
				sleep(seconds(0.5));
			}

			else if (p.playerScore < 2)
			{
				//no_money.draw_user(window);
				window.display();
				cout << "No money" << endl;
				//sleep(seconds(0.5));
			}

			else if (bought[1] == 1)
			{
				//purch.draw_user(window);
				p.speed += 0.05;
				LvUpS += 1;
				window.display();
				sleep(seconds(0.5));
			}
		}
		else
		{
			cout << "Full speed" << endl;
		}
	}
	window.display();
}

void shop(RenderWindow& window, int& num, Player& p,int& damage,int& LvUpS,int& LvUpD)
{
	float CurrentFrame = 0;
	Texture backg, Damage, d1, d2, d3,td1,td2,td3;

	backg.loadFromFile("images/giff.png");
	td1.loadFromFile("images/Damage1.png");
	td2.loadFromFile("images/Damage2.png");
	td3.loadFromFile("images/Damage3.png");
	d1.loadFromFile("images/nextUP.png");
	d2.loadFromFile("images/nextUP.png");
	d3.loadFromFile("images/nextUP.png");

	Damage.loadFromFile("images/Damage.png");
	Sprite DamageUP(Damage), backgg(backg),D1(d1),D2(d2),D3(d3),TD1(td1),TD2(td2),TD3(td3);

	RectangleShape  RectangleD(Vector2f(256.f, 128.f)), Rectangled1(Vector2f(256.f, 128.f)), Rectangled2(Vector2f(256.f, 128.f)), Rectangled3(Vector2f(256.f, 128.f));

	RectangleD.setPosition(200, 600);
	RectangleD.setFillColor(Color::Transparent);

	Rectangled1.setPosition(200, 200);
	Rectangled1.setFillColor(Color::Transparent);
	Rectangled2.setPosition(200, 600);
	Rectangled2.setFillColor(Color::Transparent);
	Rectangled3.setPosition(200, 200);
	Rectangled3.setFillColor(Color::Transparent);

	DamageUP.setPosition(200, 200);

	TD1.setPosition(500, 400);
	TD2.setPosition(800, 400);
	TD3.setPosition(1100, 400);
	D1.setPosition(500,200);
	D2.setPosition(800,200);
	D3.setPosition(1100,200);


	Clock clock;

	while (AdvMenu == 1)
	{
		window.setVerticalSyncEnabled(true);
		Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				window.close();
		}
		float time = clock.getElapsedTime().asMicroseconds();

		int num = 0;

		clock.restart();

		time = time / 800;
		

		CurrentFrame += 0.005 * time;

		backgg.setPosition(210, 0);
		backgg.setScale(3, 2);

		if (CurrentFrame > 24) CurrentFrame -= 24;
		backgg.setTextureRect(IntRect(500 * int(CurrentFrame), 0, 500, 500));

		
		if (IntRect(200, 200, 256, 128).contains(Mouse::getPosition(window)))
		{
			DamageUP.setColor(Color::Green);
			if (Mouse::isButtonPressed(Mouse::Left)) 
			{
					nummer = 1;
					purchase(nummer, window,damage, p,LvUpS,LvUpD,bought);
				
			}
		}
		else
		{
			DamageUP.setColor(Color::White);
		}
		window.clear();
		if (bought[0] == 1) { D1.setColor(Color::Blue); }
		if (bought[0] == 2) { D2.setColor(Color::Blue); D1.setColor(Color::Blue);}
		if (bought[0] >= 3) {D3.setColor(Color::Blue);  D2.setColor(Color::Blue); D1.setColor(Color::Blue);}	
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			AdvMenu = 0;
			Menu = 0;
			continuee = 1;
		}
		//window.clear();
		if (AdvMenu)
		{
			
			window.draw(backgg);
			window.draw(D1);
			window.draw(D2);
			window.draw(D3);
			window.draw(TD1);
			window.draw(TD2);
			window.draw(TD3);
			window.draw(DamageUP);
		}
		
		window.display();
	}
}

void levelchange(RenderWindow& window)
{
	float CurrentFrame = 0;
	Texture backg, l1, l2, l3, l4, l5;

	backg.loadFromFile("images/background.jpg");

	l1.loadFromFile("images/1.png");
	l2.loadFromFile("images/2.png");
	l3.loadFromFile("images/3.png");
	l4.loadFromFile("images/4.png");
	l5.loadFromFile("images/5.png");

	Sprite backgg(backg), L1(l1), L2(l2), L3(l3), L4(l4), L5(l5);

	RectangleShape RectangleL1(Vector2f(128.f, 128.f)), RectangleL2(Vector2f(128.f, 128.f)), RectangleL3(Vector2f(128.f, 128.f)), RectangleL4(Vector2f(128.f, 128.f)), RectangleL5(Vector2f(128.f, 128.f));

	RectangleL1.setPosition(500, 200);
	RectangleL1.setFillColor(Color::Transparent);
	RectangleL2.setPosition(800, 200);
	RectangleL2.setFillColor(Color::Transparent);

	RectangleL3.setPosition(1100, 200);
	RectangleL3.setFillColor(Color::Transparent);
	RectangleL4.setPosition(500, 600);
	RectangleL4.setFillColor(Color::Transparent);
	RectangleL5.setPosition(800, 600);
	RectangleL5.setFillColor(Color::Transparent);


	L1.setPosition(500, 200);
	L2.setPosition(800, 200);
	L3.setPosition(1100, 200);
	L4.setPosition(500, 600);
	L5.setPosition(800, 600);

	while (AdvMenu == 1)
	{
		window.setVerticalSyncEnabled(true);
		Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				window.close();
		}

		int num = 0;

		backgg.setPosition(0, 0);


		if (IntRect(500, 200, 128, 128).contains(Mouse::getPosition(window)))
		{
			L1.setColor(Color::Green);
			if (Mouse::isButtonPressed(Mouse::Left))
				level = 1;
		}
		else
		{
			L1.setColor(Color::White);
		}
		if (IntRect(800, 200, 128, 128).contains(Mouse::getPosition(window)))
		{
			L2.setColor(Color::Green);
			if (Mouse::isButtonPressed(Mouse::Left))
				level = 2;
		}
		else
		{
			L2.setColor(Color::White);
		}
		if (IntRect(1100, 200, 128, 128).contains(Mouse::getPosition(window)))
		{
			L3.setColor(Color::Green);
			if (Mouse::isButtonPressed(Mouse::Left))
				level = 3;
		}
		else
		{
			L3.setColor(Color::White);
		}
		if (IntRect(500, 600, 128, 128).contains(Mouse::getPosition(window)))
		{
			L4.setColor(Color::Green);
			if (Mouse::isButtonPressed(Mouse::Left))
				level = 4;
		}
		else
		{
			L4.setColor(Color::White);
		}
		if (IntRect(800, 600, 128, 128).contains(Mouse::getPosition(window)))
		{
			L5.setColor(Color::Green);
			if (Mouse::isButtonPressed(Mouse::Left))
				level = 5;
		}
		else
		{
			L5.setColor(Color::White);
		}
		window.clear();

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			AdvMenu = 0;
			Menu = 1;
		}
		//window.clear();
		if (AdvMenu)
		{

			window.draw(backgg);
			window.draw(L1);
			window.draw(L2);
			window.draw(L3);
			window.draw(L4);
			window.draw(L5);
		}

		window.display();
	}
}

void menu(RenderWindow& window)
{
	
	Texture background,  ContinueT, NewGameT, SettingsT, InfoT, QuitT, ChooseLevelT, InfoGG;

	background.loadFromFile("images/background.jpg");
	InfoGG.loadFromFile("images/InfoG.png");
	ContinueT.loadFromFile("images/Continue.png");
	NewGameT.loadFromFile("images/NewGame.png");
	SettingsT.loadFromFile("images/Settings.png");
	InfoT.loadFromFile("images/Info.png");
	QuitT.loadFromFile("images/Quit.png");
	ChooseLevelT.loadFromFile("images/ChooseLevel.png");

	

	Sprite menuBG(background), Continue(ContinueT), NewGame(NewGameT), Settings(SettingsT), Info(InfoT),InfoG(InfoGG), Quit(QuitT), ChooseLevel(ChooseLevelT);
	int menuNum = 0,dd=0;

	RectangleShape RectangleCon(Vector2f(500.f, 104.f)), RectangleNG(Vector2f(500.f, 104.f)), RectangleSet(Vector2f(500.f, 104.f)), RectangleI(Vector2f(500.f, 104.f)), RectangleQ(Vector2f(500.f, 104.f)), RectangleCHL(Vector2f(500.f, 104.f));
	RectangleCon.setPosition(125, 150);
	RectangleCon.setFillColor(Color::Transparent);
	RectangleNG.setPosition(125, 250);
	RectangleNG.setFillColor(Color::Transparent);
	RectangleCHL.setPosition(125, 350);
	RectangleCHL.setFillColor(Color::Transparent);
	RectangleSet.setPosition(125, 450);
	RectangleSet.setFillColor(Color::Transparent);
	RectangleI.setPosition(125, 550);
	RectangleQ.setPosition(125, 650);
	RectangleQ.setFillColor(Color::Transparent);
	RectangleI.setFillColor(Color::Transparent);
	menuBG.setPosition(0, 0);

	
	
	Continue.setPosition(50, 150);
	Continue.setScale(0.5,0.5);
	NewGame.setPosition(50, 250);
	NewGame.setScale(0.5, 0.5);
	ChooseLevel.setPosition(50, 350);
	ChooseLevel.setScale(0.5, 0.5);
	Settings.setPosition(50, 459);
	Settings.setScale(0.5, 0.5);
	Info.setPosition(50, 560);
	Info.setScale(0.5, 0.5);
	Quit.setPosition(50, 669);
	Quit.setScale(0.5, 0.5);


	while (Menu)
	{



		Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (IntRect(125, 150, 550, 104).contains(Mouse::getPosition(window)))
		{
			Continue.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left))
				menuNum = 1;

		}
		else
		{
			Continue.setColor(Color::White);
		}
		if (IntRect(125, 250, 550, 104).contains(Mouse::getPosition(window)))
		{
			NewGame.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left))
				menuNum = 2;
		}
		else
		{
			NewGame.setColor(Color::White);
		}
		if (IntRect(125, 350, 550, 104).contains(Mouse::getPosition(window)))
		{
			ChooseLevel.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left))
				menuNum = 3;
		}
		else
		{
			ChooseLevel.setColor(Color::White);
		}
		if (IntRect(125, 459, 550, 104).contains(Mouse::getPosition(window)))
		{
			Settings.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left))
				menuNum = 4;
		}
		else
		{
			Settings.setColor(Color::White);
		}
		if (IntRect(125, 560, 550, 104).contains(Mouse::getPosition(window)))
		{
			Info.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left))
				menuNum = 5;
		}
		else
		{
			Info.setColor(Color::White);
		}
		if (IntRect(125, 669, 550, 104).contains(Mouse::getPosition(window)))
		{
			Quit.setColor(Color::Blue);
			if (Mouse::isButtonPressed(Mouse::Left))
				menuNum = 6;
		}
		else
		{
			Quit.setColor(Color::White);
		}
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) { continuee = 1; Menu = 0; gamerun = 1; gamestart = true; }//если нажали первую кнопку, то выходим из меню 
			if (menuNum == 2) {bought[0] = 0;bought[1] = 0; Menu = 0; gamerun = 1; gamestart = true; }
			if (menuNum == 3) { AdvMenu = 1; RaznMenu = 1; levelchange(window);}//если нажали первую кнопку, то выходим из меню 
			if (menuNum == 4) { AdvMenu = 1; RaznMenu = 2; }
			if (menuNum == 5) 
			{
				AdvMenu = 1; RaznMenu = 3; 
				dd = 1;
			}
			if (menuNum == 6) { Menu = 0; gamestart = false; gamerun = 0; gg = 0; }
		}

		window.clear();



		if (Menu)
		{
			window.draw(menuBG);
			window.draw(Continue);
			window.draw(NewGame);
			window.draw(Settings);
			window.draw(Quit);
			window.draw(Info);
			window.draw(ChooseLevel);
			window.draw(RectangleCon);
			window.draw(RectangleCHL);
			window.draw(RectangleNG);
			window.draw(RectangleSet);
			window.draw(RectangleI);
			window.draw(RectangleQ);
			if (dd == 1) 
			{
				window.draw(InfoG);
				if (Keyboard::isKeyPressed(Keyboard::Escape)) 
				{
					dd = 0;
				}

			}
			
		}
		window.display();

	}
}

void changeLevel(Level& lvl, int& numberLevel) {
	if (level == 1) { lvl.LoadFromFile("firstlevel.tmx"); }
	if (level == 2) { lvl.LoadFromFile("Secondlevel.tmx"); }
	if (level == 3) { lvl.LoadFromFile("Thirdlevel.tmx"); }
	if (level == 4) { lvl.LoadFromFile("Fourthlevel.tmx"); }
	if (level == 5) { lvl.LoadFromFile("Fivthlevel.tmx"); }
}

bool startGame(RenderWindow& window, int& numberLevel,bool& gamestart) {

	view.reset(FloatRect(0, 0, 640, 480));
	//window.setVerticalSyncEnabled(true);
	Level lvl;
	changeLevel(lvl, numberLevel);//для загрузки карты для нужного уровня

	set_profiles_goods(bought);
	//SoundBuffer shootBuffer;//создаём буфер для звука
	//shootBuffer.loadFromFile("shoot.ogg");//загружаем в него звук
	//Sound shoot(shootBuffer);//создаем звук и загружаем в него звук из буфера

	//Music music;//создаем объект музыки
	//music.openFromFile("music.ogg");//загружаем файл
	//music.play();//воспроизводим музыку
	//music.setLoop(true);

	Texture textureHPB, textureAB,textureSC;
	Sprite spriteHPB, spriteAB,spriteSC;

	Image heroImage;
	heroImage.loadFromFile("images/solid2.png");

	Image ABImage;
	ABImage.loadFromFile("images/ABar.png");

	Image SCImage;
	SCImage.loadFromFile("images/Score.png");


	Image HBImage;
	HBImage.loadFromFile("images/HPBar.png");

	Image ShGImage;
	ShGImage.loadFromFile("images/ShootGun.png");

	Image AWPImage;
	AWPImage.loadFromFile("images/AWP.png");

	Image AGImage;
	AGImage.loadFromFile("images/AutoGun.png");

	Image Money1Image;
	Money1Image.loadFromFile("images/Green32.png");

	Image Money2Image;
	Money2Image.loadFromFile("images/Blue32.png");

	Image Money3Image;
	Money3Image.loadFromFile("images/Yellow32.png");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/solid.png");

	Image easyEnemyshootImage;
	easyEnemyshootImage.loadFromFile("images/solid.png");

	Image movePlatformImage;
	movePlatformImage.loadFromFile("images/MovingPlatform.png");

	Image BulletImage;//изображение для пули
	BulletImage.loadFromFile("images/Bullet1.png");//загрузили картинку в объект изображения

	Image BulletAWPImage;//изображение для пули
	BulletAWPImage.loadFromFile("images/BulletAWP.png");//загрузили картинку в объект изображения

	Image BulletP90Image;//изображение для пули
	BulletP90Image.loadFromFile("images/BulletP90.png");//загрузили картинку в объект изображения

	Image BulletSHGImage;//изображение для пули
	BulletSHGImage.loadFromFile("images/BulletShootGun.png");//загрузили картинку в объект изображения

	Image BulletEnemyImage;//изображение для пули
	BulletEnemyImage.loadFromFile("images/BulletEnemy.png");//загрузили картинку в объект изображения

	Image AmmoImage;
	AmmoImage.loadFromFile("images/Ammo.png");

	Image HealImage;
	HealImage.loadFromFile("images/Heal.png");

	Image ArmourImage;
	ArmourImage.loadFromFile("images/Armour.png");

	std::list<Entity*>  entities;
	std::list<Entity*>::iterator it;
	std::list<Entity*>::iterator it2;//второй итератор.для взаимодействия между объектами списка

	std::vector<Object> e = lvl.GetObjects("EasyEnemy");

	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 45, 50));
	}
	e = lvl.GetObjects("EasyShootEnemy");
	
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Enemy(easyEnemyshootImage, "EasyShootEnemy", lvl, e[i].rect.left, e[i].rect.top, 45, 50));
	}
	Object player = lvl.GetObject("player");
	e = lvl.GetObjects("Money1");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Bonus(Money1Image, "Money1", lvl, e[i].rect.left, e[i].rect.top, 32, 32));
	}
	e = lvl.GetObjects("Money2");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Bonus(Money2Image, "Money2", lvl, e[i].rect.left, e[i].rect.top, 32, 32));
	}
	e = lvl.GetObjects("Money3");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Bonus(Money3Image, "Money3", lvl, e[i].rect.left, e[i].rect.top, 32, 32));
	}
	e = lvl.GetObjects("Ammo");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Bonus(AmmoImage, "Ammo", lvl, e[i].rect.left, e[i].rect.top, 43, 41));
	}
	e = lvl.GetObjects("Heal");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Bonus(HealImage, "Heal", lvl, e[i].rect.left, e[i].rect.top, 29, 38));
	}
	e = lvl.GetObjects("Armour");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Bonus(ArmourImage, "Armour", lvl, e[i].rect.left, e[i].rect.top, 29, 38));
	}
	e = lvl.GetObjects("ShootGun");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Weapon(ShGImage, "ShootGun", lvl, e[i].rect.left, e[i].rect.top, 29, 38));
	}
	e = lvl.GetObjects("AutoGun");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Weapon(AGImage, "AutoGun", lvl, e[i].rect.left, e[i].rect.top, 29, 38));
	}
	e = lvl.GetObjects("AWP");
	for (int i = 0; i < e.size(); i++)
	{
		entities.push_back(new Weapon(AWPImage, "AWP", lvl, 0, 0, 64, 25));
	}
	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 32, 48);
	e = lvl.GetObjects("MovingPlatform");//забираем все платформы в вектор 
	float moveTimer = 0, xHP = 640, xA = 640, xSC = 0, moveTimerEnemy = 0;
	if (continuee == 1) 
	{
		if(p.life==true)
		get_pos_player(p,xHP,xA,xSC, bought[0],LvUpS,LvUpD);
		get_profile_goods(bought);
		
		continuee = 0;
	}
	std::vector<float> VecObject{0,0,0};
	
	
	float moveTimer_weapon = 0;
	bool stopped = false;
	
	for (int i = 0; i < e.size(); i++) entities.push_back(new MovingPlatform(movePlatformImage, "MovingPlatform", lvl, e[i].rect.left, e[i].rect.top, 95, 22));//закидываем платформу в список.передаем изображение имя уровень координаты появления (взяли из tmx карты), а так же размеры
	Clock clock;
	float CurrentFrame1 = 0;
	while (window.isOpen())
	{
		if (Keyboard::isKeyPressed(Keyboard::F9))
		{
			get_pos_player(p,xHP,xA,xSC, bought[0],LvUpS, LvUpD);
			//return true;
		}
		float time = clock.getElapsedTime().asMicroseconds();

		clock.restart();
		time = time / 800;

		Event event;
		moveTimer += time;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				window.close();
			
			if (p.isShoot == true)
			{
				
				if (event.type == Event::MouseButtonPressed)//если нажата клавиша мыши
					if (event.key.code == Mouse::Left) //а именно левая, то стреляем 
					{ //и тут же снимаем координаты мыши в этом месте
						Vector2i pixelPos = Mouse::getPosition(window);//забираем коорд курсора
						Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)
						if (p.shoot <= 0)
						{
							p.shoot = 0;
						}
						if (p.shoot > 0)
						{
							int n = 30, nn = 15;
							if (p.weapon == 0)
							{

								
								if (moveTimer > 500)
								{
									entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
									moveTimer = 0;
								}
								
							}
							if (p.weapon == 2)
							{
								if (moveTimer > 700)
								{
									entities.push_back(new Bullet(BulletAWPImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
									moveTimer = 0;
								}
							}
							if (p.weapon == 1)
							{
								if (moveTimer > 600)
								{
									if (pos.y >= p.y)
									{
										if (pos.x <= p.x)
										{
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + nn, pos.y + nn));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + n, pos.y + n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - nn, pos.y - nn));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - n, pos.y - n));
										}
										if (pos.x > p.x)
										{
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + n, pos.y - n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + nn, pos.y - nn));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - n, pos.y + n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - nn, pos.y + nn));
										}

									}
									if (pos.y < p.y)
									{
										if (pos.x >= p.x)
										{
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + nn, pos.y + nn));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - n, pos.y - n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + n, pos.y + n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - nn, pos.y - nn));
										}
										if (pos.x < p.x)
										{
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + nn, pos.y - nn));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - n, pos.y + n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x + n, pos.y - n));
											entities.push_back(new Bullet(BulletSHGImage, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x - nn, pos.y + nn));
										}
									}
									moveTimer = 0;
								}
							}
							if (p.weapon == 3)
							{
								entities.push_back(new Bullet(BulletP90Image, "Bullet", lvl, p.x, p.y + 7, 16, 16, pos.x, pos.y));
							}

							p.shoot -= 1;
						}
						//shoot.play();//играем звук пули
					}
			}
		}

		int hpp = 10, ar = 10;

		for (it = entities.begin(); it != entities.end();)//говорим что проходимся от начала до конца
		{
			Entity* b = *it;//для удобства, чтобы не писать (*it)->
			b->update(time);//вызываем ф-цию update для всех объектов (по сути для тех, кто жив)

			if (b->life == false) {
				it = entities.erase(it);/*
															delete b;*/
			}
			else it++;//и идем курсором (итератором) к след объекту. так делаем со всеми объектами списка
		}

		for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		{
			


			if (((*it)->name == "MovingPlatform") && ((*it)->getRect().intersects(p.getRect())))//если игрок столкнулся с объектом списка и имя этого объекта movingplatform
			{
				Entity* movPlat = *it;
				if ((p.dy > 0) || (p.onGround == false))//при этом игрок находится в состоянии после прыжка, т.е падает вниз
					if (p.y + p.h < movPlat->y + movPlat->h)//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
					{
						p.y = movPlat->y - p.h + 3; p.x += movPlat->dx * time; p.dy = 0; p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
					}
			}
			if ((*it)->name == "EasyShootEnemy" && abs((*it)->x - p.x) < 300)
			{

				if ((*it)->name == "EasyShootEnemy" && abs((*it)->x) > abs(p.x) && (*it)->dx <= 0 || abs((*it)->x) < abs(p.x) && (*it)->dx >= 0)
				{
					moveTimerEnemy += time;
					if (moveTimerEnemy > 500)
					{
						entities.push_back(new Bullet(BulletEnemyImage, "BulletEnemy", lvl, (*it)->x, (*it)->y + 7, 16, 16, p.x, p.y - (80 - rand() % 150)));
						moveTimerEnemy = 0;
					}
				}
			}
			if (((*it)->name == "EasyEnemy" && ((*it)->getRect().intersects(p.getRect()))))
			{

				////////выталкивание врага
				if ((*it)->dx > 0)//если враг идет вправо
				{
					//(*it)->x = p.x - (*it)->w; //отталкиваем его от игрока влево (впритык)
					if (p.armour > 0)
					{
						p.armour -= 5;
						if (p.armour < ar * 10 )
						{
							ar--;
							xA -= 64;
							if (xA< 0)
							{
								xA= 0;
							}
						}

					}
					else
					{
						p.health -= 10;
						
							hpp--;
							xHP -= 64;
							if (xHP < 0)
							{
								xHP = 0;
							}
						
					}


					//(*it)->dx = 0;//останавливаем
				}
				if ((*it)->dx < 0)//если враг идет влево
				{
					//(*it)->x = p.x + p.w; ////налогично - отталкиваем вправо
					//(*it)->dx = 0;//останавливаем
					if (p.armour > 0)
					{
						p.armour -= 5;
						if (p.armour < ar * 10 )
						{
							ar--;
							xA -= 64;
							if (xA < 0)
							{
								xA = 0;
							}
						}
					}
					else
					{
						p.health -= 10;
						
							hpp--;
							xHP -= 64;
							if (xHP < 0)
							{
								xHP = 0;
							}
						
					}
				}
				///////выталкивание игрока
				if (p.dx < 0)
				{
					//p.x = (*it)->x + (*it)->w;
					//(*it)->health -=1 ;
				}//если столкнулись с врагом и игрок идет влево то выталкиваем игрока
				if (p.dx > 0)
				{
					//p.x = (*it)->x - p.w;
					//(*it)->health -= 1;
				}//если столкнулись с врагом и игрок идет вправо то выталкиваем игрока		
			}
			for (it2 = entities.begin(); it2 != entities.end(); it2++)
			{
				if ((*it)->getRect() != (*it2)->getRect())//при этом это должны быть разные прямоугольники
					if (((*it)->getRect().intersects((*it2)->getRect())) && ((*it)->name == "EasyEnemy") && ((*it2)->name == "Bullet"))//если столкнулись два объекта и они враги
					{
						float tmpX, tmpY;
						int randd;
						//cout << damage << endl;

						if (p.weapon == 2)
						{
							if (bought[0] == 0) { (*it)->health -= 20; }
							if (bought[0] == 1) { (*it)->health -= 30; }
							if (bought[0] == 2) { (*it)->health -= 35; }
							if (bought[0] >= 3) { (*it)->health -= 50; }
						}
						else
						{
							if (bought[0] == 0) { (*it)->health -= 10; }
							if (bought[0] == 1) { (*it)->health -= 15; }
							if (bought[0] == 2) { (*it)->health -= 20; }
							if (bought[0] >= 3) { (*it)->health -= 25; }

						}
						
						std::cout << bought[0] << " bought" << endl;
						std::cout << (*it)->health <<"EE" <<endl;
						if ((*it)->health <= 0)
						{
							tmpX = (*it)->x;
							tmpY = (*it)->y;
							randd=rand() % 4;
							if (randd >= 4) 
							{
								randd = rand() % 4;
							}
							if (randd==1)
							{
								entities.push_back(new Weapon(ShGImage, "ShootGun", lvl, tmpX, tmpY + 15, 64, 25));
							}
							if (randd == 2)
							{
								entities.push_back(new Weapon(AWPImage, "AWP", lvl, tmpX, tmpY + 15, 64, 25));
							}
							if (randd == 3)
							{
								entities.push_back(new Weapon(AGImage, "AutoGun", lvl, tmpX, tmpY + 15, 64, 25));
							}
							(*it)->life = false;
						}
						(*it2)->life = false;
						if ((*it)->life == false && (*it)->name == "EasyEnemy")
							p.playerScore += 1;

					}
				if (((*it)->getRect().intersects((*it2)->getRect())) && ((*it)->name == "EasyShootEnemy") && ((*it2)->name == "Bullet"))//если столкнулись два объекта и они враги
				{
					float tmpX, tmpY;
					int randd;
					//cout << damage << endl;
					if (p.weapon==2)
					{
						if (bought[0] == 0) { (*it)->health -= 20; }
						if (bought[0] == 1) { (*it)->health -= 30; }
						if (bought[0] == 2) { (*it)->health -= 35; }
						if (bought[0] >= 3) { (*it)->health -= 50; }
					}
					else 
					{
						if (bought[0] == 0) { (*it)->health -= 10; }
						if (bought[0] == 1) { (*it)->health -= 15; }
						if (bought[0] == 2) { (*it)->health -= 20; }
						if (bought[0] >= 3) { (*it)->health -= 25; }

					}
					std::cout << (*it)->health << "ESE" << endl;
					if ((*it)->health <= 0) 
					{
						tmpX = (*it)->x;
						tmpY = (*it)->y;
						randd = rand() % 4;
						if (randd >= 4)
						{
							randd = rand() % 4;
						}
						if (randd == 1)
						{
							entities.push_back(new Weapon(ShGImage, "ShootGun", lvl, tmpX, tmpY + 15, 64, 25));
						}
						if (randd == 2)
						{
							entities.push_back(new Weapon(AWPImage, "AWP", lvl, tmpX, tmpY + 15, 64, 25));
						}
						if (randd == 3)
						{
							entities.push_back(new Weapon(AGImage, "AutoGun", lvl, tmpX, tmpY + 15, 64, 25));
						}
						(*it)->life = false;
					}
						
						
					(*it2)->life = false;
					if ((*it)->life == false && (*it)->name == "EasyShootEnemy")
						p.playerScore += 2;
				}
				
				if (((*it)->getRect().intersects(p.getRect())) && ((*it)->name == "BulletEnemy"))
				{
					if (p.armour > 0)
					{
						p.health -= 10;
						p.armour -= 20;

						hpp--;
						xHP -= 64;
						if (xHP < 0)
						{
							xHP = 0;
						}
						ar-=2;
						xA -= 128;
						if (xA < 0)
						{
							xA = 0;
						}
					}
					else
					{
						p.health -= 20;

						hpp-=2;
						xHP -= 128;
						if (xHP < 0)
						{
							xHP = 0;
						}

					}
					(*it)->life = false;
					break;

				}

				
				if (((*it)->name == "Money1") && ((*it)->getRect().intersects(p.getRect())))
				{
					xSC += 64;
					if (xSC > 640)
					{
						xSC = 640;
					}
					p.playerScore += 1;
					(*it)->life = false;
					cout << p.playerScore << "Score" << endl;
					break;
				}
				if (((*it)->name == "Money2") && ((*it)->getRect().intersects(p.getRect())))
				{

					xSC += 128;
					if (xSC > 640)
					{
						xSC = 640;
					}
					p.playerScore += 2;
					(*it)->life = false;
					cout << p.playerScore << "Score" << endl;
					break;
				}
				if (((*it)->name == "Money3") && ((*it)->getRect().intersects(p.getRect())))
				{
					xSC += 192;
					if (xSC > 640)
					{
						xSC = 640;
					}
					p.playerScore += 3;
					(*it)->life = false;
					cout << p.playerScore << "Score" << endl;
					break;
				}
				if (((*it)->name == "Ammo") && ((*it)->getRect().intersects(p.getRect())))
				{
					p.shoot += 10;
					(*it)->life = false;
					break;
				}
				if (((*it)->name == "Heal") && ((*it)->getRect().intersects(p.getRect())))
				{
					p.health += 50;
					xHP += 5 * 64;
						//cout << xHP << endl;
					if (xHP > 640) 
					{
						xHP = 640;
					}
					if (p.health > 100) 
					{
						p.health = 100;
					}
					(*it)->life = false;
					break;
				}
				if (((*it)->name == "Armour") && ((*it)->getRect().intersects(p.getRect())))
				{
					p.armour += 50;
					xA += 5 * 64;
					if (xA > 640)
					{
						xA = 640;
					}
					if (p.armour > 100)
					{
						p.armour = 100;
					}
					(*it)->life = false;
					break;
				}
				if (((*it)->name == "AWP") && ((*it)->getRect().intersects(p.getRect())))
				{
					p.weapon = 2;
					(*it)->life = false;
				}
				if (((*it)->name == "ShootGun") && ((*it)->getRect().intersects(p.getRect())))
				{
					p.weapon = 1;
					(*it)->life = false;
				}
				if (((*it)->name == "AutoGun") && ((*it)->getRect().intersects(p.getRect())))
				{
					p.weapon = 3;
					(*it)->life = false;
				}


			}
			for (it2 = entities.begin(); it2 != entities.end(); it2++)
			{
				if ((*it)->getRect() != (*it2)->getRect())//при этом это должны быть разные прямоугольники
					if (((*it)->getRect().intersects((*it2)->getRect())) && ((*it)->name == "EasyEnemy") && ((*it2)->name == "EasyEnemy"))//если столкнулись два объекта и они враги
					{
						(*it)->dx *= -1;//меняем направление движения врага
						(*it)->sprite.scale(-1, 1);//отражаем спрайт по горизонтали
					}
				if (((*it)->getRect().intersects((*it2)->getRect())) && ((*it)->name == "EasyEnemy") && ((*it2)->name == "EasyShootEnemy"))//если столкнулись два объекта и они враги
				{
					(*it)->dx *= -1;//меняем направление движения врага
					(*it)->sprite.scale(-1, 1);//отражаем спрайт по горизонтали
				}
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::T)) { lvl.levelNumber++; return true; }
		if (Keyboard::isKeyPressed(Keyboard::Tab)) { return true; }//если таб, то перезагружаем игру
		if (Keyboard::isKeyPressed(Keyboard::F5)) 
		{
			set_pos_player(p,xHP,xA,xSC, bought[0], LvUpS, LvUpD);
		}

		if (Keyboard::isKeyPressed(Keyboard::F1))
		{
			view.reset(FloatRect(0, 0, 1920, 1080));
			window.setView(view);
			set_pos_player(p, xHP, xA, xSC, bought[0], LvUpS, LvUpD);
			Menu = 1;
			AdvMenu = 1;
			RaznMenu = 2;
			shop(window, nummer, p , bought[0],LvUpS,LvUpD);

			//menu(window);
			gamestart = true;
			
			return gamestart;
			
		}
		
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			view.reset(FloatRect(0, 0, 1920, 1080));
			window.setView(view);
			set_pos_player(p,xHP,xA,xSC, bought[0], LvUpS, LvUpD);
			//gamerun = 0;
			Menu = 1;
			gamestart = false;
			menu(window);
			return gamestart;
		}
		if (p.life==false) 
		{
			view.reset(FloatRect(0, 0, 1920, 1080));
			window.setView(view);
			//gamerun = 0;
			Menu = 1;
			gamestart = false;
			menu(window);
			return gamestart;
		}

		//если эскейп, то выходим из игры
		p.update(time);//перенесли сюда update игрока
		window.setView(view);
		window.clear(Color(66, 61, 91));
		lvl.Draw(window);
		

		if (gamerun == 1)
		{

			textureHPB.loadFromImage(HBImage);
			spriteHPB.setTexture(textureHPB);

			spriteHPB.setTextureRect(IntRect(xHP, 0, 64, 16));
			
			spriteHPB.setPosition(view.getCenter().x - 300, view.getCenter().y - 200);
			window.draw(spriteHPB);

			textureAB.loadFromImage(ABImage);
			spriteAB.setTexture(textureAB);

			spriteAB.setTextureRect(IntRect(xA, 0, 64, 16));
			
			spriteAB.setPosition(view.getCenter().x - 300, view.getCenter().y - 150);
			window.draw(spriteAB);

			textureSC.loadFromImage(SCImage);
			spriteSC.setTexture(textureSC);

			spriteSC.setTextureRect(IntRect(xSC, 0, 64, 16));

			spriteSC.setPosition(view.getCenter().x - 300, view.getCenter().y - 100);
			window.draw(spriteSC);

			/*
			shoot.setString("Ammo:" + to_string(int(p.shoot)));
			shoot.setPosition(view.getCenter().x - 300, view.getCenter().y - 50);
			window.draw(shoot);*/

		}
		else
		{
			return false;
			window.clear();
			window.close();
		}
		for (it = entities.begin(); it != entities.end(); it++) {
			window.draw((*it)->sprite);
		}
		//get_profile_balance(p.playerScore);
		window.draw(p.sprite);
		window.display();
	
	}
	//return false;
}

void gameRunning(RenderWindow& window, int& numberLevel,bool& gamestart) 
{//ф-ция перезагружает игру , если это необходимо
	
		if (startGame(window, numberLevel, gamestart))
		{

			
			gameRunning(window, numberLevel, gamestart);

		}//принимает с какого уровня начать игру
}

int main()
{
	int numberLevel = 1;//сначала 1-ый уровень
		if (gamerun == 1)
		{
			menu(window);
			gameRunning(window, level,gamestart);
		}
	window.close();
	return 0;
}