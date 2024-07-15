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
	
	char presets_path[64];
	memset(presets_path, '\0', 64);

	if(argc == 2) {
		memcpy(presets_path, argv[1], strnlen(argv[1], 64));
	} else {
		memcpy(presets_path, "./Data/presets_default.ini", sizeof("./Data/presets_default.ini"));
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
			{
				grid = menu.NewGame(presets_path);
				break;
			}

			// LOAD GAME
			case 2:
			{
				grid = menu.LoadGame();
				break;
			}

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
		
		int cursor_row = 1;
		int cursor_column = 1;
		bool start_counter = false;
		

		std::cout << 
			CLEAR_DISPLAY 
			CLEAR_SCROLL_BACK 
			CUR_TO_ORIGIN;

		// HEADER: the number of bombs, smiley and time elapsed
		//menu.Header();

		// GRID
		grid->Draw();

		printf(DEC_MODE);

		// FOOTER
		menu.Footer();


		cursor_row = grid->GetStartRow() + grid->GetTopBorderSize();
		cursor_column = grid->GetStartColumn() + grid->GetLeftBorderSize();
		printf(CUR_MOVE_TO, cursor_row, cursor_column);
		
		bool pause_update = false;

		//menu.StartCounter();

		// Main loop
		while(grid->GetState() == EGridState::NONE) {
			PoolInputs(menu, cursor_row, cursor_column);
		}
		
		//menu.StopCounter();

		EGridState grid_state = grid->GetState();
		if(grid_state == EGridState::WON || grid_state == EGridState::LOST) {

			std::cout << CUR_HIDE;
			printf(CUR_MOVE_TO, grid->GetNumOfRows() * grid->GetRowStride() + grid->GetStartRow()+grid->GetTopBorderSize(), 1);

			for(size_t i=0; i<8; i++) {

				printf(DEC_LINE_VERT "%-*s" COL_F_YELLOW "%-*.*s" COL_BF_YELLOW "%-*.*s" COL_DEFAULT DEC_LINE_VERT "\r",
						menu.GetFooterWidth() - 12,
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

			int key_pressed = _getch();
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
void PoolInputs(Menu &menu, int &cur_x, int &cur_y) {

	Grid *grid = menu.GetCurrentGrid();

	switch (_getch()) {

		case 224:
			
			switch(_getch()) {
				case 72:
					// UP
					if(cur_x > grid->GetStartRow() + grid->GetTopBorderSize()) {
						cur_x-=grid->GetRowStride();
					}
					break;

				case 75:
					// LEFT
					if(cur_y > grid->GetStartColumn() + grid->GetLeftBorderSize()) {
						cur_y-=grid->GetColumnStride();
					}
					break;

				case 77:
					// RIGHT
					if(cur_y < grid->GetNumOfColumns()*grid->GetColumnStride() + grid->GetStartColumn() - grid->GetColumnStride()) {
						cur_y+=grid->GetColumnStride();
					}
					break;

				case 80:
					// DOWN
					if(cur_x < grid->GetNumOfRows()*grid->GetRowStride() + grid->GetStartRow() - grid->GetRowStride()) {
						cur_x+=grid->GetRowStride();
					}
					break;
			}
			break;
		
		case 13:
			// ENTER
			std::cout << CUR_SAVE;
			grid->EndGameConditions((cur_x-grid->GetStartRow())/grid->GetRowStride(), (cur_y-grid->GetStartColumn())/grid->GetColumnStride());
			std::cout << CUR_LOAD;
			//if(!menu.IsCounterRunning()) menu.StartCounter2();
			grid->StartCounter();
			break;
		
		case 's':
		case 'S':
			//menu.PauseUpdate(true);
			grid->PauseCounter();
			menu.SaveGame(*menu.GetCurrentPreset(), *grid);
			grid->PauseCounter();
			//menu.PauseUpdate(false);
			break;
		
		case 'm':
		case 'M':
			grid->QuitGrid();
			break;

		default:
			// OTHER
			break;
	}

	printf(CUR_MOVE_TO, cur_x, cur_y);
}


