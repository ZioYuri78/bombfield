#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <cassert>
#include <thread>

#include "VTS_table.h"
#include "MyUtils.h"
#include "Grid.h"
#include "Menu.h"

using namespace std::chrono_literals;

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


Menu::Menu() {
	m_savePresets = nullptr;
	m_currentGrid = nullptr;
	m_cursorBlinkThr = nullptr;
	m_startCounter = false;
	m_pauseUpdate = false;
};

Menu::~Menu() {

	if(m_savePresets) {
		delete[] m_savePresets;
	}

	// We deallocate the memory in the main
	// loop (look at Bombfield.cpp)
	
	// delete m_currentGrid;
	
};

void Menu::MainMenu() {

	delete[] m_savePresets;
	m_savePresets = LoadSaveGameList("./Data/save_game_list.dat", m_maxSaves);

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
			m_savePresets ? COL_F_YELLOW "[2]" COL_BF_YELLOW "LOAD" : COL_F_EXTENDED(100, 75, 0) "[2]LOAD",
			COL_F_YELLOW "[Q]" COL_BF_YELLOW "QUIT" COL_DEFAULT);

	printf(DEC_CORNER_BL);
	for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
	printf(DEC_CORNER_BR);
}


// ================ //
// === NEW GAME === //
// ================ //
Grid *Menu::NewGame(const char *presets_path) {

	m_currentGrid = nullptr;

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

	SGameSettings game_presets[10];
	bool has_presets = LoadPresets(presets_path, game_presets);
	
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
			int m_currentGrid_start_row = 4;
			int m_currentGrid_start_column = 1;
			m_currentPreset = game_presets[value-'0'];
			int numRows = m_currentPreset.m_numberOfRows;
			int numCols = m_currentPreset.m_numberOfColumns;
			float bombsRatio = m_currentPreset.m_bombsRatio;
			bool hasCounter = m_currentPreset.m_isTimed;
			m_currentGrid = new Grid(m_currentGrid_start_row, m_currentGrid_start_column, numRows, numCols, bombsRatio);
			break;
		}
	}while(value != 'b');

	return m_currentGrid;
}


// ================= //
// === LOAD GAME === //
// ================= //
Grid *Menu::LoadGame() {

	m_currentGrid = nullptr;

	if(m_savePresets) {
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

		for(size_t i=0; i<m_maxSaves; i++) {
			printf(DEC_LINE_VERT " " COL_BF_YELLOW ASCII_MODE "[%02zu] ", i);
			std::cout << m_savePresets[i] << DEC_MODE COL_DEFAULT " " DEC_LINE_VERT "\n";
		}


		printf(DEC_CORNER_BL);
		for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
		printf(DEC_CORNER_BR);

		int value = 0;
		do{
			value = _getch();
			if(value >= '0' && value < (char)(m_maxSaves+'0')){
				m_currentPreset = m_savePresets[value-'0'];
				m_currentGrid = Load(m_currentPreset);
				break;
			}
		}while(value != 'b');
	}
	return m_currentGrid;
}


// ================= //
// === SAVE GAME === //
// ================= //
bool Menu::SaveGame(SGameSettings &preset, Grid &m_currentGrid) {
	
	int width = 0;
	m_currentGrid.GetNumOfColumns() < 6 
		? width = 6 * m_currentGrid.GetColumnStride() + 3 
		: width = (m_currentGrid.GetNumOfColumns()*m_currentGrid.GetColumnStride()) + 3;
	
	int height = m_currentGrid.GetNumOfRows() * m_currentGrid.GetRowStride() + m_currentGrid.GetStartRow()+m_currentGrid.GetTopBorderSize();

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


	FILE *data = fopen("./Data/save_game_list.dat", "rb+");
	if(!data) {
		data = fopen("./Data/save_game_list.dat", "wb+");
		if(!data) {
			printf( COL_BF_RED "ERROR: can't create %s\n" COL_DEFAULT, "./Data/save_game_list.dat");
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
	sprintf(path, "./Data/save_%d.dat", preset.m_id);

	data = fopen(path, "wb");
	if(!data) {
		printf(COL_BF_RED "ERROR: can't create %s\n" COL_DEFAULT, path);
		return false;
	}

	fwrite(&m_currentGrid, sizeof(Grid), 1, data);
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


void Menu::Header() {

	m_headerWidth = 50;
}

void Menu::Footer() {

	m_footerWidth = 50;

}

void Menu::StartCounter() {

	m_pauseUpdate = false;
	m_startCounter = false;

	m_cursorBlinkThr = new std::thread(
			&Menu::UpdateCounter,
			this,
			m_currentGrid->GetNumOfColumns() < 6
			? (m_headerWidth/2)+5
			: (m_headerWidth/2)+6
			);
}

void Menu::StopCounter() {
	m_cursorBlinkThr->join();
	delete m_cursorBlinkThr;
}


// About save_game_list.dat content:
// No more than 10 saves and we assume that ID go from 0 to 9
// So the first entry in the list has ID = 0 and refer to 
// file save_00.dat
SGameSettings *Menu::LoadSaveGameList(const char *path, size_t &list_lenght) {

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


// ==================== //
// === LOAD PRESETS === //
// ==================== //
bool Menu::LoadPresets(const char *path, SGameSettings *presets) {

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


// ============ //
// === LOAD === //
// ============ //
Grid *Menu::Load(const SGameSettings &preset) {

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
	sprintf(path, "./Data/save_%d.dat", preset.m_id);
	
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

// ====================== //
// === UPDATE COUNTER === //
// ====================== //
void Menu::UpdateCounter(int cur_pos) {

	bool flip_flop = true;
	while(m_currentGrid->GetState() == EGridState::NONE) {
		if(m_pauseUpdate) {
			std::cout << CUR_HIDE;
			continue;
		}
		printf("%s", flip_flop ? CUR_SHOW : CUR_HIDE);
		flip_flop = !flip_flop;
		std::this_thread::sleep_for(500ms);
		if(m_currentPreset.m_isTimed && m_startCounter && flip_flop) {
			std::cout << CUR_SAVE CUR_HIDE;
			printf(CUR_MOVE_TO COL_BF_GREEN "%03d" COL_DEFAULT, 2, cur_pos, ++m_currentPreset.m_timeElapsed);
			std::cout << CUR_LOAD CUR_SHOW;
		}
	}
}
