#include <iostream>
#include <random>
#include "VTS_table.h"
#include "Grid.h"

using namespace std::chrono_literals;

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

FCell::~FCell() {}

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

	m_startCounter = false;
	m_pauseCounter = false;
	m_state = EGridState::NONE;
	m_totalCells = num_of_rows * num_of_columns;
	m_totalRevealed = 0;
	m_numOfRows = num_of_rows;
	m_numOfColumns = num_of_columns;
	m_rowStride = 2;
	m_columnStride = 4;
	m_startRow = start_row;
	m_startColumn = start_column;
	m_topBorderSize = 1;
	m_leftBorderSize = 2;
	m_bombsRatio = bombs_ratio;
	m_totalBombs = ceil(m_totalCells * bombs_ratio);
	
	for(size_t i=0; i<num_of_rows; i++) {
		for(size_t j=0; j<num_of_columns; j++) {
			m_cells[i][j].screen_space_row = (i*m_rowStride)+start_row+m_topBorderSize;
			m_cells[i][j].screen_space_column = (j*m_columnStride)+start_column+m_leftBorderSize;
		}
	}

	for(size_t b=0; b<m_totalBombs; b++){
		
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

	m_cursorAndCounterUpdater = std::thread(
			&Grid::UpdateCursorAndCounter,
			this);
}


Grid::~Grid() {
	if(m_cursorAndCounterUpdater.joinable()) {
		m_state = EGridState::QUIT;
		m_cursorAndCounterUpdater.join();
	}
}


void Grid::RevealCell(int x, int y) {
	
	FCell &cell = m_cells[x][y];
	if(!cell.is_hidden) return;
	
	m_totalRevealed++;

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
		if(x == 0 && y == m_numOfColumns-1) {
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
		if(x == m_numOfRows-1 && y == 0) {
			RevealCell(x-1, y);
			RevealCell(x, y+1);
			RevealCell(x-1, y+1);
		}else
		// Bottom Right
		if(x == m_numOfRows-1 && y == m_numOfColumns-1) {
			RevealCell(x-1, y);
			RevealCell(x, y-1);
			RevealCell(x-1, y-1);
		}else
		// Bottom Row
		if(x == m_numOfRows-1) {
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
		if(y == m_numOfColumns-1) {
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

	printf(DEC_MODE);

	// HEADER
	m_headerWidth = 0;
	m_numOfColumns < 6 
		? m_headerWidth = 6 * m_columnStride
		: m_headerWidth = m_numOfColumns * m_columnStride;

	printf(DEC_CORNER_UL);
	for(size_t hs = 0; hs < m_headerWidth-1; hs++) { std::cout << DEC_LINE_HOR;	}
	printf(DEC_CORNER_UR);
	std::cout << "\n";

	printf("%-*s " COL_BF_RED "[%03d]" COL_DEFAULT "  %2s  " COL_BF_GREEN "[%03d]" COL_DEFAULT " %*s\n",
			m_numOfColumns < 6
			? 3
			: m_headerWidth/2 - 8,
			DEC_LINE_VERT, 
			m_totalBombs, 
			":)", 
			//m_currentPreset.m_isTimed ? m_currentPreset.m_timeElapsed : 999,
			false ? 000 : 999,
			m_numOfColumns < 6
			? 4
			: m_headerWidth/2 - 9,
			DEC_LINE_VERT
		  );

	printf(DEC_CORNER_BL);
	for(size_t hs = 0; hs < m_headerWidth-1; hs++) { std::cout << DEC_LINE_HOR; }
	printf(DEC_CORNER_BR);
	std::cout << '\n';

	// GRID
	printf(CUR_MOVE_TO, m_startRow, m_startColumn);
	
	// Top Rows divisor
	printf(DEC_CORNER_UL DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR);
	for(size_t k=0; k<m_numOfColumns-2; k++) {
		std::cout << DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR;
	}
	printf(DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_CORNER_UR);

	std::cout << '\n';
	for(size_t j=0; j<m_numOfRows-1; j++){				

		// Row
		printf(CUR_MOVE_HOR_ABS, m_startColumn);
		std::cout << DEC_LINE_VERT;
		for(size_t c=0; c<m_numOfColumns; c++) {
			std::cout << ' ' << m_cells[j][c] << " " DEC_LINE_VERT;
		}

		// Middle Rows divisor
		std::cout << '\n';
		printf(CUR_MOVE_HOR_ABS, m_startColumn);
		printf(DEC_LINE_VERT);
		for(size_t k=0; k<m_numOfColumns-1; k++) {
			std::cout << " " DEC_LINE_HOR "  ";
		}
		printf(" " DEC_LINE_HOR " " DEC_LINE_VERT);
		std::cout << '\n';
	}

	// Last Row
	printf(CUR_MOVE_HOR_ABS, m_startColumn);
	std::cout << DEC_LINE_VERT;
	for(size_t c=0; c<m_numOfColumns; c++) {
		std::cout << ' ' << m_cells[m_numOfRows-1][c] << " " DEC_LINE_VERT;
	}
	std::cout << '\n';
	
	// Bottom row divisor
	printf(CUR_MOVE_HOR_ABS, m_startColumn);
	printf(DEC_CORNER_BL DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR);
	for(size_t k=0; k<m_numOfColumns-2; k++) {
		std::cout << DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR;
	}
	printf(DEC_LINE_HOR DEC_LINE_HOR DEC_LINE_HOR DEC_CORNER_BR);

	std::cout << '\n';

	// FOOTER
	m_footerWidth = 0;
	m_numOfColumns < 6
		? m_footerWidth = 6 * m_columnStride + 3
		: m_footerWidth = (m_numOfColumns * m_columnStride) + 3;


	printf( DEC_LINE_VERT " %s %*s " DEC_LINE_VERT "\n",
			COL_F_YELLOW "[S]" COL_BF_YELLOW "SAVE" COL_DEFAULT,
			m_footerWidth,
			COL_F_YELLOW "[M]" COL_BF_YELLOW "MENU" COL_DEFAULT
		  );

	printf(ASCII_MODE);

}


void Grid::EndGameConditions(int x, int y) {

	EGridState grid_state = EGridState::NONE;

	auto RevealGrid = [&](){
		for(size_t i=0; i<m_numOfRows; i++) {
			for(size_t j=0; j<m_numOfColumns; j++) {
				RevealCell(i, j);
			}
		}
	};

	if(m_cells[x][y].is_bomb) {
		RevealGrid();
		m_state = EGridState::LOST;
	} else {

		RevealCell(x, y);

		if(m_totalRevealed == m_totalCells - m_totalBombs) {
			RevealGrid();	
			m_state = EGridState::WON;
		}
	}	

	// #TODO add stop counter inside here!!!
	auto message = [&](const char *msg) {
			this->StopCounter();
			int width = 0;
			m_numOfColumns < 6
				? width = 6 * m_columnStride 
				: width = m_numOfColumns * m_columnStride;

			printf(CUR_MOVE_TO_(2, 1));
			std::printf("%*s", width, " ");
			printf(CUR_MOVE_TO_(2, 1));
			std::printf("%-*s%s%*s\n",
					m_numOfColumns < 6
					? 8
					: width/2 - 4, 
					DEC_LINE_VERT, 
					msg, 
					m_numOfColumns < 6
					? 9
					: width/2 - 3, 
					DEC_LINE_VERT
					);
		};

		switch (m_state) {
			case EGridState::WON:
				message(WINDOW_TITLE("YOU WON!") COL_BF_GREEN "YOU WON!" COL_DEFAULT);
				break;
			case EGridState::LOST:
				message(WINDOW_TITLE("BOOOOOM!") COL_BF_RED "BOOOOOM!" COL_DEFAULT);
				break;
			case EGridState::QUIT:
			case EGridState::NONE:
			default:
				break;
		}


}

void Grid::StopCounter() {
	m_cursorAndCounterUpdater.join();
}

void Grid::UpdateCursorAndCounter() {

	bool flip_flop = true;
	while(m_state == EGridState::NONE) {
		if(m_pauseCounter) {
			std::cout << CUR_HIDE;
			continue;
		}
		printf("%s", flip_flop ? CUR_SHOW : CUR_HIDE);
		flip_flop = !flip_flop;
		std::this_thread::sleep_for(500ms);
		if(/*m_currentPreset.m_isTimed*/ true && m_startCounter && flip_flop) {
			std::cout << CUR_SAVE CUR_HIDE;
			printf(CUR_MOVE_TO COL_BF_GREEN "%03d" COL_DEFAULT, 
					2, 
					m_numOfColumns < 6
					? (m_headerWidth/2)+5
					: (m_headerWidth/2)+6, 
					//++m_currentPreset.m_timeElapsed
					0
					);

			std::cout << CUR_LOAD CUR_SHOW;
		}
	}
}
