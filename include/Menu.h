#pragma once
#include <ostream>
#include <thread>

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
class thread;

class Menu {

public:
	Menu();
	~Menu();

	void MainMenu();
	Grid *NewGame(const char *presets_path);
	Grid *LoadGame();
	bool SaveGame(SGameSettings &preset, Grid &grid);
	void Header();
	void Footer();
	void StartCounter();
	void StopCounter();

	inline SGameSettings *GetCurrentPreset() {return &m_currentPreset;}
	inline Grid *GetCurrentGrid() {return m_currentGrid;}
	inline int GetHeaderWidth() {return m_headerWidth;}
	inline int GetFooterWidth() {return m_footerWidth;}
	inline void PauseUpdate(bool value) {m_pauseUpdate = value;}
	inline void StartCounter2() {m_startCounter = true;}
	inline bool IsCounterRunning() {return m_startCounter;}

private:

	SGameSettings *LoadSaveGameList(const char *path, size_t &listLenght);
	bool LoadPresets(const char *path, SGameSettings *presets);
	Grid *Load(const SGameSettings &preset);
	void UpdateCounter(int cur_pos);
	
	SGameSettings *m_savePresets = nullptr;
	SGameSettings m_currentPreset;
	size_t m_maxSaves;
	Grid *m_currentGrid;
	int m_headerWidth;
	int m_footerWidth;

	std::thread *m_cursorBlinkThr;
	bool m_pauseUpdate;
	bool m_startCounter;
	
};
