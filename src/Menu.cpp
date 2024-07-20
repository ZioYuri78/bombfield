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
		int _id, 
		const char *_name, 
		int _numOfRows, 
		int _numberOfColumns, 
		float _bombsRatio, 
		bool _isTimed, 
		int _timeElapsed/* = 0*/
		) {
	m_id = _id;
	assert(strlen(_name) <= 16);
	strcpy_s(m_name, _name);
	m_numberOfRows = _numOfRows;
	m_numberOfColumns = _numberOfColumns;
	m_bombsRatio = _bombsRatio;
	m_isTimed = _isTimed;
	m_timeElapsed = _timeElapsed;

}

std::ostream& operator<<(std::ostream &_os, const SGameSettings &_gs) {
	char description[64];
	sprintf(description, "%-16s %2dx%-2d %3.0f%% %6s", 
			_gs.m_name, 
			_gs.m_numberOfRows, 
			_gs.m_numberOfColumns, 
			_gs.m_bombsRatio*100, 
			_gs.m_isTimed ? "yes" : "no"
		   );
	_os << description;
	return _os;
}


Menu::Menu() {
	m_savePresets = nullptr;
	m_currentGrid = nullptr;
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
			m_savePresets ? COL_F_YELLOW "[2]" COL_BF_YELLOW "LOAD" : COL_F_EXTENDED_(100, 75, 0) "[2]LOAD",
			COL_F_YELLOW "[Q]" COL_BF_YELLOW "QUIT" COL_DEFAULT);

	printf(DEC_CORNER_BL);
	for(size_t i=0; i<41; i++){std::cout << DEC_LINE_HOR;}
	printf(DEC_CORNER_BR);
}


// ================ //
// === NEW GAME === //
// ================ //
Grid *Menu::NewGame(const char *_presetsPath) {

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

	SGameSettings gamePresets[10];
	bool hasPresets = LoadPresets(_presetsPath, gamePresets);
	
	if(hasPresets) {

		for(size_t i=0; i<10; i++) {
			std::cout << 
				DEC_LINE_VERT 
				" " 
				COL_BF_YELLOW ASCII_MODE "[0" << i << "] " << gamePresets[i] << DEC_MODE COL_DEFAULT 
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
		if(hasPresets && value >= '0' && value <= '9'){
			m_currentPreset = gamePresets[value-'0'];
			m_currentGrid = new Grid(
					4, 
					1, 
					m_currentPreset.m_numberOfRows,
					m_currentPreset.m_numberOfColumns, 
					m_currentPreset.m_bombsRatio,  
					m_currentPreset.m_isTimed,     
					m_currentPreset.m_timeElapsed
					);
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
				m_currentGrid = LoadGrid(m_currentPreset);
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
	size_t fileSize = ftell(data);
	int numSaves = fileSize/sizeof(SGameSettings);
	if(numSaves < preset.m_id + 1) {
		preset.m_id = numSaves;
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

	fwrite(&m_currentGrid, sizeof(Grid) - sizeof(std::thread), 1, data);
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

// About save_game_list.dat content:
// No more than 10 saves and we assume that ID go from 0 to 9
// So the first entry in the list has ID = 0 and refer to 
// file save_00.dat
SGameSettings *Menu::LoadSaveGameList(const char *_path, size_t &_listLenght) {

	SGameSettings *gs = nullptr;
	FILE *data = fopen(_path, "rb");
	if(data) {	

		fseek(data, 0, SEEK_END);

		_listLenght = ftell(data)/sizeof(SGameSettings);
		if(_listLenght > 0) {

			gs = new SGameSettings[_listLenght];
			fseek(data,0, SEEK_SET);

			for(size_t i=0; i<_listLenght; i++) {
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
bool Menu::LoadPresets(const char *_path, SGameSettings *_presets) {

	FILE *data = fopen(_path, "r");
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
		if(_presets) {
			_presets[i] = 
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
Grid *Menu::LoadGrid(const SGameSettings &_preset) {

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
	sprintf(path, "./Data/save_%d.dat", _preset.m_id);
	
	Grid *g = nullptr;
	FILE *data = fopen(path, "rb");
	if(data) {
		g = new Grid();
		fread(g, sizeof(Grid)-sizeof(std::thread), 1, data);
		fclose(data);
		g->StartCounter(false);
	}

	std::this_thread::sleep_for(500ms);
	std::cout << CUR_LOAD;

	return g;
}

