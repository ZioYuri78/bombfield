#pragma once
#include <cstdint>
#include <ostream>
#include <thread>

enum class EGridState :uint8_t {
	WON,
	LOST,
	QUIT,
	NONE
};

// Single cell
struct FCell {
	FCell();
	~FCell();

	int screen_space_row;
	int screen_space_column;
	int near_bombs;
	bool is_bomb;
	bool is_hidden;

	friend std::ostream& operator<<(std::ostream& os, const FCell &cell);
};


// Grid
class Grid {

public:

	Grid(int start_row = 1, int start_column = 1, int num_of_rows = 5, int num_of_columns = 5, float bombs_ratio = 0.1);
	~Grid();

	void RevealCell(int x, int y);
	void Draw();
	void EndGameConditions(int x, int y);
	void StopCounter();

	inline EGridState GetState() {return m_state;}
	inline void QuitGrid() {m_state = EGridState::QUIT;}
	inline int GetNumOfRows() {return m_numOfRows;}
	inline int GetNumOfColumns() {return m_numOfColumns;}
	inline int GetRowStride() {return m_rowStride;}
	inline int GetColumnStride() {return m_columnStride;}
	inline int GetStartRow() {return m_startRow;}
	inline int GetStartColumn() {return m_startColumn;}
	inline int GetTopBorderSize() {return m_topBorderSize;}
	inline int GetLeftBorderSize(){return m_leftBorderSize;}
	inline int GetTotalBombs() {return m_totalBombs;}
	inline int GetTotalCells() {return m_totalCells;}
	inline FCell *GetCellAt(int x, int y) {return &m_cells[x][y];}

	inline void StartCounter(){ m_startCounter = true;}
	inline void PauseCounter(){m_pauseCounter = !m_pauseCounter;}

private:

	void UpdateCursorAndCounter();
	
	bool m_startCounter;
	bool m_pauseCounter;

	EGridState m_state;


	int m_totalCells;
	int m_totalRevealed;
	
	int m_numOfRows;
	int m_numOfColumns;
	int m_rowStride;
	int m_columnStride;

	int m_startRow;
	int m_startColumn;

	int m_topBorderSize;
	int m_leftBorderSize;
	int m_headerWidth;
	int m_footerWidth;
	int m_totalBombs;
	float m_bombsRatio;

	FCell m_cells[50][50];

	std::thread m_cursorAndCounterUpdater;
};
