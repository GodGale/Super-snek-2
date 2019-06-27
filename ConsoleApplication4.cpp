// ConsoleApplication4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <stdlib.h>
#include <direct.h> //windows file handling.
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <list>
#include <thread>
using namespace std;

 /*SSSSSSSSSSSSSSSNNNNNNNN        NNNNNNNEEEEEEEEEEEEEEEEEEEEEKKKKKKKKK    KKKKKKK
 SS:::::::::::::::N:::::::N       N::::::E::::::::::::::::::::K:::::::K    K:::::K
S:::::SSSSSS::::::N::::::::N      N::::::E::::::::::::::::::::K:::::::K    K:::::K
S:::::S     SSSSSSN:::::::::N     N::::::EE::::::EEEEEEEEE::::K:::::::K   K::::::K
S:::::S           N::::::::::N    N::::::N E:::::E       EEEEEKK::::::K  K:::::KKK
S:::::S           N:::::::::::N   N::::::N E:::::E              K:::::K K:::::K
 S::::SSSS        N:::::::N::::N  N::::::N E::::::EEEEEEEEEE    K::::::K:::::K
  SS::::::SSSSS   N::::::N N::::N N::::::N E:::::::::::::::E    K:::::::::::K
	SSS::::::::SS N::::::N  N::::N:::::::N E:::::::::::::::E    K:::::::::::K
	   SSSSSS::::SN::::::N   N:::::::::::N E::::::EEEEEEEEEE    K::::::K:::::K
            S:::::N::::::N    N::::::::::N E:::::E              K:::::K K:::::K
            S:::::N::::::N     N:::::::::N E:::::E       EEEEEKK::::::K  K:::::KKK
SSSSSSS     S:::::N::::::N      N::::::::EE::::::EEEEEEEE:::::K:::::::K   K::::::K
S::::::SSSSSS:::::N::::::N       N:::::::E::::::::::::::::::::K:::::::K    K:::::K
S:::::::::::::::SSN::::::N        N::::::E::::::::::::::::::::K:::::::K    K:::::K
 SSSSSSSSSSSSSSS  NNNNNNNN         NNNNNNEEEEEEEEEEEEEEEEEEEEEKKKKKKKKK    KKKKKK*/

int nScreenWidth = 120;
int nScreenHeight = 40;
struct sSnakeSegment //a struct of individual segments.
{
	int x;
	int	y;
};

int main() {
	//screen buffer
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];//This line is telling the program to create a new memory space, which is a linear char wchar_t array, nWidth * nHeight in size, AND assign a pointer called screen to it.
	//in this context, *screen is the cursor position on this long string. You move *screen "cursor" to the desired position, you put a character on that position, and move on to the next. once all the desired characters have been submited, the whole
	// string is passed on to the render pipeline, which prints it on the console, giving the impression of a screen. then the whole process starts again for the next frame.
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';//for the whole screen, draw empty spaces. Initialization.
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);//this is the actual pipeline to the screen; it gets the character stream sumbited by WriteConsoleOutputCharacter() and passes onto
	//the console. it needs generic read / write as permissions in order to be able to do so. 
	//WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0,0 }, &dwBytesWritten); <- WriteConsoleOutputCharacter (destination, source, length, start pos, size printed).


	SetConsoleActiveScreenBuffer(hConsole); //sets this buffer as the one currently outputting text to the console. you can have many buffers, but only one can be active.
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 1;
	


	DWORD dwBytesWritten = 0;
	int difficulty = 0;
	int nStepsRequiredX = 6;
	int nStepsRequiredY = 10;
	int uiScreenFrameReference = 0;
	bool bIntro = false;
	bool bSetup = true;
	bool bIntroFinished = false;
	while (1) {

		//screen setup
		while (bSetup) {
			for (int i = 0; i < nScreenWidth; i++) //draw the borders above and below. 
			{
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
				//starts at 2 times the width of the screen +iterator(presumably 0)? does this mean that the screen buffer is actually a linear sweep from the top left corner, going
				//down one line at a time?
				screen[39 * nScreenWidth + i] = L'=';//the window doesn't auto-size to the correct size, there's no way of knowing where the bottom is without a border.
			}
			for (int i = 3 * nScreenWidth; i < (nScreenWidth*(nScreenHeight - 1)); i += nScreenWidth) //draw the fences, left and right. Start on the 4th row, finish one row before the bottom.
			{
				screen[i] = L'X';
				screen[i + nScreenWidth - 1] = L'X';
			}
			wsprintf(&screen[((nScreenHeight*nScreenWidth) / 2) + 19], L"Please expand the console window until the bottom of the playable area is visible.");
			wsprintf(&screen[nScreenWidth * 22 + (nScreenWidth / 2) - 12], L"Press SPACE to continue.");
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0,0 }, &dwBytesWritten);
			if ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) != 0) {
				while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) != 0);
				bSetup = false;
			}
		}
		//INTRO & SPLASH SCREEN
		bIntro = true;
		for (int i = 0; i < (nScreenWidth*nScreenHeight); i++) //clear the screen. Instant, no frames.
		{
			screen[i] = L' ';
		}
		while (bIntro && !bIntroFinished) {
			uiScreenFrameReference++;
			auto t0 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t0) < 0.025ms);
			if (uiScreenFrameReference > 500) {
				for (int i = nScreenWidth * nScreenHeight; (i > ((nScreenWidth * nScreenHeight) - (uiScreenFrameReference-500)) &&  i >= 0); i--) screen[i] = L'/';//the first half of the condition is used to set a backwards sliding stop, that shifts with every frame. 
				//iterate for every symbol cell. //do if the iterator is at a position on the screen that is higher than the current frame, adjusted for the moment where it starts (500 frames after frame 0).																											//Without it, you're just painting the screen in one go.
			}	//using a loop to cover for potential synch problems.
			if (uiScreenFrameReference > 1000) {
				for (int i = nScreenWidth * nScreenHeight; (i > ((nScreenWidth * nScreenHeight) - (uiScreenFrameReference-1000)) && i >= 0); i--) screen[i] = L'|';
			}
			if (uiScreenFrameReference > 1500) {
				for (int i = nScreenWidth * nScreenHeight; (i > ((nScreenWidth * nScreenHeight) - (uiScreenFrameReference-1500)) && i >= 0); i--) screen[i] = L'/';
			}
			if (uiScreenFrameReference > 2000) {
				for (int i = nScreenWidth * nScreenHeight; (i > ((nScreenWidth * nScreenHeight) - (uiScreenFrameReference-2000)) && i >= 0); i--) screen[i] = L' ';
			}
			if (uiScreenFrameReference > 6000) {
				int uiSnekOffset = 27 - ((uiScreenFrameReference - 6000)/20);
				if (uiSnekOffset < 0) uiSnekOffset = 0;
				wsprintf(&screen[(nScreenWidth * (14+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(       __________________________________________________________________          )delimiter");
				wsprintf(&screen[(nScreenWidth * (15+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter( _____/\\\\\\\\\\\____/\\\\\_____/\\\__/\\\\\\\\\\\\\\\__/\\\________/\\\_         )delimiter");
				wsprintf(&screen[(nScreenWidth * (16+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(  ___/\\\/////////\\\_\/\\\\\\___\/\\\_\/\\\///////////__\/\\\_____/\\\//__        )delimiter");
				wsprintf(&screen[(nScreenWidth * (17+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(   __\//\\\______\///__\/\\\/\\\__\/\\\_\/\\\_____________\/\\\__/\\\//_____       )delimiter");
				wsprintf(&screen[(nScreenWidth * (18+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(    ___\////\\\_________\/\\\//\\\_\/\\\_\/\\\\\\\\\\\_____\/\\\\\\//\\\_____      )delimiter");
				wsprintf(&screen[(nScreenWidth * (19+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(     ______\////\\\______\/\\\\//\\\\/\\\_\/\\\///////______\/\\\//_\//\\\____     )delimiter");
				wsprintf(&screen[(nScreenWidth * (20+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(      _________\////\\\___\/\\\_\//\\\/\\\_\/\\\_____________\/\\\____\//\\\___    )delimiter");
				wsprintf(&screen[(nScreenWidth * (21+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(       __/\\\______\//\\\__\/\\\__\//\\\\\\_\/\\\_____________\/\\\_____\//\\\__   )delimiter");
				wsprintf(&screen[(nScreenWidth * (22+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(        _\///\\\\\\\\\\\/___\/\\\___\//\\\\\_\/\\\\\\\\\\\\\\\_\/\\\______\//\\\_  )delimiter");
				wsprintf(&screen[(nScreenWidth * (23+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(         ___\///////////_____\///_____\/////__\///////////////__\///________\///__ )delimiter");
				wsprintf(&screen[(nScreenWidth * (24+uiSnekOffset)) + nScreenWidth / 2 - 42], LR"delimiter(          __________________________________________________________________/      )delimiter");
			}
			if (uiScreenFrameReference > 7200) { //it works. It's trash but it works.
				int uiSuperOffset = 24 - ((uiScreenFrameReference - 7200) / 20);
				if (uiSuperOffset < 0) uiSuperOffset = 0;

				auto row1 = LR"delimiter(   ____                 )delimiter";
				auto row2 = LR"delimiter(  / __/_ _____  ___ ____)delimiter";
				auto row3 = LR"delimiter( _\ \/ // / _ \/ -_) __/)delimiter";
				auto row4 = LR"delimiter(/___/\_,_/ .__/\__/_/)delimiter";
				auto row5 = LR"delimiter(        /_/)delimiter";
				wsprintf(&screen[(nScreenWidth * 11) + 3 - uiSuperOffset], row1);
				wsprintf(&screen[(nScreenWidth * 12) + 3 - uiSuperOffset], row2);
				wsprintf(&screen[(nScreenWidth * 13) + 3 - uiSuperOffset], row3);
				wsprintf(&screen[(nScreenWidth * 14) + 3 - uiSuperOffset], row4);
				wsprintf(&screen[(nScreenWidth * 15) + 3 - uiSuperOffset], row5);

				wsprintf(&screen[(nScreenWidth * (10)) + nScreenWidth - 24], LR"delimiter(                        )delimiter"); //absolutely shit code, saving the day.
				wsprintf(&screen[(nScreenWidth * (11)) + nScreenWidth - 24], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (12)) + nScreenWidth - 24], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (13)) + nScreenWidth - 24], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (14)) + nScreenWidth - 24], LR"delimiter(                        )delimiter");

				wsprintf(&screen[(nScreenWidth * (24)) + nScreenWidth - 25 + uiSuperOffset], LR"delimiter(.oooo.   )delimiter");
				wsprintf(&screen[(nScreenWidth * (25)) + nScreenWidth - 28 + uiSuperOffset], LR"delimiter( .dP""Y88b  )delimiter");
				wsprintf(&screen[(nScreenWidth * (26)) + nScreenWidth - 28 + uiSuperOffset], LR"delimiter(       ]8P' )delimiter");
				wsprintf(&screen[(nScreenWidth * (27)) + nScreenWidth - 28 + uiSuperOffset], LR"delimiter(    .d8P'   )delimiter");
				wsprintf(&screen[(nScreenWidth * (28)) + nScreenWidth - 28 + uiSuperOffset], LR"delimiter(   .dP'     )delimiter");
				wsprintf(&screen[(nScreenWidth * (29)) + nScreenWidth - 28 + uiSuperOffset], LR"delimiter( .oP'    .o )delimiter");
				wsprintf(&screen[(nScreenWidth * (30)) + nScreenWidth - 28 + uiSuperOffset], LR"delimiter( 8888888888 )delimiter");

				wsprintf(&screen[(nScreenWidth * (25)) ], LR"delimiter(                        )delimiter"); //absolutely shit code, the sequel.
				wsprintf(&screen[(nScreenWidth * (26)) ], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (27)) ], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (28)) ], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (29)) ], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (30))], LR"delimiter(                        )delimiter");
				wsprintf(&screen[(nScreenWidth * (31))], LR"delimiter(                        )delimiter");

			}
			if (uiScreenFrameReference > 8700) wsprintf(&screen[nScreenWidth * 32 + ((nScreenWidth - 22) / 2)], L"PRESS [SPACE] TO START"); //screenwidth minus string length, divided in half.
			if (((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) != 0) && uiScreenFrameReference>750) bIntro = false; //skip to game but prevent accidental skips.
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0,0 }, &dwBytesWritten);
		}
		bIntroFinished = true;





		list <sSnakeSegment> snake = { //all the segments, each an instance of sSnakeSegment, as seen above. Snake facing left, see below.
		{60, 15}, //snake.front().x and snake.front().y
		{61, 15},
		{62, 15},
		{63, 15},
		{64, 15},
		{65, 15},
		{66, 15},
		{67, 15},
		{68, 15},
		{69, 15} //snake.back().x and snake.back().y
		};
		
		int nBulletTime = 1000;
		bool bBulletTime = false;
		int nMoveStep = 0;
		int nFoodStep = 0;
		int nItemStep = 7500;
		int nItemLifespan = 2000;
		bool bItemSpawned = false;
		bool bFoodEvActive = false;
		bool bItemEvActive = false;
		int nFoodX = 30;
		int nFoodY = 15;
		int nItemX = 0;
		int nItemY = 0;
		int nScore = 0;
		int uiMenuCursor = 0;
		int uiFoodPickupEv[3] = { -100, NULL, NULL }; //time-stamp, x, y.
		int uiItemPickupEv[4] = { -100, NULL, NULL, 0 }; //time-stamp, x, y, a/b switch for bonus/power up
		int nSnakeDirection = 3; //facing left
		bool bDead = false, bPaused = false, bKeyPaused = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false, bKeyUp = false, bKeyDown = false;
		
		while (!bDead) {
			if (((0x8000 & GetAsyncKeyState((unsigned char)('\x1B'))) != 0) && !bKeyPaused && !bPaused) {
				bKeyPaused = true;
				uiMenuCursor = 0;
				bPaused = true;
				while ((0x8000 & GetAsyncKeyState((unsigned char)('\x1B'))) != 0);
				bKeyPaused = false;
			}
			if (((0x8000 & GetAsyncKeyState((unsigned char)('\x1B'))) != 0) && !bKeyPaused && bPaused) {
				bKeyPaused = true;
				bPaused = false;
				while ((0x8000 & GetAsyncKeyState((unsigned char)('\x1B'))) != 0);
				bKeyPaused = false;
			}

			nItemStep++;
			nMoveStep++;
			nFoodStep++; //add one to the food step.
			uiScreenFrameReference++;

			/*   /////// //  //       //  ///////
				   //   //  ///     ///  //
				  //   //  ////   ////  ////
				 //   //  // // // //  //
				//   //  //  ///  //  ///////  */
			auto t1 = chrono::system_clock::now(); //this time-stamp is set every time the game loops the main body of code.
			while ((chrono::system_clock::now() - t1) < 10ms) { //finally, fixed refresh rate. moved aspect ratio compensation to the difficulty and step logic.
			//this loop stays open until 10 ms have passed, after which, the rest of the code gets executed. literally, while now-t1 < 10, do inputs, then move further.
				if ((0x8000 & GetAsyncKeyState((unsigned char)('\x10'))) != 0) { //press shift to go faster
					nMoveStep++;
				}
				if (((0x8000 & GetAsyncKeyState((unsigned char)('\x11'))) != 0) && !bBulletTime && nBulletTime >= 1000) { //press ctrl to use slow-mo power-up
					bBulletTime = true;
				}
				if (bBulletTime && nBulletTime <= 0) bBulletTime = false; //power up is on but gauge has run empty, turn it off.



				//get input
				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
				if (!bPaused) {

					if (bKeyRight && !bKeyRightOld) {
						nSnakeDirection++; //pressing right rotates the direction clockwise. valid directions == 0, 1, 2, 3. up, right, down, left.
						if (nSnakeDirection == 4) nSnakeDirection = 0;//higher than 3, convert to 0.
					}

					if (bKeyLeft && !bKeyLeftOld)
					{
						nSnakeDirection--;
						if (nSnakeDirection == -1) nSnakeDirection = 3;//lower than 0, convert to 3.
					}

					bKeyRightOld = bKeyRight;
					bKeyLeftOld = bKeyLeft;
				}
			}
			//---------- GAME LOGIC ------------
			// update the position and put the head at the new position.


			if (!bPaused) {

				if (difficulty == 0) { //noob difficulty
					nStepsRequiredX = 12;
					nStepsRequiredY = 20;
				}
				else if (difficulty == 1) { // easy
					nStepsRequiredX = 9;
					nStepsRequiredY = 15;
				}
				else if (difficulty == 2) { //normal
					nStepsRequiredX = 6;
					nStepsRequiredY = 10;
				}
				else if (difficulty == 3) { //hard
					nStepsRequiredX = 3;
					nStepsRequiredY = 5;
				}
				if (bBulletTime) {
					nStepsRequiredX *= 2;
					nStepsRequiredY *= 2;
				}

				switch (nSnakeDirection) {
				case 0: //up
					if (nMoveStep >= nStepsRequiredY) {
						snake.push_front({ snake.front().x, snake.front().y - 1 }); //insert an element at the top(front) of the list that is at the same X, but one line higher than the
						if (bBulletTime) {
							nBulletTime -= 10;
						}
						else {
							nBulletTime++;
						}
						snake.pop_back(); //delete the last element on the snake list. pop_back() is the opposite of push_back(). pop_front() is the opposite of push_front(). Let's see if it works here.
						nMoveStep = 0;
					}
					break;
				case 1: //right
					if (nMoveStep >= nStepsRequiredX) {
						snake.push_front({ snake.front().x + 1, snake.front().y });
						if (bBulletTime) {
							nBulletTime -= 10;
						}
						else {
							nBulletTime++;
						}
						snake.pop_back(); //delete the last element on the snake list. pop_back() is the opposite of push_back(). pop_front() is the opposite of push_front(). Let's see if it works here.
						nMoveStep = 0;
					}
					break;
				case 2: //down
					if (nMoveStep >= nStepsRequiredY) {
						snake.push_front({ snake.front().x, snake.front().y + 1 });
						if (bBulletTime) {
							nBulletTime -= 10;
						}
						else {
							nBulletTime++;
						}

						snake.pop_back(); //delete the last element on the snake list. pop_back() is the opposite of push_back(). pop_front() is the opposite of push_front(). Let's see if it works here.
						nMoveStep = 0;
					}
					break;
				case 3: //left
					if (nMoveStep >= nStepsRequiredY) {
						snake.push_front({ snake.front().x - 1 , snake.front().y });
						if (bBulletTime) {
							nBulletTime -= 10;
						}
						else {
							nBulletTime++;
						}
						snake.pop_back(); //delete the last element on the snake list. pop_back() is the opposite of push_back(). pop_front() is the opposite of push_front(). Let's see if it works here.
						nMoveStep = 0;
					}
					break;
				}
				if (nBulletTime > 1000) nBulletTime = 1000;
			}

			//collision with power-up, spawning power up, de-spawning power-up
			if (snake.front().x == nItemX && snake.front().y == nItemY) {
				uiItemPickupEv[0] = (uiScreenFrameReference >= 975) ? uiScreenFrameReference -1000 : uiScreenFrameReference; //beyond frame ref 975, the event would end at 1000 or higher, but that's where the ref loop resets, meaning the even would never end. 
				uiItemPickupEv[1] = nItemX;
				uiItemPickupEv[2] = nItemY;
				bItemEvActive = true;
				//trigger the event in the rendering area.
				if (nBulletTime >= 100) {
					nScore += 10;
					uiItemPickupEv[3] = 1; //print BONUS!
				}
				else {
					nBulletTime = 1000;
					nScore++;
					uiItemPickupEv[3] = 0; //print Power-Up!
				}
				nItemX = NULL;
				nItemY = NULL;
				bItemSpawned = false;
			}
			if (nItemStep > 9000) {
				bItemSpawned = true;
				nItemLifespan = 2000; //stays on screen 20 seconds
				nItemX = (rand() % (nScreenWidth - 2) + 1);
				nItemY = (rand() % (nScreenHeight - 4)) + 3;
				nItemStep = 0; //reset the timer.
			}
			if (bItemSpawned) {
				if (nItemLifespan > 0) {
					nItemLifespan--; //ticks life away
				}
				else {
					bItemSpawned = false;
					nItemX = NULL;
					nItemY = NULL;
				}
			}

			//collision with food, spawning food, add random food drifting.
			if (snake.front().x == nFoodX && snake.front().y == nFoodY) {
				nScore++;
				uiFoodPickupEv[0] = (uiScreenFrameReference >= 975) ? uiScreenFrameReference - 1000 : uiScreenFrameReference;
				uiFoodPickupEv[1] = nFoodX;
				uiFoodPickupEv[2] = nFoodY;
				bFoodEvActive = true;
						 //trigger the event in the rendering area.

					while (screen[nFoodY*nScreenWidth + nFoodX] != L' ') { //AFTER BEING EATEN, and until it's location equals an empty space, throw random numbers. These will be used later to print food.
						nFoodX = rand() % nScreenWidth;					   //The variables, though invisible, are what snake.front() actually interacts with, not the printed character.
						nFoodY = (rand() % (nScreenHeight - 4)) + 3;   //the -4 removes 4 lines of screen height, due to the UI elements, the +3 lowers the output position by 3 lines, for the same reason.
					}
					for (int i = 0; i < 5; i++) //lengthen the snake with 5 segments at its end, by adding 5 elements to the dynamic list called snake.
						snake.push_back({ snake.back().x, snake.back().y });
					}

				if (!bPaused) {
					if (difficulty > 0){
						if (nFoodStep >= (450/difficulty)) {
							nFoodStep = 0;

							nFoodX += (rand() % 3) - 1;//random food drifting, because why not.
							nFoodY += (rand() % 3) - 1;
							if (nFoodX >= nScreenWidth - 2)
								nFoodX = nScreenWidth - 2;

							if (nFoodX < 1)
								nFoodX = 1;

							if (nFoodY < 3)
								nFoodY = 3;

							if (nFoodY >= nScreenHeight - 2)
							nFoodY = nScreenHeight - 2;
						}
					}
				}
				//collision with screen edges
				if (snake.front().x == 0 || snake.front().x >= nScreenWidth - 1)
					bDead = true;
				if (snake.front().y < 3 || snake.front().y >= nScreenHeight - 1)
					bDead = true;

				// collision with itself
				for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)//spawn an iterator called i. 
					if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)//skip the front() element from consideration, since that's pos that's being compared 
				// against and it would alway return a positive.
				// the -> is used by the pointer i to dereference a member of the struct. For example, i->x == snake.front().x means
				// "is the value stored in the x member of element i equal to the value stored in the x member of the first element of the list called snake?"
						bDead = true;

				//==================DRAW SCREEN==================
				for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' '; //draw a clear screen... 

				for (int i = 0; i < nScreenWidth; i++) //draw the borders above and below. 
				{
					screen[i] = L'=';
					screen[2 * nScreenWidth + i] = L'='; 
					//starts at 2 times the width of the screen +iterator(presumably 0)? does this mean that the screen buffer is actually a linear sweep from the top left corner, going
					//down one line at a time?
					screen[39 * nScreenWidth + i] = L'=';//the window doesn't auto-size to the correct size, there's no way of knowing where the bottom is without a border.
				}
				for (int i = 3 * nScreenWidth; i < (nScreenWidth*(nScreenHeight - 1)); i += nScreenWidth) //draw the fences, left and right. Start on the 4th row, finish one row before the bottom.
				{
					screen[i] = L'X';
					screen[i + nScreenWidth - 1] = L'X';
				}

				if (uiScreenFrameReference >= 1000) uiScreenFrameReference -= 1000; //this program has strict frame exclusivity and it would be safe to just use "= 0". But in any case where it is connected to outside code, synch problems might arise wherever 
																				//you set a relative value (any point >= 1000) to an absolute value (0). It's better to reduce it by the designated range (1000), unless otherwise required.

				int Lplacement = 0;  // HUD scrolling text.
				if ((uiScreenFrameReference >= 0) && (uiScreenFrameReference < 500)) {
					Lplacement = (50 - (uiScreenFrameReference / 2));
					if (Lplacement < 0) Lplacement = 0;
					wsprintf(&screen[nScreenWidth + (5 + Lplacement)], L"Super > S N E K < 2");
				}
				else if ((uiScreenFrameReference >= 500) && (uiScreenFrameReference < 1000)) {
					Lplacement = (50 - ((uiScreenFrameReference -500) / 2));
					if (Lplacement < 0) Lplacement = 0;
					wsprintf(&screen[nScreenWidth + (5 + Lplacement)], L"Press ESC for menu and controls.");
				}

				//DRAW THE HUD
				wsprintf(&screen[nScreenWidth + 45], L"  SCORE: %d                                         ", nScore); //lol, score causes the power-gauge to move, depending on how long it is. extra spaces required due to scrolling text
				wsprintf(&screen[nScreenWidth + 65], L"Slow-Mo [           ] %d%%", nBulletTime / 10);
				if ((!bBulletTime) && (nBulletTime == 1000)) wsprintf(&screen[nScreenWidth + 86], L"READY!");
				for (int i = 0; i < (nBulletTime / 100); i++) {
					if (bBulletTime) {
						wsprintf(&screen[nScreenWidth + 74 + i], L"@");
					}
					else {
						(nBulletTime == 1000) ? wsprintf(&screen[nScreenWidth + 74 + i], L"O") : wsprintf(&screen[nScreenWidth + 74 + i], L"o");
					}
					
				}
				wsprintf(&screen[nScreenWidth + 84], L"]");//print the end of the power gauge which has been overwritten by the last charachter of the previous loop.

				// DRAW THE SNAKE
				for (auto s : snake)
					screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'O';
				// Send to the screen buffer: multiply the int value stored in Y by the width of screen. Each full length lowers the "cursor" by one line; just like when text wraps around the edge of a page.
				// Add the value stored in X to move the "cursor" to the right, to it's final position. The values stored in every instance of sSnakeSegment are just numbers; this "for" loop we're running
				//now is what puts them in the right place inside the buffer, which is linear. The rendering process is what's going to turn them into a 2d image.
				//Note the smart looking "in-line if" after the assignment operator. The multiplication was probably written first since it's what has to be done first, according to math rules. Just optics.

				//DRAW THE SNAKE'S HEAD
				screen[snake.front().y*nScreenWidth + snake.front().x] = bDead ? L'X' : L'@';

				//DRAW POWER-UP
				if(bItemSpawned) screen[nItemY*nScreenWidth + nItemX] = L'&';
				if (bItemEvActive) {
					if (uiScreenFrameReference > uiItemPickupEv[0] + 74) bItemEvActive = false;
					if (uiItemPickupEv[3]=0) wsprintf(&screen[(((uiItemPickupEv[2] - 1) - ((uiScreenFrameReference - uiItemPickupEv[0]) / 25))* nScreenWidth) + uiItemPickupEv[1] - 4], L"Power-Up!");
					else wsprintf(&screen[(((uiItemPickupEv[2] - 1) - ((uiScreenFrameReference - uiItemPickupEv[0]) / 25))* nScreenWidth) + uiItemPickupEv[1] - 2], L"BONUS!");
					//Well, that looks pretty. nFoodY minus 1, minus the difference between  the current time and the time stamp, divided by 25. Add nItemX with a small offset to the left. The complicated bit should make the text bubble rise.
				}


				//PRINT FOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOD!
				screen[nFoodY*nScreenWidth + nFoodX] = L'%';
				if (bFoodEvActive){
					if (uiScreenFrameReference > uiFoodPickupEv[0] + 74) bFoodEvActive = false;

					wsprintf(&screen[(((uiFoodPickupEv[2] - 1) - ((uiScreenFrameReference-uiFoodPickupEv[0])/25))* nScreenWidth) + uiFoodPickupEv[1]-1], L"Nom!");
					//Well, that looks pretty. nFoodY minus 1, minus the difference between  the current time and the time stamp, divided by 25. Add nfoodX with a small offset to the left. The complicated bit should make the text bubble rise.
				}
				
				//if dead, print prompt.
				if (bDead) wsprintf(&screen[15 * nScreenWidth + 40], L"    PRESS SPACE TO PLAY AGAIN    ");

				//Draw the pause menu
				if (bPaused) {
					
					wsprintf(&screen[11 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"       PAUSE!       ");
					wsprintf(&screen[12 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"********************");
					wsprintf(&screen[13 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"*      Resume      *");
					wsprintf(&screen[14 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"*    Difficulty    *");
					wsprintf(&screen[15 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"*                  *");
					wsprintf(&screen[16 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"*       Quit       *");
					wsprintf(&screen[17 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"********************");
					wsprintf(&screen[19 * nScreenWidth + ((nScreenWidth / 2) - 14)], L"Use left/righ keys to turn");
					wsprintf(&screen[21 * nScreenWidth + ((nScreenWidth / 2) - 13)], L"Press SHIFT to go faster");
					wsprintf(&screen[23 * nScreenWidth + ((nScreenWidth / 2) - 16)], L"When the Slow-Mo gauge is full,");
					wsprintf(&screen[24 * nScreenWidth + ((nScreenWidth / 2) - 13)], L"press CTRL to activate it.");

					wsprintf(&screen[37 * nScreenWidth + 2], L"Originally a tutorial by Javidx9 / https://www.onelonecoder.com");
					wsprintf(&screen[38 * nScreenWidth + 2], L"Further developed by GodGale, who doesn't have a fancy site." );


					if (((0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0) && uiMenuCursor >= 1 && !bKeyUp) {
						bKeyUp = true;
						uiMenuCursor--;
						while ((0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0);
						bKeyUp = false;
					}
					if (((0x8000 & GetAsyncKeyState((unsigned char)('\x28'))) != 0) && uiMenuCursor <= 1 && !bKeyDown) {
						bKeyDown = true;
						uiMenuCursor++;
						while ((0x8000 & GetAsyncKeyState((unsigned char)('\x28'))) != 0);
						bKeyDown = false;
					}
					if (uiMenuCursor == 0) {//resume
						wsprintf(&screen[13 * nScreenWidth + ((nScreenWidth / 2) - 5)], L"@ RESUME @");
						if ((0x8000 & GetAsyncKeyState((unsigned char)('\x0D'))) != 0) bPaused = 0;
					}
					else if (uiMenuCursor == 1) {//difficulty
						wsprintf(&screen[14 * nScreenWidth + ((nScreenWidth / 2) - 10)], L"*  @ DIFFICULTY @  *");
						wsprintf(&screen[15 * nScreenWidth + ((nScreenWidth / 2) - 5)], L"<        >");
						if (((0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0) /*&& !bKeyLeft*/) {
							//bKeyLeft = true;
							if (difficulty != 0) difficulty-=1;
							while ((0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0);
							//bKeyLeft = false;
						}
						if (((0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0) /*&& !bKeyRight*/) {
							if (difficulty != 3)  difficulty+=1;
							while ((0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0);
						}
					}
					else if (uiMenuCursor == 2) {//quit
						wsprintf(&screen[16 * nScreenWidth + ((nScreenWidth / 2) - 4)], L"@ QUIT @");
						if ((0x8000 & GetAsyncKeyState((unsigned char)('\x0D'))) != 0) return 0;
					}

					if (difficulty == 0) { //noob difficulty
						wsprintf(&screen[15 * nScreenWidth + ((nScreenWidth / 2) - 2)], L"Noob");
					}
					else if (difficulty == 1) { // easy
						wsprintf(&screen[15 * nScreenWidth + ((nScreenWidth / 2) - 2)], L"Easy");
					}
					else if (difficulty == 2) { //normal
						wsprintf(&screen[15 * nScreenWidth + ((nScreenWidth / 2) - 3)], L"Normal");
					}
					else if (difficulty == 3) {//hard
						wsprintf(&screen[15 * nScreenWidth + ((nScreenWidth / 2) - 2)], L"Hard");
					}
				}
				//DRAW THE FRAME, FINALLY!
				WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth*nScreenHeight, { 0,0 }, &dwBytesWritten);
		}
		//if ded, w8 for space. 
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);//this while holds you here until \x20 (the space-bar) is no longer false. Interesting syntax.
	}
	return 0;
}


//code cemetery, commented notepad.



/*
int main() { //PRINT WHICH DIRECTIONAL KEY WAS PRESSED LAST. updates super fast. You can use either if or while to print stuff.
	bool bRight = false, bLeft = false, bUp = false, bDown = false;//boolean states for keys.
	while (true)
	{

		bRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
		bLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
		bDown = (0x8000 & GetAsyncKeyState((unsigned char)('\x28'))) != 0;
		bUp = (0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0;
		if (bRight) {
			puts("Right key was pressed!");
			bRight = false;
		}
		if (bLeft) {
			puts("Left key was pressed!");
			bLeft = false;
		}
		if (bUp) {
			puts("Up key was pressed!");
			bUp = false;
		}
		if (bDown) {
			puts("Down key was pressed!");
			bDown = false;
		}
	}
}
*/

/*
//MAKING FILES AND FOLDERS
int main(){ //_mkdir is a part of the direct.h library, which does windows file handling. _mkdir only allows normal symbols (no weird shit).
	//_wmkdir allows for the use of 16bit characters, ie: non-standard symbols. to go from char[] to wchar_t, you need to put a capital L before the quotation marks. See below.
	if (_wmkdir( L"bacon" ) == 0) { //create a folder at the project's folder. Note the capital L before the string.
		puts("CREATED!");
	}
	if (_mkdir("bacon\\morebacon") == 0) {//create a subfolder, won't work if the first folder isn't there; doesn't create both in one go. Note the normal string.
		puts("CREATED!");
	}
	FILE *FILEPOINTER; //create a memory pointer, which will be connected to the file.
	errno = fopen_s(&FILEPOINTER, "bacon\\morebacon\\bacon.txt", "a"); //if it returns 0, everything is ok. fopen_s(memory destination, file source, reading mode).
	fprintf_s(FILEPOINTER, "bacon"); //"print" the text bacon to the file connected to the memory pointer. Appending will always add info at the end of the file.
	_fcloseall(); //close files, free memory.
	return 0;
}*/

/*
//CITIZEN STUFF
struct CITIZEN {// struct study + strcpy() + fopen
	char name[128];
	char gender;
	int age;
};
int main() {
	FILE *textFile;
	errno_t error; //better coding habits. this is used to return an error if something has gone wrong with the file-handling.
	
	char nameInput[128];
	char genderInput;
	char menuChoice1, menuChoice2;
	int ageInput;
	int citizenNumber = 1;
	error = fopen_s(&textFile, "TESTFILE.txt", "a"); //check for error. fopen syntax = fopen_s(&destination, path/filename, mode).
	if (error == 0) {
		puts("File successfully opened.");
	}
	else {
		puts("Failed to open file.");
	}
	while (true)
	{
		puts("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		puts("$ Welcome to the citizen generator! $");
		puts("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		puts("Do you wish to create a new entry?");
		scanf_s(" %c", &menuChoice1, sizeof(menuChoice1));

		if ((menuChoice1 == 'y') || (menuChoice1 == 'Y')) {
			puts("Please input the citizen's name!");
			scanf_s("%s", &nameInput, sizeof(nameInput));
			puts("Please input the citizen's gender! M / F");
			scanf_s(" %c", &genderInput, sizeof(genderInput));
			puts("Please input the citizen's age!");
			scanf_s("%i", &ageInput);
			CITIZEN Citizen1;
			Citizen1.age = ageInput;
			Citizen1.gender = genderInput;
			menuChoice1 = 'z';
			strcpy_s(Citizen1.name, sizeof(nameInput), nameInput); //you can't assign old C character arrays the same way you would for a c++ string. You have to
			//assign them new values with strcpy_s(destination, size, source).
			printf("Created: Citizen1, %s, %c, %i.\n", Citizen1.name, Citizen1.gender, Citizen1.age);

			puts("Save? Y/N");
			scanf_s(" %c", &menuChoice2, sizeof(menuChoice2));
			if ((menuChoice2 == 'Y') || (menuChoice2 == 'y')) {
				menuChoice2 = 'z';
				fprintf(textFile, "Citizen nr: %d, %s, %c, %i\n", citizenNumber, Citizen1.name, Citizen1.gender, Citizen1.age);
				puts("Saved!");
				citizenNumber++;
			}else{
				menuChoice2 = 'z';
				puts("Discarded!");
			}
		}
		else {
			puts("\nSee you next time!\n");
			fclose(textFile);
			menuChoice1 = 'z';
			return 0;
		}
	}
}
*/
/*
int main() {
	double x;
	double result;
	x = -2.5;
	result = fabs(x); 
	printf("|%.3f| = %.3f\n", x, result); // %.1f = print 1 decimal; replace the number with how many decimals you need. use f or lf depending on how big the number needs to be.


	return 0;
}*/
/*
int main() {

	int age = 30;
	int *pAge = &age;
	printf("%d  %p %i", age, pAge, *pAge); //int variable, the pointer (same as &var) and a de-referenced pointer (returns the value). Ytho?
	return 0;
}
*/
/*
int main() {//switch / array study.
	int numberIn;
	puts("Please enter a number between 1 and 7.");
	//string weekDays[8] = { "invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; //easy mode
	char weekDays[][10] = { "invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	cin >> numberIn >> letter;
	//cout << weekDays[numberIn]; //easy mode
	printf("%s", weekDays[numberIn]);//doesn't require the second set of brackets to print the char string, only if you use %c to print a letter.
	/*
	switch (numberIn) {
	case 1:
		printf("Sunday");
		break;
	case 2:
		printf("Monday");
		break;
	case 3:
		printf("Tuesday");
		break;
	case 4:
		printf("Wednesday");
		break;
	case 5:
		printf("Thursday");
		break;
	case 6:
		printf("Friday");
		break;
	case 7:
		printf("Saturday");
		break;
	default:
		printf("Invalid input; terminating.");
		break;
	}*/
//	return 0;
//}
/* 
int main(){//Euclid's Algorithm in  c++
	bool isOn = 1;
	int userInput1, userInput2, dividend, divisor, remainder;
	puts("Welcome! This software uses Euclid's Algorithm to calculate the maximum common divisor between two numbers.");
	while (isOn == 1) {
		remainder = -1;
		puts("Please input the first number!");
		scanf_s("%d", &userInput1);
		if (userInput1 == -1) return 0;
		puts("Please input the second number!");
		scanf_s("%d", &userInput2);
		if (userInput2 == -1) return 0;
		puts("Thank you! Please wait... ");
		if (userInput1 >= userInput2) {
			dividend = userInput1;
			divisor = userInput2;
		}
		else {
			dividend = userInput2;
			divisor = userInput1;
		}
		while (remainder != 0) {
			cout << dividend << " / " << divisor << " = " << dividend / divisor << " (remainder: " << dividend % divisor << ')' << endl;
			remainder = dividend % divisor;
			dividend = divisor;
			divisor = remainder;
		}
		puts("\nDONE!\a");
		cout << "The maximum common divisor between " << userInput1 << " and " << userInput2 << " is " << dividend << ".\n" << endl;
		//since the the divisor becomes the dividend, and the process only stops after the remainder has become 0, the quickest way to get the
		//divisor is to ask for the dividend instead.
	}
}*/
/*
int main() { //this one uses a loop to find out what integers can divide a given number.
	int number;
	bool isOn = true;
	while (isOn == true) {
		puts("Insert a whole, positive number. (Input -1 to exit.)");
		scanf_s("%i", &number);
		if (number == -1) return 0;
		puts("This number is divisible by: ");
		for (int i = 1; i <= number; i++) {
			if (number%i == 0) {
				printf("%i \n", i);
			}
			if (i == number) {
				puts("\n###  #### #   # ####  #");
				puts("#  # #  # ##  # #     #");
				puts("#  # #  # # # # ####  #");
				puts("#  # #  # #  ## #     ");
				puts("###  #### #   # ####  #\n\n\a");
			}
		}
	} 	
}*/
/*
int main() {//mini-calculator
	float number1;
	float number2;
	char op;
	printf("Enter a number: ");
	scanf_s("%f", &number1); //scanf_s only requires sizeof(variable) to prevent bufferoverflow with text, it already knows the max size
							 //of any numeric variable. scanf_s("%i", &varname) or scanf_s("%s", &varname, sizeof(varname)).
	printf("Enter an operator: ");
	scanf_s(" %c", &op, sizeof(op)); //it MUST have a space before the placeholder, or it counts the previous enter as an input... or is it npos????
	printf_s("Enter the second number: ");
	scanf_s("%f", &number2);
	if (op == '+') {
		printf("the result is: %f", number1 + number2);
	}
	else if (op == '-') {
		printf("the result is: %f", number1 - number2);
	}
	else if (op == '*') {
		printf("the result is: %f", number1 * number2);
	}
	else if (op == '/') {
		printf("the result is: %f", number1 / number2);
	}
	else {
		printf("Invalid input; terminating.");
	}
	return 0;
}*/


/*
int main() { // splits and counts words in a string.
	string userinput;
	puts("Enter a string of text.");
	getline(cin, userinput);

	size_t findSpace=0;
	size_t findWord= 0;
	size_t endWord = 0;
	int wordsTotal = 0;
	
	if (!userinput.empty()) {
		cout << "Word@ " << 0 << " - \"" << userinput.substr(0, userinput.find(' ', 0)) << "\"\n";
			wordsTotal++;		
	}
	else {
		puts("No input; terminating.");
	}

	while (findSpace != string::npos) { //while findSpace hasn't received npos as a value
		findSpace = userinput.find(' ', findSpace + 1 );// find the index of the next space, starting 1 index above the last space..
		//if you don't +1 the index, you'll keep getting the same space as a reply.
		endWord = userinput.find(' ', findSpace + 1)-findSpace-1; //the word's size is the distance travelled from the last found space to the next space, minus the distance we had already travelled and I'm a dumbass. Npos still counts as a space.
		findWord = userinput.find_first_not_of(' ', findSpace); //did we find something which was not a space? Not to be used for numbers.
		if (findWord != string::npos) {							// if it's not the end of the line then
			cout << "Word@ " << findSpace + 1 << " - \"" << userinput.substr(findSpace+1, endWord) <<  "\"\n";// print it's location
			wordsTotal++;
		}	
		
	}
	if (wordsTotal > 0) {
		cout << "Words found: " << wordsTotal << endl;
	}
	return 0;
}*/
/*
int main() {
	char FName[20];
	char LName[20];
	//fgets(names, sizeof(names), stdin);
	scanf_s("%s%s", FName, sizeof(FName), LName, sizeof(LName));
	printf("%s %s", FName, LName);
	return 0;
}*/
/*
int main()
{
	char name[5]; //only gives you 4 places, since ENTER produces \n, which also counts as a letter.
	puts("please enter name");
	scanf_s("%s", name, sizeof(name));
	//fgets(name, 5, stdin);
	printf("hello,your name is %s!", name);
	return 0;
}
*/
/*
int main() {
	int rng[5]{ 1,2,3,4,5};
	for (int n : rng) {
		cout << n << ' ' << " \n";
		n = n * 3;
		cout << ' ' << n << " \n";
	}
	return 0;

}*/

/*
int main() {
	vector<float> Vee;
	Vee.push_back(1);
	Vee.push_back(44);
	Vee.push_back(99);
	for (float& n : Vee) {
		cout << n << endl;
	}
	
	
}*/


/*int main() {
	string array[2][3]{ {"apple", "orange", "strawberry"},{"carrot", "kale", "pumpkin"} };
	for (int i = 0; i < 2; i++) {
		for (int n = 0; n < 3; n++) {
			cout << array[i][n] << ' ';
		}
		cout << endl;
	}
	return 0;
}*/
/*
int main() {

	int AA = (1 << 1) | (1 << 4);// binary for 18 is 0000 0000 0001 0010, two "ones": one shifted one place to the left and 
								 //									   		one shifted 4 places to the left.
	int BB = AA & ~(1<<1);		 // if you take 0000 0000 0001 0010 and add the inversion of 1<<1,
								 //				1111 1111 1111 1101, the result in binary is
								 //				0000 0000 0001 0000, 16 in decimal.

	cout << AA;
	cout << BB;
	return 0;
}
*/
/*
void longTask(void (*f)()) {
	int n = 10000;
	while (n > 0) {
		int z = n/11;
		cout << "doing stuff"[z] << ' ' << n << endl;
		n--;
	}
	void exitFunction1() {
		cout << "exit1" << endl;
	}

}


void exitFunction2() {
	cout << "exit2" << endl;
}

int main() {
	longTask(exitFunction1);
	return 0;
	
}
*/

/*
int main() {
	string greeting = "HELLO!";
	

	for (int n = 0; n < 6; n++) {
		cout << greeting[n] << endl;
	}
	return 0;
}
*/
/*
class chef{
public:
	void makeChicken() { cout << "the chef makes chicken.\n"; }
	void makeBurritos() { cout << "the chef makes burritos.\n"; }
	void makeDessert() { cout << "the chef makes dessert.\n"; }
};

class seniorChef :public chef {
public:
	void makePizza() { cout << "the chef makes pizza.\n"; }
};


int main() {
	seniorChef JJ;
	JJ.makePizza();
	return 0;
}*/
////////////////////////////////////////////////////////////////////////
/*
class Citizen {
private:
	string name;
	int civNumber, age, height, weight;
public:
	Citizen (string setname, int setcivNumber, int setage, int setheight, int setweight) {
		name = setname;
		civNumber = setcivNumber; 
		age = setage;
		height = setheight;
		weight = setweight;
		cout << "Entry Added: citizen nr " << civNumber << ",\n" << name << ",\n" << age << " years old,\n"
			<< height << " cm,\n" << weight << "kg." << endl;
	}
	void fSetAge(int setage) {
		age = setage;
	}
};

int main()
{
	string inName, command, fileName;
	int inNumber = 0, inAge, inHeight, inWeight;

	
	cout << "Please insert details:\nFull name ->";
	getline(cin, inName);
	inNumber++;
	
	cout << "Age ->";
	cin >> inAge;
	cout << "Height in cm ->";
	cin >> inHeight;
	cout << "Weight in Kg ->";
	cin >> inWeight;
	cout << "Thank you! ";
	Citizen XXX(inName, inNumber, inAge, inHeight, inWeight);
	XXX.fSetAge()
	
	return 0;
}*/
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
