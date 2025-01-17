#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <cassert>
#include "conio.h"

#include "BF_version.h"
#include "Bombfield.h"
#include "Grid.h"
#include "VTS_table.h"
#include "Menu.h"

using namespace std::chrono_literals;


// ============ //
// === MAIN === //
// ============ //
int main(int argc, char **argv) {
	
	char presetsPath[64];
	memset(presetsPath, '\0', 64);

	if(argc == 2) {
		memcpy(presetsPath, argv[1], strnlen(argv[1], 64));
	} else {
		memcpy(presetsPath, "./Data/presets_default.ini", sizeof("./Data/presets_default.ini"));
	}


	bool mainQuit = false;

	Menu menu = Menu();
	
	do {

		Grid *grid = nullptr;

		std::cout << 
			CUR_HIDE 
			CLEAR_DISPLAY 
			CLEAR_SCROLL_BACK 
			CUR_TO_ORIGIN 
			BUF_ALTR_SCR 
			CUR_SHAPE_BLOCK_STEADY;
		
		printf(WINDOW_TITLE("BombField v%d.%d"), BombField_VERSION_MAJOR, BombField_VERSION_MINOR);
		
		printf(DEC_MODE);
	
		menu.MainMenu();

		int choice = _getch() - '0';
		switch(choice) {

			// NEW GAME
			case 1:
				grid = menu.NewGame(presetsPath);
				break;

			// LOAD GAME
			case 2:
				grid = menu.LoadGame();
				break;

			// QUIT
			case 'q'-'0':
			case 'Q'-'0':
				mainQuit = true;
				continue;

			default:
				break;
		}

		if(!grid) continue;

		std::cout << CUR_SHOW;
		
		int cursorRow = 1;
		int cursorColumn = 1;

		std::cout << 
			CLEAR_DISPLAY 
			CLEAR_SCROLL_BACK 
			CUR_TO_ORIGIN;


		// GRID
		grid->Draw();

		printf(DEC_MODE);

		// Main Loop inside here
		PoolInputs(menu, *grid);
		
		EGridState gridState = grid->GetState();
		if(gridState == EGridState::WON || gridState == EGridState::LOST) {

			std::cout << CUR_HIDE;
			printf(CUR_MOVE_TO, grid->GetNumOfRows() * grid->GetRowStride() + grid->GetStartRow()+grid->GetTopBorderSize(), 1);

			for(size_t i=0; i<8; i++) {

				printf(DEC_LINE_VERT "%-*s" COL_F_YELLOW "%-*.*s" COL_BF_YELLOW "%-*.*s" COL_DEFAULT DEC_LINE_VERT "\r",
						grid->GetOuterWidth() - 9,
						" ",
						i < 4 ? 3 : 0,
						(int)i,
						"[M]",
						5,
						i < 4 ? 0 : (int)i-3,
						"MENU"
					  );

				std::this_thread::sleep_for(100ms);
			}

			_getch();
			// Yeah, press M is a lie.
		}

		delete grid;

	}while(!mainQuit);


	std::cout << 
		ASCII_MODE
		CUR_SHAPE_DEFAULT 
		BUF_MAIN_SCR 
		CUR_SHOW;

	return 0;
}


// =================== //
// === POOL INPUTS === //
// =================== //
void PoolInputs(Menu &_menu, Grid &_grid) {

	int _curX = _grid.GetStartRow() + _grid.GetTopBorderSize();
	int _curY = _grid.GetStartColumn() + _grid.GetLeftBorderSize();
	printf(CUR_MOVE_TO, _curX, _curY);

	while(_grid.GetState() == EGridState::NONE) {

		switch (_getch()) {

			case 224:

				switch(_getch()) {
					case 72:
						// UP
						if(_curX > _grid.GetStartRow() + _grid.GetTopBorderSize()) {
							_curX-=_grid.GetRowStride();
						}
						break;

					case 75:
						// LEFT
						if(_curY > _grid.GetStartColumn() + _grid.GetLeftBorderSize()) {
							_curY-=_grid.GetColumnStride();
						}
						break;

					case 77:
						// RIGHT
						if(_curY < _grid.GetNumOfColumns()*_grid.GetColumnStride() + _grid.GetStartColumn() - _grid.GetColumnStride()) {
							_curY+=_grid.GetColumnStride();
						}
						break;

					case 80:
						// DOWN
						if(_curX < _grid.GetNumOfRows()*_grid.GetRowStride() + _grid.GetStartRow() - _grid.GetRowStride()) {
							_curX+=_grid.GetRowStride();
						}
						break;
				}
				break;

			case 13:
				// ENTER
				std::cout << CUR_SAVE;
				_grid.EndGameConditions(_curX, _curY);
				std::cout << CUR_LOAD;
				_grid.StartCounter();
				break;

			case 's':
			case 'S':
				_grid.PauseCursorAndCounter();
				_menu.SaveGame(_grid);
				_grid.PauseCursorAndCounter();
				break;

			case 'm':
			case 'M':
				_grid.QuitGrid();
				break;

			default:
				// OTHER
				break;
		}

		printf(CUR_MOVE_TO, _curX, _curY);
	}

}


