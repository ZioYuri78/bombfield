#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <cassert>
#include "conio.h"

#include "BF_version.h"
#include "Bombfield.h"
#include "MyUtils.h"
#include "Grid.h"
#include "VTS_table.h"

using namespace std::chrono_literals;

#define SAVE_LIST_PATH "./Data/save_game_list.dat"
#define SAVE_SLOT_PATH "./Data/save_%d.dat"

SGameSettings::SGameSettings(
		int id, 
		const char *name, 
		int numOfRows, 
		int numberOfColumns, 
		float bombsRatio, 
		bool isTimed, 
		int timeElapsed/* = 0*/)
{
	m_id = id;
	assert(strlen(name) <= 16);
	strcpy_s(m_name, name);
	m_numberOfRows = numOfRows;
	m_numberOfColumns = numberOfColumns;
	m_bombsRatio = bombsRatio;
	m_isTimed = isTimed;
	m_timeElapsed = timeElapsed;

}

std::ostream& operator<<(std::ostream &os, const SGameSettings &gs) {
	char description[64];
	sprintf(description, "%-16s %2dx%-2d %3.0f%% %6s", 
			gs.m_name, 
			gs.m_numberOfRows, 
			gs.m_numberOfColumns, 
			gs.m_bombsRatio*100, 
			gs.m_isTimed ? "yes" : "no"
		   );
	os << description;
	return os;
}


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


	int numRows = 5;
	int numCols = 5;
	float bombsRatio = 0.1f;
	bool hasCounter = true;
	bool mainQuit = false;

	do {
		
		SGameSettings game_presets[10];
		SGameSettings current_preset;
		bool has_presets = LoadPresets(presets_path, game_presets);
		
		size_t max_saves;
		SGameSettings *save_presets = LoadSaveGameList(SAVE_LIST_PATH, max_saves);

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
		
		printf(DEC_CORNER_UL);
		for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
		printf(DEC_CORNER_UR);
		
		std::cout << '\n';
		std::cout << DEC_LINE_VERT "                " COL_BF_RED "BOMBFIELD" COL_DEFAULT "                " DEC_LINE_VERT "\n";
		
		printf(DEC_CROSS_HL);
		for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
		printf(DEC_CROSS_HR);
		std::cout << '\n';
		
		printf(DEC_LINE_VERT" %s %s %38s " DEC_LINE_VERT "\n",
				COL_F_YELLOW "[1]" COL_BF_YELLOW "NEW" COL_DEFAULT,
				save_presets ? COL_F_YELLOW "[2]" COL_BF_YELLOW "LOAD" : COL_F_EXTENDED(100, 75, 0) "[2]LOAD",
				COL_F_YELLOW "[Q]" COL_BF_YELLOW "QUIT" COL_DEFAULT);

		printf(DEC_CORNER_BL);
		for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
		printf(DEC_CORNER_BR);
		
		int choice = _getch() - '0';
		switch(choice) {

			// NEW GAME
			case 1:
			{

				std::cout << CUR_MOVE_UP "\r";
				printf(DEC_LINE_VERT " %s %44s " DEC_LINE_VERT "\n", 
						COL_BF_YELLOW "NEW GAME", 
						COL_F_YELLOW "[B]" COL_BF_YELLOW "BACK" COL_DEFAULT);
				
				printf(DEC_CROSS_HL CUR_MOVE_FORWARD_(41) DEC_CROSS_HR);
				

				std::cout << 
					"\n" DEC_LINE_VERT " " 
					COL_BF_YELLOW 
					ASCII_MODE "[ID] name             size  ratio  time" DEC_MODE
					COL_DEFAULT 
					" " DEC_LINE_VERT "\n";
				
				std::cout << DEC_LINE_VERT " ";
				for(size_t i=0; i<39; i++){std::cout << DEC_LINE_HOR;}
				std::cout << " " DEC_LINE_VERT "\n";
				
				if(has_presets) {

					for(size_t i=0; i<10; i++) {
						std::cout << 
							DEC_LINE_VERT 
							" " 
							COL_BF_YELLOW ASCII_MODE "[0" << i << "] " << game_presets[i] << DEC_MODE COL_DEFAULT 
							" " 
							DEC_LINE_VERT 
							"\n";
					}

				} else {
					std::cout << 
						DEC_LINE_VERT 
						"        " 
						COL_BF_RED ASCII_MODE "ERROR: can't find presets" DEC_MODE COL_DEFAULT 
						"        " 
						DEC_LINE_VERT 
						"\n";
				}

				printf(DEC_CORNER_BL);
				for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
				printf(DEC_CORNER_BR);

				int value = 0;
				do{
					value = _getch();
					if(has_presets && value >= '0' && value <= '9'){
						int grid_start_row = 4;
						int grid_start_column = 1;
						current_preset = game_presets[value-'0'];
						numRows = current_preset.m_numberOfRows;
						numCols = current_preset.m_numberOfColumns;
						bombsRatio = current_preset.m_bombsRatio;
						hasCounter = current_preset.m_isTimed;
						grid = new Grid(grid_start_row, grid_start_column, numRows, numCols, bombsRatio);
						break;
					}
				}while(value != 'b');
				break;
			}

			// LOAD GAME
			case 2:
			{
				if(save_presets) {
					std::cout << CUR_MOVE_UP "\r";
				    printf(DEC_LINE_VERT " %s %43s " DEC_LINE_VERT "\n", 
							COL_BF_YELLOW "LOAD GAME", 
							COL_F_YELLOW "[B]" COL_BF_YELLOW "BACK" COL_DEFAULT
							);

					printf(DEC_CROSS_HL CUR_MOVE_FORWARD_(41) DEC_CROSS_HR);
					
					std::cout << '\n';
					std::cout << 
						DEC_LINE_VERT 
						" " 
						COL_BF_YELLOW ASCII_MODE "[ID] name             size  ratio  time" DEC_MODE COL_DEFAULT 
						" "
						DEC_LINE_VERT
						"\n";
					
					std::cout << DEC_LINE_VERT " ";
					for(size_t i=0; i<39; i++){std::cout << DEC_LINE_HOR;}
					std::cout << " " DEC_LINE_VERT "\n";

					for(size_t i=0; i<max_saves; i++) {
						printf(DEC_LINE_VERT " " COL_BF_YELLOW ASCII_MODE "[%02zu] ", i);
						std::cout << save_presets[i] << DEC_MODE COL_DEFAULT " " DEC_LINE_VERT "\n";
					}


					printf(DEC_CORNER_BL);
					for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
					printf(DEC_CORNER_BR);

					int value = 0;
					do{
						value = _getch();
						if(value >= '0' && value < (char)(max_saves+'0')){
							current_preset = save_presets[value-'0'];
							grid = LoadGame(current_preset);
							break;
						}
					}while(value != 'b');
				}
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
				hasCounter ? current_preset.m_timeElapsed : 999,
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
				std::ref(current_preset.m_timeElapsed), 
				grid->GetNumOfColumns() < 6
				? (header_width/2)+5
				: (header_width/2)+6
				);

		// Main loop
		while(grid->GetState() == EGridState::NONE) {
			PoolInputs(current_preset, *grid, start_counter, cursor_row, cursor_column, pause_update);
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

		delete[] save_presets;
		delete grid;

	}while(!mainQuit);


	std::cout << 
		ASCII_MODE
		CUR_SHAPE_DEFAULT 
		BUF_MAIN_SCR 
		CUR_SHOW;

	return 0;
}

// About save_game_list.dat content:
// No more than 10 saves and we assume that ID go from 0 to 9
// So the first entry in the list has ID = 0 and refer to 
// file save_00.dat
SGameSettings *LoadSaveGameList(const char *path, size_t &list_lenght) {

	SGameSettings *gs = nullptr;
	FILE *data = fopen(path, "rb");
	if(data) {	

		fseek(data, 0, SEEK_END);

		list_lenght = ftell(data)/sizeof(SGameSettings);
		if(list_lenght > 0) {

			gs = new SGameSettings[list_lenght];
			fseek(data,0, SEEK_SET);

			for(size_t i=0; i<list_lenght; i++) {
				fread(&gs[i], sizeof(SGameSettings), 1, data);
			}
		}

		fclose(data);
	}

	return gs;
}

// ================= //
// === LOAD GAME === //
// ================= //
Grid *LoadGame(const SGameSettings &preset) {

	std::cout << 
		CUR_SAVE 
		CUR_TO_ORIGIN 
		CUR_MOVE_DOWN_(3);

	for(size_t i=0; i<11; i++) {

		printf(DEC_LINE_VERT " " COL_BF_YELLOW "%.*s %*s " DEC_LINE_VERT "\r",
					(int)i,
					"LOADING...",
					52-(int)i,
					COL_F_YELLOW "[B]" COL_BF_YELLOW "BACK" COL_DEFAULT
					);

		std::this_thread::sleep_for(100ms);
	}
	
	char path[256];
	sprintf(path, SAVE_SLOT_PATH, preset.m_id);
	
	Grid *g = nullptr;
	FILE *data = fopen(path, "rb");
	if(data) {
		g = new Grid();
		fread(g, sizeof(Grid), 1, data);
		fclose(data);
	}

	std::this_thread::sleep_for(500ms);
	std::cout << CUR_LOAD;

	return g;
}



// ================= //
// === SAVE GAME === //
// ================= //
bool SaveGame(SGameSettings &preset, Grid &grid) {
	
	int width = 0;
	grid.GetNumOfColumns() < 6 
		? width = 6 * grid.GetColumnStride() + 3 
		: width = (grid.GetNumOfColumns()*grid.GetColumnStride()) + 3;
	
	int height = grid.GetNumOfRows() * grid.GetRowStride() + grid.GetStartRow()+grid.GetTopBorderSize();

	std::cout << 
		CUR_SAVE 
		CUR_HIDE;

	printf(CUR_MOVE_TO, height, 1);
	
	for(size_t i=0; i<7; i++) {

		printf(DEC_LINE_VERT " " COL_F_YELLOW "[]" COL_BF_YELLOW "%.*s %*s " DEC_LINE_VERT "\r",
					(int)i,
					"SAVING",
					width+5-(int)i,
					COL_F_YELLOW "[M]" COL_BF_YELLOW "MENU" COL_DEFAULT
				);

		std::this_thread::sleep_for(100ms);
	}


	FILE *data = fopen(SAVE_LIST_PATH, "rb+");
	if(!data) {
		data = fopen(SAVE_LIST_PATH, "wb+");
		if(!data) {
			printf( COL_BF_RED "ERROR: can't create %s\n" COL_DEFAULT, SAVE_LIST_PATH);
			return false;
		}
	}
	
	long offset = 0;
	fseek(data, 0, SEEK_END);
	size_t file_size = ftell(data);
	int num_saves = file_size/sizeof(SGameSettings);
	if(num_saves < preset.m_id + 1) {
		preset.m_id = num_saves;
	}
	offset = preset.m_id;

	fseek(data, sizeof(SGameSettings)*offset, SEEK_SET);
	fwrite(&preset, sizeof(SGameSettings), 1, data);
	fclose(data);

	char path[256];
	sprintf(path, SAVE_SLOT_PATH, preset.m_id);

	data = fopen(path, "wb");
	if(!data) {
		printf(COL_BF_RED "ERROR: can't create %s\n" COL_DEFAULT, path);
		return false;
	}

	fwrite(&grid, sizeof(Grid), 1, data);
	fclose(data);

	std::this_thread::sleep_for(500ms);
	
	printf( DEC_LINE_VERT " %s %*s " DEC_LINE_VERT "\n",
			COL_F_YELLOW "[S]" COL_BF_YELLOW "SAVE" COL_DEFAULT,
			width,
			COL_F_YELLOW "[M]" COL_BF_YELLOW "MENU" COL_DEFAULT
			);

	std::cout << 
		CUR_LOAD 
		CUR_SHOW;
	
	return true;
}


// ==================== //
// === LOAD PRESETS === //
// ==================== //
bool LoadPresets(const char *path, SGameSettings *presets) {

	FILE *data = fopen(path, "r");
	if(!data) {
		return false;
	}

	for(size_t i=0; i<10; i++) {
		char name[16];
		int rows, columns, timed;
		float ratio;
		fscanf(data, "%15s %d %d %f %d", name, &rows, &columns, &ratio, &timed);
		if(name[0] == '#') {
			fscanf(data, "%*[^\n]\n");
			i--;
			continue;
		}
		if(presets) {
			presets[i] = 
				SGameSettings(
						i, 
						name, 
						MyUtils::Clamp(rows, 2, 24), 
						MyUtils::Clamp(columns, 2, 45),
						MyUtils::Clamp(ratio, 0.0f, 1.0f), 
						timed
						);
		} else {
			// if preset is nullptr just print as debug.
			printf("%s %d %d %f %d\n", name, rows, columns, ratio, timed);
		}

	} 	

	fclose(data);
	return true;
}

// =================== //
// === POOL INPUTS === //
// =================== //
void PoolInputs(SGameSettings &preset, Grid &grid, bool &start_counter, int &cur_x, int &cur_y, bool &pause_update) {

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
			SaveGame(preset, grid);
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
