#include "pch.h"
#include <iostream>
#include <fstream>
#include<windows.h> 
#include <conio.h>
#include <string>

using namespace std;

int idGO;
int cellPositions[10][10] = {
	{0,0,1,1,1,1,1,1,1,1},
	{1,0,1,1,0,0,0,0,3,1},
	{1,0,0,1,1,0,0,0,0,1},
	{1,0,2,0,0,0,0,0,0,1},
	{1,0,0,0,0,1,0,2,0,1},
	{1,1,1,1,2,1,0,0,0,1},
	{1,1,1,0,0,1,1,0,0,1},
	{1,0,1,0,1,1,1,0,0,1},
	{1,0,0,0,1,1,1,0,0,1},
	{1,1,1,1,1,1,1,1,0,0}
};
CONSOLE_SCREEN_BUFFER_INFO biTemp;
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hwnd = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_CURSOR_INFO     cursorInfo;

static class PrepareConsoleForGame {
	
public:
	enum ColorOfSymb
	{
		Black,
		Blue,
		Green,
		Cyan,
		Red,
		Magenta,
		Brown,
		LightGray,
		DarkGray,
		LightBlue,
		LightGreen,
		LightCyan,
		LightRed,
		LightMagenta,
		Yellow,
		White,
	};
	static	void ChangeColorSymb(ColorOfSymb ForgC) {

		WORD wColor;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hStdOut, &csbi);
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	static void ShowConsoleCursor(bool showFlag)
	{

		GetConsoleCursorInfo(out, &cursorInfo);
		cursorInfo.bVisible = showFlag; // set the cursor visibility
		SetConsoleCursorInfo(out, &cursorInfo);
	}

};

void readFile(string textfile) {
	ofstream Myfstream;
	Myfstream.open(textfile);
	Myfstream << "test";
	Myfstream.close();
}
int counter = 0;

struct Vector2
{
	int x, y;
	Vector2(int sX, int sY)
	{
		x = sX; y = sY;
	}
	Vector2()
	{
		x = 0; y = 0;
	}

	friend Vector2 operator + (Vector2 v1, Vector2 v2) {
		return Vector2(v1.x + v2.x, v1.y + v2.y);
	}

	friend	Vector2 operator += (Vector2 v1, const Vector2 v2)
	{
		return Vector2(v1.x = v1.x + v2.x, v1.y = v1.y + v2.y);
	}
	friend	Vector2 operator + (const Vector2 v)
	{
		return v;
	}
	Vector2 operator - (const Vector2 v)
	{
		return Vector2(v.x * -1, v.y * -1);
	}
	Vector2 operator / (const int v)
	{
		return Vector2(this->x = this->x / v, this->y = this->y / v);
	}
	friend const bool operator == (const Vector2 v1, const Vector2 v2)
	{
		if ((v1.x == v2.x) && (v1.y == v2.y))
			return true;
		return false;
	}
	friend const bool operator != (const Vector2 v1, const Vector2 v2)
	{
		return !(v1 == v2);
	}


	//

	/*Vector2 operator=(Vector2& v1)
	{

		return Vector2(this->x = v1.x, this->y = v1.y);
	}*/

};
class GameObject
{	
public:Vector2 _position;
string name = "GO" + idGO;
	
	PrepareConsoleForGame::ColorOfSymb clrGO;
	char CellSymbol = char(219);
	//virtual  Vector2 position() const { return _position; }
	//	virtual   void position(Vector2 value) { _position = value; }
		GameObject() {
			_position = Vector2(0, 0);
			clrGO = PrepareConsoleForGame::Black;
			idGO++;
		}
		GameObject(Vector2 pos) {
			_position = pos;
			clrGO = PrepareConsoleForGame::White;
			idGO++;
		}
		~GameObject() {
			cout << "\n deleted " + name + "\n";
		}
		
	virtual void Draw(Vector2 pos, PrepareConsoleForGame::ColorOfSymb  col) 
{

	_position = pos;
	clrGO = col;
	PrepareConsoleForGame::ChangeColorSymb(col);
	COORD b;	
	b.X = _position.x;
	b.Y = _position.y;
	SetConsoleCursorPosition(hStdOut, b);
	cout << char(219) << endl;
}
		virtual	 bool OnColliderEnter(int& u) {

			Draw(_position, PrepareConsoleForGame::Black);
			return true;
		}
};

class Wall :public GameObject {

public:

	Wall(Vector2 position) {
		name = "wall" + idGO; idGO++;
		_position = position;
		Draw(position, PrepareConsoleForGame::LightBlue);
	}
	Wall(Vector2 position, int k) {
		name = "wall" + idGO;
		_position = position; idGO++;
	}
	~Wall() {
		cout << "\n deleted " + name + "\n";
	}

	bool OnColliderEnter(int& u) override {
		Draw(_position, PrepareConsoleForGame::Blue);
		return false;
	}
};


class Item :public GameObject {
public:
	bool isCollectable = true;
public:
	Item(Vector2 v) {
		name = "item" + idGO; idGO++;
		_position = v;
	}
	~Item() {
		cout << "\n deleted " + name + "\n";
	}
	bool OnColliderEnter(int& u) override {
		OnCollect(u);
		return true;
	}
	virtual	void OnCollect(int& u) {
		if (isCollectable) {
			Draw(_position, PrepareConsoleForGame::Black);
			u++;
			isCollectable = false;
			cellPositions[_position.x][_position.y] = 0;
		}
	}
	};

class Character :public GameObject {
public:
	Character() {  }
	virtual void Attack(int& o) = 0;

};

class Monster :public Character
{
public:
	int ft=0;
	Monster(Vector2 mv);
	~Monster();
	bool OnColliderEnter(int& q) override{
		Attack(q);
		return true;

	}
	void Attack(int& o) {
		o--;
	}
	void Move() {
		int rx;
		int ry =_position.y;
		switch (ft)
		{
		case 0:
			rx = _position.x + 1; ft++;
			break;
		case 1:
			rx = _position.x - 1; ft = 0;
			break;
		default:
			break;
		}
		/*int rx = 1 + rand() % 7;
		int ry = 1 + rand() % 7;*/
		if ((cellPositions[rx][ry] == 0) || (cellPositions[rx][ry] == 3))
		{
			Draw(_position, PrepareConsoleForGame::Black);

			Vector2	randVect = Vector2(rx, ry);
			Draw(randVect, PrepareConsoleForGame::LightMagenta);
		}
	}
};

Monster::Monster(Vector2 mv)
{
	name = "monster" + idGO; idGO++;
	_position = mv;
	Monster::Draw(_position, PrepareConsoleForGame::LightRed);
	Monster::Move();
}
Monster::~Monster()
{
	cout << "\n deleted " + name + "\n";
}

class Player :public GameObject {
public:
	int score = 0;
	   Player() {
		   name = "player" + idGO;
		   idGO++;
		   _position = Vector2(0, 0);
		   Draw(_position, PrepareConsoleForGame::White);
	   }
	   ~Player() {
		   cout << "\n deleted " + name + "\n";
	   }
	   /*virtual  Vector2 position()  const override { return _position; }

	   void position(Vector2 value) { _position = value; }*/

	   virtual void Draw(Vector2 pos, PrepareConsoleForGame::ColorOfSymb col)override {

		   _position = pos;
		   clrGO = col;
		   PrepareConsoleForGame::ChangeColorSymb(col);
		   COORD b;


		   b.X = _position.x;
		   b.Y = _position.y;
		   SetConsoleCursorPosition(hStdOut, b);

		   cout << '+' << endl;

	   }
};

//replace this to GameManager::IManager (mb singleton)
void SendGameMessage(int r, int yL, string who) 
{
	PrepareConsoleForGame::ChangeColorSymb(PrepareConsoleForGame::White);
	COORD b;
	b.X = 10;
	b.Y = yL;
	SetConsoleCursorPosition(hStdOut, b);
	cout << "game\t " + who + " score:" << r;
}

Player player = Player();
Player enemy = Player();
int numberOfItem = 0;
GameObject * go[10][10];

class GameManager
{
public:
	static GameManager& Instance()
	{
		// согласно стандарту, этот код ленивый и потокобезопасный
		static GameManager s;
		return s;
	}

private:
	GameManager() { 
		cout << "\n GameManager created\n";
		Playing();
	}  
	~GameManager() {
		cout << "\nGameManager deleted\n";

	} 

	// необходимо запретить копирование
	GameManager(GameManager const&); // реализация не нужна
	GameManager& operator = (GameManager const&);  // и тут
	
	void OpenCells(Vector2 v2) {
		for (int x = v2.x - 2; x < v2.x + 3; x++) {
			for (int y = v2.y - 2; y < v2.y + 3; y++) {
				if (((x) < 10) && ((y) < 10) && ((x) >= 0) && ((y) >= 0)) {
					if (cellPositions[x][y] == 1) {
						go[x][y]->Draw(Vector2(x, y), PrepareConsoleForGame::LightBlue);
					}
					else if (cellPositions[x][y] == 2)
					{
						go[x][y]->Draw(Vector2(x, y), PrepareConsoleForGame::Green);
					}
					else if (cellPositions[x][y] == 3)
					{
						go[x][y]->Draw(Vector2(x, y), PrepareConsoleForGame::Magenta);
					}

				}
			}
		}
	}
	void Update(int r) {
		Vector2 playerCoords = Vector2(0, 0);

		Monster m = Monster(Vector2(2, 6));
		Vector2 randVect;// = Vector2(2, 2);
		Vector2 v1, v2;
		v1.x = 1;
		v2.x = 0;
		v1.y = 1;
		v2.y = 0;
		v2 = player._position;
		
		playerCoords = v2;
		int	button1 = 0;

		while (button1 != 27)
		{

			button1 = _getch();
			cout << "\b";
			int rx = 1 + rand() % 7;
			int ry = 1 + rand() % 7;
			randVect = Vector2(rx, ry);
			m.Move();
			switch (_getch()) {
			case 27:
				cout << endl << "End" << endl;
				break;
			case 72:

				if ((v2.x >= 0)) {
					if (go[v2.x][v2.y - 1]->OnColliderEnter(player.score)) {
						player.Draw(v2, PrepareConsoleForGame::Black);
						/*rand += enemy.position();
						enemy.Draw(rand,blue);*/
						v2.y -= 1;
					}
				}
				else
				{
					cout << "\a";
				}

				break;
			case 80:

				if ((v2.y + 1 < 10)) {
					if (go[v2.x][v2.y + 1]->OnColliderEnter(player.score)) {
						player.Draw(v2, PrepareConsoleForGame::Black);
						v2.y += 1;
					}
				}
				else
				{
					cout << "\a";
				}

				break;
			case 77:

				if ((v2.x + 1 < 10)) {
					if (go[v2.x + 1][v2.y]->OnColliderEnter(player.score)) {
						player.Draw(v2, PrepareConsoleForGame::Black);
						v2.x += 1;
					}
				}
				else
				{
					cout << "\a";
				}

				break;
			case 75:

				if ((v2.x - 1 >= 0)) {
					if (go[v2.x - 1][v2.y]->OnColliderEnter(player.score)) {
						player.Draw(v2, PrepareConsoleForGame::Black);
						v2.x -= 1;
					}
				}
				else
				{
					cout << "\a";
				}

				break;
			}
			
			OpenCells(v2);
			player._position=v2;
			player.Draw(v2, PrepareConsoleForGame::White);


			if (cellPositions[randVect.x][randVect.y] == 0)
			{
				enemy.Draw(enemy._position, PrepareConsoleForGame::Black);
				enemy.Draw(randVect, PrepareConsoleForGame::LightRed);
			}
			else {
				go[randVect.x][randVect.y]->OnColliderEnter(enemy.score);
			}

			SendGameMessage(player.score, 13, "player"	);
			SendGameMessage(enemy.score, 15, "enemy");
			/*	if (player.score == numberOfItem) {
					if (player.position() == Vector2(9, 9)) {

						cout << endl << "End" << endl;
						break;
					}
				}*/
			
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &biTemp);
			COORD temp; temp.X = 1; temp.Y = 2;
			SetConsoleCursorPosition(hStdOut, temp);

			if (player._position == Vector2(9, 9)) {

				cout << endl << "End" << endl;
				break;
			}
		}
		system("cls");
		cout << "\n\ngame over\n\n\a";
	}
	void InstantiateAllElements() {
		cout << "\nstarted\n";
		int length = sizeof(cellPositions) / sizeof(cellPositions[0]);

		for (int x = 0; x < length; x++) {
			for (int y = 0; y < length; y++) {
				if (cellPositions[x][y] == 1) {
					go[x][y] = new Wall(Vector2(x, y), 1);
				}
				else if (cellPositions[x][y] == 2)
				{
					go[x][y] = new Item(Vector2(x, y));
					numberOfItem++;
				}
				else if (cellPositions[x][y] == 3) {
					go[x][y] = new Monster(Vector2(x, y));
				}
				else
				{
					go[x][y] = new GameObject(Vector2(x, y));
				}
			}
		}
		
		system("pause");
		system("cls");
	}
	
	void Playing() {

		PrepareConsoleForGame::ShowConsoleCursor(false);
		InstantiateAllElements();
		Update(1);
	}
};
//


int main()
{
	GameManager& instance = GameManager::Instance();	
}
