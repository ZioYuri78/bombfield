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


	bool hasCounter = true;
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
		int header_width = 0;
		grid->GetNumOfColumns() < 6 
			? header_width = 6 * grid->GetColumnStride() 
			: header_width = (grid->GetNumOfColumns()*grid->GetColumnStride());
		
		printf(DEC_CORNER_UL);
		for(size_t hs = 0; hs < header_width-1; hs++) { std::cout << DEC_LINE_HOR;	}
		printf(DEC_CORNER_UR);
		std::cout << "\n";


		printf("%-*s " COL_BF_RED "[%03d]" COL_DEFAULT "  %2s  " COL_BF_GREEN "[%03d]" COL_DEFAULT " %*s\n",
				grid->GetNumOfColumns() < 6
				? 3
				: header_width/2 - 8,
				DEC_LINE_VERT, 
				grid->GetTotalBombs(), 
				":)", 
				hasCounter ? menu.GetCurrentPreset()->m_timeElapsed : 999,
				grid->GetNumOfColumns() < 6
				? 4
				: header_width/2 - 9,
				DEC_LINE_VERT
				);

		printf(DEC_CORNER_BL);
		for(size_t hs = 0; hs < header_width-1; hs++) { std::cout << DEC_LINE_HOR; }
		printf(DEC_CORNER_BR);
		std::cout << '\n';

		// GRID
		grid->Draw();

		printf(DEC_MODE);

		// FOOTER
		int footer_width = 0;
		grid->GetNumOfColumns() < 6
			? footer_width = 6 * grid->GetColumnStride() + 3
			: footer_width = (grid->GetNumOfColumns()*grid->GetColumnStride()) + 3;
	

		printf( DEC_LINE_VERT " %s %*s " DEC_LINE_VERT "\n",
				COL_F_YELLOW "[S]" COL_BF_YELLOW "SAVE" COL_DEFAULT,
				footer_width,
				COL_F_YELLOW "[M]" COL_BF_YELLOW "MENU" COL_DEFAULT
				);

		cursor_row = grid->GetStartRow() + grid->GetTopBorderSize();
		cursor_column = grid->GetStartColumn() + grid->GetLeftBorderSize();
		printf(CUR_MOVE_TO, cursor_row, cursor_column);
		
		bool pause_update = false;

		std::thread timer_thr = std::thread(
				&UpdateCounter, 
				std::ref(*grid), 
				std::ref(pause_update), 
				hasCounter ? &start_counter : nullptr, 
				std::ref(menu.GetCurrentPreset()->m_timeElapsed), 
				grid->GetNumOfColumns() < 6
				? (header_width/2)+5
				: (header_width/2)+6
				);

		// Main loop
		while(grid->GetState() == EGridState::NONE) {
			PoolInputs(menu, start_counter, cursor_row, cursor_column, pause_update);
		}
		
		timer_thr.join();

		auto message = [&](const char *msg) {
			int width = 0;
			grid->GetNumOfColumns() < 6
				? width = 6 * grid->GetColumnStride()
				: width = grid->GetNumOfColumns() * grid->GetColumnStride();

			printf(CUR_MOVE_TO_(2, 1));
			std::printf("%*s", width, " ");
			printf(CUR_MOVE_TO_(2, 1));
			std::printf("%-*s%s%*s\n",
					grid->GetNumOfColumns()< 6
					? 8
					: width/2 - 4, 
					DEC_LINE_VERT, 
					msg, 
					grid->GetNumOfColumns() < 6
					? 9
					: width/2 - 3, 
					DEC_LINE_VERT
					);
		};


		switch (grid->GetState()) {
			case EGridState::WON:
				message(WINDOW_TITLE("YOU WON!") COL_BF_GREEN "YOU WON!" COL_DEFAULT);
				break;
			case EGridState::LOST:
				message(WINDOW_TITLE("BOOOOOM!") COL_BF_RED "BOOOOOM!" COL_DEFAULT);
				break;
			case EGridState::QUIT:
			case EGridState::NONE:
			default:
				break;
		}

		EGridState grid_state = grid->GetState();
		if(grid_state == EGridState::WON || grid_state == EGridState::LOST) {

			std::cout << CUR_HIDE;
			printf(CUR_MOVE_TO, grid->GetNumOfRows() * grid->GetRowStride() + grid->GetStartRow()+grid->GetTopBorderSize(), 1);

			for(size_t i=0; i<8; i++) {

				printf(DEC_LINE_VERT "%-*s" COL_F_YELLOW "%-*.*s" COL_BF_YELLOW "%-*.*s" COL_DEFAULT DEC_LINE_VERT "\r",
						footer_width - 12,
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
void PoolInputs(Menu &menu, bool &start_counter, int &cur_x, int &cur_y, bool &pause_update) {

	Grid grid = *menu.GetCurrentGrid();

	switch (_getch()) {

		case 224:
			
			switch(_getch()) {
				case 72:
					// UP
					if(cur_x > grid.GetStartRow() + grid.GetTopBorderSize()) {
						cur_x-=grid.GetRowStride();
					}
					break;

				case 75:
					// LEFT
					if(cur_y > grid.GetStartColumn() + grid.GetLeftBorderSize()) {
						cur_y-=grid.GetColumnStride();
					}
					break;

				case 77:
					// RIGHT
					if(cur_y < grid.GetNumOfColumns()*grid.GetColumnStride() + grid.GetStartColumn() - grid.GetColumnStride()) {
						cur_y+=grid.GetColumnStride();
					}
					break;

				case 80:
					// DOWN
					if(cur_x < grid.GetNumOfRows()*grid.GetRowStride() + grid.GetStartRow() - grid.GetRowStride()) {
						cur_x+=grid.GetRowStride();
					}
					break;
			}
			break;
		
		case 13:
			// ENTER
			std::cout << CUR_SAVE;
			grid.EndGameConditions((cur_x-grid.GetStartRow())/grid.GetRowStride(), (cur_y-grid.GetStartColumn())/grid.GetColumnStride());
			std::cout << CUR_LOAD;
			if(!start_counter) start_counter = true;
			break;
		
		case 's':
		case 'S':
			pause_update = true;
			menu.SaveGame(*menu.GetCurrentPreset(), grid);
			pause_update = false;
			break;
		
		case 'm':
		case 'M':
			grid.QuitGrid();
			break;

		default:
			// OTHER
			break;
	}

	printf(CUR_MOVE_TO, cur_x, cur_y);
}


// ====================== //
// === UPDATE COUNTER === //
// ====================== //
void UpdateCounter(Grid &grid, bool &pause_update, bool *start_counter, int &time_elapsed, int cur_pos) {

	bool flip_flop = true;
	while(grid.GetState() == EGridState::NONE) {
		if(pause_update) {
			std::cout << CUR_HIDE;
			continue;
		}
		printf("%s", flip_flop ? CUR_SHOW : CUR_HIDE);
		flip_flop = !flip_flop;
		std::this_thread::sleep_for(500ms);
		if(start_counter && *start_counter && flip_flop) {
			std::cout << CUR_SAVE CUR_HIDE;
			printf(CUR_MOVE_TO COL_BF_GREEN "%03d" COL_DEFAULT, 2, cur_pos, ++time_elapsed);
			std::cout << CUR_LOAD CUR_SHOW;
		}
	}
}
