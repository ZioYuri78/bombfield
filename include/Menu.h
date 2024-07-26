#pragma once
#include <ostream>

class Grid;

struct SGameSettings {

	SGameSettings(){}
	
	SGameSettings(
			int _id, 
			const char *_name, 
			int _numOfRows, 
			int _numberOfColumns, 
			float _bombsRatio, 
			bool _isTimed, 
			int _timeElapsed = 0
			);

	~SGameSettings(){}

	int m_id;
	char m_name[16];
	int m_numberOfRows;
	int m_numberOfColumns;
	float m_bombsRatio;
	bool m_isTimed;
	int m_timeElapsed;

	friend std::ostream& operator<<(std::ostream &_os, const SGameSettings &_gs);
};


class Menu {

public:
	Menu();
	~Menu();

	void MainMenu();
	Grid *NewGame(const char *_presetsPath);
	Grid *LoadGame();
	bool SaveGame(SGameSettings &_preset, Grid &_grid);

	inline SGameSettings *GetCurrentPreset() {return &m_currentPreset;}

private:

	SGameSettings *LoadSaveGameList(const char *_path, size_t &_listLenght);
	bool LoadPresets(const char *_path, SGameSettings *_presets);
	Grid *LoadGrid(const SGameSettings &_preset);
	
	SGameSettings *m_savePresets;
	SGameSettings m_currentPreset;
	size_t m_maxSaves;
};
