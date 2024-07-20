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

	int m_screenSpaceRow;
	int m_screenSpaceColumn;
	int m_nearBombs;
	bool m_isBomb;
	bool m_isHidden;

	friend std::ostream& operator<<(std::ostream& _os, const FCell &_cell);
};


// Grid
class Grid {

public:

	Grid(
		int _startRow = 1, 
		int _startColumn = 1,
		int _numOfRows = 5,
		int _numOfColumns = 5,
		float _bombsRatio = 0.1,
		bool _hasTimer = false,
		int _timeElapsed = 999
		);

	~Grid();

	void RevealCell(int _x, int _y);
	void Draw();
	void EndGameConditions(int _x, int _y);
	void StopCounter();

	inline EGridState GetState() { return m_state; }
	inline void QuitGrid() { m_state = EGridState::QUIT; }
	inline int GetNumOfRows() { return m_numOfRows; }
	inline int GetNumOfColumns() { return m_numOfColumns; }
	inline int GetRowStride() { return m_rowStride; }
	inline int GetColumnStride() { return m_columnStride; }
	inline int GetStartRow() { return m_startRow; }
	inline int GetStartColumn() { return m_startColumn; }
	inline int GetTopBorderSize() { return m_topBorderSize; }
	inline int GetLeftBorderSize() { return m_leftBorderSize; }
	inline int GetOuterWidth() { return m_outerWidth; }

	inline int GetTotalBombs() { return m_totalBombs; }
	inline int GetTotalCells() { return m_totalCells; }
	inline FCell *GetCellAt(int _x, int _y) { return &m_cells[_x][_y]; }

	inline void StartCounter(bool _start = true) { m_startCounter = _start; }
	inline void PauseCursorAndCounter() { m_pauseCursorAndCounter = !m_pauseCursorAndCounter; }



private:

	void UpdateCursorAndCounter();
	
	bool m_startCounter;
	bool m_pauseCursorAndCounter;
	bool m_hasTimer;
	int m_timeElapsed;

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
	int m_outerWidth;
	int m_totalBombs;
	float m_bombsRatio;

	FCell m_cells[50][50];

	std::thread m_cursorAndCounterUpdater;
};
