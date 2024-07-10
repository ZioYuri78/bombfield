#include <iostream>
#include <random>
#include "VTS_table.h"
#include "Grid.h"

// =================== //
// === Single cell === //
// =================== //
FCell::FCell() {
	screen_space_row = -1;
	screen_space_column = -1;
	near_bombs = 0;
	is_hidden = true; 
	is_bomb = false;
}

FCell::~FCell(){}

std::ostream& operator<<(std::ostream& os, const FCell &cell) {

	if(!cell.is_hidden) {
		if(cell.is_bomb) {
			os << COL_BF_RED "*" COL_DEFAULT;
		} else if(cell.near_bombs > 0) {
			os << COL_BF_YELLOW << cell.near_bombs << COL_DEFAULT; 
		} else {
			os << COL_B_BLACK " " COL_DEFAULT;
		}
	} else {
		os << COL_B_WHITE " " COL_DEFAULT;
	}

	return os;
}


// ============ // 
// === Grid === //
// ============ //
Grid::Grid(int start_row /*= 1*/, int start_column /*= 1*/, int num_of_rows /*= 5*/, int num_of_columns /*= 5*/, float bombs_ratio /*= 0.1*/) {
	
	std::random_device rd;
	std::mt19937 gen32(rd());

	m_state = EGridState::NONE;
	m_total_cells = num_of_rows * num_of_columns;
	m_total_revealed = 0;
	m_num_of_rows = num_of_rows;
	m_num_of_columns = num_of_columns;
	m_row_stride = 2;
	m_column_stride = 4;
	m_start_row = start_row;
	m_start_column = start_column;
	m_top_border_size = 1;
	m_left_border_size = 2;
	m_bombs_ratio = bombs_ratio;
	m_total_bombs = ceil(m_total_cells * bombs_ratio);
	
	for(size_t i=0; i<num_of_rows; i++) {
		for(size_t j=0; j<num_of_columns; j++) {
			m_cells[i][j].screen_space_row = (i*m_row_stride)+start_row+m_top_border_size;
			m_cells[i][j].screen_space_column = (j*m_column_stride)+start_column+m_left_border_size;
		}
	}

	for(size_t b=0; b<m_total_bombs; b++){
		
		int x, y;
		while(true) {
			x = gen32()%num_of_rows;
			y = gen32()%num_of_columns;
			if(!m_cells[x][y].is_bomb) {
				break;
			}
		}

		m_cells[x][y].is_bomb = true;
		
		// Top Left
		if(x == 0 && y == 0) {
			m_cells[x+1][y].near_bombs++;
			m_cells[x][y+1].near_bombs++;
			m_cells[x+1][y+1].near_bombs++;
		}else 
		// Top Right
		if(x == 0 && y == num_of_columns-1) {
			m_cells[x+1][y].near_bombs++;
			m_cells[x][y-1].near_bombs++;
			m_cells[x+1][y-1].near_bombs++;
		}else
		// Top Row
		if(x == 0) {
			m_cells[x][y-1].near_bombs++;
			m_cells[x][y+1].near_bombs++;
			m_cells[x+1][y-1].near_bombs++;
			m_cells[x+1][y].near_bombs++;
			m_cells[x+1][y+1].near_bombs++;
		}else
		// Bottom Left
		if(x == num_of_rows-1 && y == 0) {
			m_cells[x-1][y].near_bombs++;
			m_cells[x][y+1].near_bombs++;
			m_cells[x-1][y+1].near_bombs++;
		}else
		// Bottom Right
		if(x == num_of_rows-1 && y == num_of_columns-1) {
			m_cells[x-1][y].near_bombs++;
			m_cells[x][y-1].near_bombs++;
			m_cells[x-1][y-1].near_bombs++;
		}else
		// Bottom Row
		if(x == num_of_rows-1) {
			m_cells[x][y-1].near_bombs++;
			m_cells[x][y+1].near_bombs++;
			m_cells[x-1][y-1].near_bombs++;
			m_cells[x-1][y].near_bombs++;
			m_cells[x-1][y+1].near_bombs++;
		}else
		// Left Column
		if(y == 0) {
			m_cells[x-1][y].near_bombs++;
			m_cells[x+1][y].near_bombs++;
			m_cells[x-1][y+1].near_bombs++;
			m_cells[x][y+1].near_bombs++;
			m_cells[x+1][y+1].near_bombs++;
		}else
		// Right Column
		if(y == num_of_columns-1) {
			m_cells[x-1][y].near_bombs++;
			m_cells[x+1][y].near_bombs++;
			m_cells[x-1][y-1].near_bombs++;
			m_cells[x][y-1].near_bombs++;
			m_cells[x+1][y-1].near_bombs++;
		} else {
			// If none of above the bomb is in the middle
			m_cells[x-1][y-1].near_bombs++;
			m_cells[x-1][y].near_bombs++;
			m_cells[x-1][y+1].near_bombs++;
			m_cells[x][y-1].near_bombs++;
			m_cells[x][y+1].near_bombs++;
			m_cells[x+1][y-1].near_bombs++;
			m_cells[x+1][y].near_bombs++;
			m_cells[x+1][y+1].near_bombs++;
		}
	}
}


Grid::~Grid() {}


void Grid::RevealCell(int x, int y) {
	
	FCell &cell = m_cells[x][y];
	if(!cell.is_hidden) return;
	
	m_total_revealed++;

	cell.is_hidden = false;
	printf(CUR_MOVE_TO, cell.screen_space_row, cell.screen_space_column);
	std::cout << cell;

	if(cell.near_bombs == 0) {
		
		// Top Left
		if(x == 0 && y == 0) {
			RevealCell(x+1, y);
			RevealCell(x, y+1);
			RevealCell(x+1, y+1);
		}else 
		// Top Right
		if(x == 0 && y == m_num_of_columns-1) {
			RevealCell(x+1, y);
			RevealCell(x, y-1);
			RevealCell(x+1, y-1);
		}else
		// Top Row
		if(x == 0) {
			RevealCell(x, y-1);
			RevealCell(x, y+1);
			RevealCell(x+1, y-1);
			RevealCell(x+1, y);
			RevealCell(x+1, y+1);
		}else
		// Bottom Left
		if(x == m_num_of_rows-1 && y == 0) {
			RevealCell(x-1, y);
			RevealCell(x, y+1);
			RevealCell(x-1, y+1);
		}else
		// Bottom Right
		if(x == m_num_of_rows-1 && y == m_num_of_columns-1) {
			RevealCell(x-1, y);
			RevealCell(x, y-1);
			RevealCell(x-1, y-1);
		}else
		// Bottom Row
		if(x == m_num_of_rows-1) {
			RevealCell(x, y-1);
			RevealCell(x, y+1);
			RevealCell(x-1, y-1);
			RevealCell(x-1, y);
			RevealCell(x-1, y+1);
		}else
		// Left Column
		if(y == 0) {
			RevealCell(x-1, y);
			RevealCell(x+1, y);
			RevealCell(x-1, y+1);
			RevealCell(x, y+1);
			RevealCell(x+1, y+1);
		}else
		// Right Column
		if(y == m_num_of_columns-1) {
			RevealCell(x-1, y);
			RevealCell(x+1, y);
			RevealCell(x-1, y-1);
			RevealCell(x, y-1);
			RevealCell(x+1, y-1);
		} else {
			// Middle
			RevealCell(x-1, y-1);
			RevealCell(x-1, y);
			RevealCell(x-1, y+1);
			RevealCell(x, y-1);
			RevealCell(x, y+1);
			RevealCell(x+1, y-1);
			RevealCell(x+1, y);
			RevealCell(x+1, y+1);
		}
	}
}


void Grid::Draw() {

	printf(CUR_MOVE_TO DEC_MODE, m_start_row, m_start_column);
	
	// Top Rows divisor
	printf(DEC_CORNER_UL DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR);
	for(size_t k=0; k<m_num_of_columns-2; k++) {
		std::cout << DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR;
	}
	printf(DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_CORNER_UR);

	std::cout << '\n';
	for(size_t j=0; j<m_num_of_rows-1; j++){				

		// Row
		printf(CUR_MOVE_HOR_ABS, m_start_column);
		std::cout << DEC_LINE_VERT;
		for(size_t c=0; c<m_num_of_columns; c++) {
			std::cout << ' ' << m_cells[j][c] << " " DEC_LINE_VERT;
		}

		// Middle Rows divisor
		std::cout << '\n';
		printf(CUR_MOVE_HOR_ABS, m_start_column);
		printf(DEC_LINE_VERT);
		for(size_t k=0; k<m_num_of_columns-1; k++) {
			std::cout << " " DEC_LINE_HOR "  ";
		}
		printf(" " DEC_LINE_HOR " " DEC_LINE_VERT);
		std::cout << '\n';
	}

	// Last Row
	printf(CUR_MOVE_HOR_ABS, m_start_column);
	std::cout << DEC_LINE_VERT;
	for(size_t c=0; c<m_num_of_columns; c++) {
		std::cout << ' ' << m_cells[m_num_of_rows-1][c] << " " DEC_LINE_VERT;
	}
	std::cout << '\n';
	
	// Bottom row divisor
	printf(CUR_MOVE_HOR_ABS, m_start_column);
	printf(DEC_CORNER_BL DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR);
	for(size_t k=0; k<m_num_of_columns-2; k++) {
		std::cout << DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR;
	}
	printf(DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_CORNER_BR);

	std::cout << '\n';

	printf(ASCII_MODE);

}


void Grid::EndGameConditions(int x, int y) {

	EGridState grid_state = EGridState::NONE;

	auto RevealGrid = [&](){
		for(size_t i=0; i<m_num_of_rows; i++) {
			for(size_t j=0; j<m_num_of_columns; j++) {
				RevealCell(i, j);
			}
		}
	};

	if(m_cells[x][y].is_bomb) {
		RevealGrid();
		m_state = EGridState::LOST;
	} else {

		RevealCell(x, y);

		if(m_total_revealed == m_total_cells - m_total_bombs) {
			RevealGrid();	
			m_state = EGridState::WON;
		}
	}
}
