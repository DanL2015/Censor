#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <Windows.h>
using namespace std;
using namespace sf;

int windowx = 512;
int windowy = 512;
RenderWindow window(VideoMode(windowx, windowy), "Censor", Style::Close | Style::Titlebar);
RenderWindow subWindows[10000];
int subSize = 0;
bool isFocused = true;
bool buttonPressed = false;
bool moving[10000];
bool resizing[10000];
vector <int> direction[10000];
Vector2i grabbedOffset[10000];
Vector2i grabbedLocation[10000];
Vector2i grabbedScreenLocation[10000];
Vector2u windowSize[10000];

void secondaryThread()
{
	system("focus.bat");
}

void renderMain()
{
	RectangleShape background(Vector2f(512, 512));
	background.setPosition(Vector2f(0, 0));
	background.setFillColor(Color::White);
	window.draw(background);
	Texture logoTexture;
	logoTexture.loadFromFile("Textures/Logo.png");
	Sprite logo;
	logo.setTexture(logoTexture);
	logo.setPosition(Vector2f(0, 0));
	logo.setScale(Vector2f(2, 2));
	window.draw(logo);
}

int main()
{
	Thread thread2(secondaryThread);
	Texture pressedTexture;
	pressedTexture.loadFromFile("Textures/Button_1.png");
	Texture releasedTexture;
	releasedTexture.loadFromFile("Textures/Button_0.png");
	Sprite pressed;
	pressed.setTexture(pressedTexture);
	pressed.setOrigin(Vector2f(32, 16));
	pressed.setPosition(Vector2f(256, 256));
	Sprite released;
	released.setTexture(releasedTexture);
	released.setOrigin(Vector2f(32, 16));
	released.setPosition(Vector2f(256, 256));
	while (window.isOpen())
	{
		Event evnt;
		while (window.pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
			{
				ofstream fout("focus.bat");
				fout << "";
				cout << subSize << endl;
				for (int i = 0; i < subSize; i++)
				{
					string fpath = "focus_" + to_string(i) + ".ahk";
					string command = "del -f " + fpath;
					system(command.c_str());
				}
				system("taskkill /F /IM AutoHotkey.exe");
				window.close();
			}
			if (evnt.type == Event::MouseButtonPressed)
			{
				if (evnt.mouseButton.button == Mouse::Left)
				{
					Vector2i mousePos = Mouse::getPosition(window);
					if (mousePos.x >= 224 && mousePos.x <= 288 && mousePos.y >= 240 && mousePos.y <= 272)
					{
						buttonPressed = true;
						string title = to_string(subSize);
						subWindows[subSize].create(VideoMode(800, 600), title, Style::None);
						RectangleShape background;
						background.setFillColor(Color::Black);
						subWindows[subSize].draw(background);
						subWindows[subSize].display();
						subSize++;
						window.requestFocus();
						string fpathstring = "focus_" + title + ".ahk";
						const char* fpath = (fpathstring.c_str());
						ofstream fout(fpath);
						string fdata("Winset, Alwaysontop, toggle, " + title);
						fout << fdata << endl;
						string bpathstring = "focus.bat";
						const char* bpath = (bpathstring.c_str());
						ofstream bout(bpath);
						string bdata("\"C:\\Program Files\\AutoHotkey\\AutoHotkey.exe\" \"focus_" + title + ".ahk\"");
						bout << bdata << endl;
						thread2.terminate();
						thread2.launch();
					}
				}
			}
			if (buttonPressed && (evnt.type == Event::MouseButtonReleased || !window.hasFocus()))
			{
				buttonPressed = false;
			}
		}
		window.clear();
		renderMain();
		if (buttonPressed)
		{
			window.draw(pressed);
		}
		else
		{
			window.draw(released);
		}
		window.display();
		for (int i = 0; i < subSize; i++)
		{
			Event subevnt;
			subWindows[i].setActive(true);
			while (subWindows[i].pollEvent(subevnt))
			{
				if (subevnt.type == Event::Closed)
				{
					subWindows[i].close();
				}
				else if (subevnt.type == Event::KeyPressed)
				{
					if (subevnt.key.code == Keyboard::Escape)
					{
						subWindows[i].close();
					}
				}
				else if (subevnt.type == Event::MouseButtonPressed)
				{
					if (subevnt.mouseButton.button == Mouse::Left)
					{
						Vector2i mousePos = Mouse::getPosition(subWindows[i]);
						if (mousePos.x >= 10 && mousePos.x <= subWindows[i].getSize().x - 10 && mousePos.y >= 10 && mousePos.y <= subWindows[i].getSize().y - 10)
						{
							grabbedOffset[i] = subWindows[i].getPosition() - Mouse::getPosition();
							moving[i] = true;
						}
						else
						{
							grabbedScreenLocation[i] = Mouse::getPosition();
							grabbedLocation[i] = mousePos;
							windowSize[i] = subWindows[i].getSize();
							resizing[i] = true;
							Vector2u size = subWindows[i].getSize(); //size of window (not including bars)
							Vector2i position = subWindows[i].getPosition(); //position of window on screen
							if (grabbedLocation[i].x >= subWindows[i].getSize().x - 10)
							{
								direction[i].push_back(1);
							}
							if (grabbedLocation[i].x <= 10)
							{
								direction[i].push_back(2);
							}
							if (grabbedLocation[i].y >= subWindows[i].getSize().y - 10)
							{
								direction[i].push_back(3);
							}
							if (grabbedLocation[i].y <= 10)
							{
								direction[i].push_back(4);
							}
						}
					}
				}
				if (subevnt.type == Event::MouseButtonReleased)
				{
					if (subevnt.mouseButton.button == Mouse::Left)
					{
						moving[i] = false;
						resizing[i] = false;
						direction[i].clear();
					}
				}
				if (subevnt.type == Event::MouseMoved)
				{
					if (moving[i])
					{
						subWindows[i].setPosition(Mouse::getPosition() + grabbedOffset[i]);
					}
					if (resizing[i])
					{
						for (int j = 0; j < direction[i].size(); j++)
						{
							if (direction[i][j] == 1)
							{
								subWindows[i].setSize(Vector2u(Mouse::getPosition(subWindows[i]).x, subWindows[i].getSize().y));
							}
							else if (direction[i][j] == 2)
							{
								subWindows[i].setSize(Vector2u(windowSize[i].x - (Mouse::getPosition().x- grabbedScreenLocation[i].x), subWindows[i].getSize().y));
								subWindows[i].setPosition(Vector2i(Mouse::getPosition().x, subWindows[i].getPosition().y));
							}
							else if (direction[i][j] == 3)
							{
								subWindows[i].setSize(Vector2u(subWindows[i].getSize().x, Mouse::getPosition(subWindows[i]).y));
							}
							else if (direction[i][j] == 4)
							{
								subWindows[i].setSize(Vector2u(subWindows[i].getSize().x, windowSize[i].y - (Mouse::getPosition().y - grabbedScreenLocation[i].y)));
								subWindows[i].setPosition(Vector2i(subWindows[i].getPosition().x, Mouse::getPosition().y));
							}
						}
					}
				}
			}
			RectangleShape background;
			background.setFillColor(Color::Black);
			background.setSize(Vector2f(subWindows[i].getSize().x, subWindows[i].getSize().y));
			subWindows[i].draw(background);
			subWindows[i].display();
		}
	}
}
