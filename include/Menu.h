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

class Menu {

public:
	Menu();
	~Menu();

	void MainMenu();
	Grid *NewGame(const char *presets_path);
	Grid *LoadGame();
	bool SaveGame(SGameSettings &preset, Grid &grid);
	inline SGameSettings *GetCurrentPreset() {return &m_currentPreset;}
	inline Grid *GetCurrentGrid() {return m_currentGrid;}


private:

	SGameSettings *LoadSaveGameList(const char *path, size_t &listLenght);
	bool LoadPresets(const char *path, SGameSettings *presets);
	Grid *Load(const SGameSettings &preset);
	
	SGameSettings *m_savePresets = nullptr;
	SGameSettings m_currentPreset;
	size_t m_maxSaves;
	Grid *m_currentGrid;
};
