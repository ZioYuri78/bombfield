#pragma once
#include <cstdint>
#include <ostream>

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

	inline EGridState GetState() {return m_state;}
	inline void QuitGrid() {m_state = EGridState::QUIT;}
	inline int GetNumOfRows() {return m_num_of_rows;}
	inline int GetNumOfColumns() {return m_num_of_columns;}
	inline int GetRowStride() {return m_row_stride;}
	inline int GetColumnStride() {return m_column_stride;}
	inline int GetStartRow() {return m_start_row;}
	inline int GetStartColumn() {return m_start_column;}
	inline int GetTopBorderSize() {return m_top_border_size;}
	inline int GetLeftBorderSize(){return m_left_border_size;}
	inline int GetTotalBombs() {return m_total_bombs;}
	inline int GetTotalCells() {return m_total_cells;}
	inline FCell *GetCellAt(int x, int y) {return &m_cells[x][y];}

private:

	EGridState m_state;

	int m_total_cells;
	int m_total_revealed;
	
	int m_num_of_rows;
	int m_num_of_columns;
	int m_row_stride;
	int m_column_stride;

	int m_start_row;
	int m_start_column;

	int m_top_border_size;
	int m_left_border_size;
	int m_total_bombs;
	float m_bombs_ratio;

	FCell m_cells[50][50];
};
