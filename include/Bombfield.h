#pragma once
#include <ostream>

struct SGameSettings {

	SGameSettings(){}
	
	SGameSettings(
			int id, 
			const char *name, 
			int numOfRows, 
			int numberOfColumns, 
			float bombsRatio, 
			bool isTimed, 
			int timeElapsed = 0);

	~SGameSettings(){}

	int m_id;
	char m_name[16];
	int m_numberOfRows;
	int m_numberOfColumns;
	float m_bombsRatio;
	bool m_isTimed;
	int m_timeElapsed;

	friend std::ostream& operator<<(std::ostream &os, const SGameSettings &gs);
};


class Grid;

SGameSettings *LoadSaveGameList(const char *path, size_t &listLenght);
Grid *LoadGame(const SGameSettings &preset);
bool SaveGame(SGameSettings &preset, const Grid &grid);

bool LoadPresets(const char *path, SGameSettings *presets);

void PoolInputs(
		SGameSettings &preset, 
		Grid &grid, 
		bool &startCounter, 
		int &cursorX, 
		int &cursorY, 
		bool &pauseUpdate); 

void UpdateCounter(
		Grid &grid, 
		bool &pauseUpdate, 
		bool *startCounter, 
		int &timeElapsed, 
		int cur_pos);
	
