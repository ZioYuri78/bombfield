#include <iostream>
#include <ostream>
#include <random>
#include "VTS_table.h"
#include "Grid.h"

using namespace std::chrono_literals;

// =================== //
// === Single cell === //
// =================== //
FCell::FCell() {
	m_screenSpaceRow = -1;
	m_screenSpaceColumn = -1;
	m_nearBombs = 0;
	m_isHidden = true; 
	m_isBomb = false;
}

FCell::~FCell() {}

std::ostream& operator<<(std::ostream& _os, const FCell &_cell) {

	if(!_cell.m_isHidden) {
		if(_cell.m_isBomb) {
			_os << COL_BF_RED "*" COL_DEFAULT;
		} else if(_cell.m_nearBombs > 0) {
			_os << COL_BF_YELLOW << _cell.m_nearBombs << COL_DEFAULT; 
		} else {
			_os << COL_B_BLACK " " COL_DEFAULT;
		}
	} else {
		_os << COL_B_WHITE " " COL_DEFAULT;
	}

	return _os;
}




// ============ // 
// === Grid === //
// ============ //
Grid::Grid(
		int _startRow /*= 1*/, 
		int _startColumn /*= 1*/, 
		int _numOfRows /*= 5*/, 
		int _numOfColumns /*= 5*/, 
		float _bombsRatio /*= 0.1*/, 
		bool _hasTimer /*= false*/, 
		int _timeElapsed /* = 999*/
		) {
	
	std::random_device rd;
	std::mt19937 gen32(rd());

	m_state = EGridState::NONE;
	m_totalCells = _numOfRows * _numOfColumns;
	m_totalRevealed = 0;
	m_numOfRows = _numOfRows;
	m_numOfColumns = _numOfColumns;
	m_rowStride = 2;
	m_columnStride = 4;
	m_startRow = _startRow;

	m_startColumn = _numOfColumns < 6 ? 3 : _startColumn;
	
	m_topBorderSize = 1;
	m_leftBorderSize = 2;
	m_bombsRatio = _bombsRatio;
	m_totalBombs = ceil(m_totalCells * _bombsRatio);
	m_hasTimer = _hasTimer;
	m_timeElapsed = _timeElapsed;
	m_startCounter = false;
	m_pauseCursorAndCounter = true;
	
	for(size_t i=0; i<_numOfRows; i++) {
		for(size_t j=0; j<_numOfColumns; j++) {
			m_cells[i][j].m_screenSpaceRow = (i*m_rowStride)+_startRow+m_topBorderSize;
			m_cells[i][j].m_screenSpaceColumn = (j*m_columnStride)+_startColumn+m_leftBorderSize;
		}
	}

	for(size_t b=0; b<m_totalBombs; b++){
		
		int x, y;
		while(true) {
			x = gen32()%_numOfRows;
			y = gen32()%_numOfColumns;
			if(!m_cells[x][y].m_isBomb) {
				break;
			}
		}

		m_cells[x][y].m_isBomb = true;
		
		// Top Left
		if(x == 0 && y == 0) {
			m_cells[x+1][y].m_nearBombs++;
			m_cells[x][y+1].m_nearBombs++;
			m_cells[x+1][y+1].m_nearBombs++;
		}else 
		// Top Right
		if(x == 0 && y == _numOfColumns-1) {
			m_cells[x+1][y].m_nearBombs++;
			m_cells[x][y-1].m_nearBombs++;
			m_cells[x+1][y-1].m_nearBombs++;
		}else
		// Top Row
		if(x == 0) {
			m_cells[x][y-1].m_nearBombs++;
			m_cells[x][y+1].m_nearBombs++;
			m_cells[x+1][y-1].m_nearBombs++;
			m_cells[x+1][y].m_nearBombs++;
			m_cells[x+1][y+1].m_nearBombs++;
		}else
		// Bottom Left
		if(x == _numOfRows-1 && y == 0) {
			m_cells[x-1][y].m_nearBombs++;
			m_cells[x][y+1].m_nearBombs++;
			m_cells[x-1][y+1].m_nearBombs++;
		}else
		// Bottom Right
		if(x == _numOfRows-1 && y == _numOfColumns-1) {
			m_cells[x-1][y].m_nearBombs++;
			m_cells[x][y-1].m_nearBombs++;
			m_cells[x-1][y-1].m_nearBombs++;
		}else
		// Bottom Row
		if(x == _numOfRows-1) {
			m_cells[x][y-1].m_nearBombs++;
			m_cells[x][y+1].m_nearBombs++;
			m_cells[x-1][y-1].m_nearBombs++;
			m_cells[x-1][y].m_nearBombs++;
			m_cells[x-1][y+1].m_nearBombs++;
		}else
		// Left Column
		if(y == 0) {
			m_cells[x-1][y].m_nearBombs++;
			m_cells[x+1][y].m_nearBombs++;
			m_cells[x-1][y+1].m_nearBombs++;
			m_cells[x][y+1].m_nearBombs++;
			m_cells[x+1][y+1].m_nearBombs++;
		}else
		// Right Column
		if(y == _numOfColumns-1) {
			m_cells[x-1][y].m_nearBombs++;
			m_cells[x+1][y].m_nearBombs++;
			m_cells[x-1][y-1].m_nearBombs++;
			m_cells[x][y-1].m_nearBombs++;
			m_cells[x+1][y-1].m_nearBombs++;
		} else {
			// If none of above the bomb is in the middle
			m_cells[x-1][y-1].m_nearBombs++;
			m_cells[x-1][y].m_nearBombs++;
			m_cells[x-1][y+1].m_nearBombs++;
			m_cells[x][y-1].m_nearBombs++;
			m_cells[x][y+1].m_nearBombs++;
			m_cells[x+1][y-1].m_nearBombs++;
			m_cells[x+1][y].m_nearBombs++;
			m_cells[x+1][y+1].m_nearBombs++;
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


void Grid::RevealCell(int _x, int _y) {
	
	FCell &cell = m_cells[_x][_y];
	if(!cell.m_isHidden) return;
	
	m_totalRevealed++;

	cell.m_isHidden = false;
	printf(CUR_MOVE_TO, cell.m_screenSpaceRow, cell.m_screenSpaceColumn);
	std::cout << cell;

	if(cell.m_nearBombs == 0) {
		
		// Top Left
		if(_x == 0 && _y == 0) {
			RevealCell(_x+1, _y);
			RevealCell(_x, _y+1);
			RevealCell(_x+1, _y+1);
		}else 
		// Top Right
		if(_x == 0 && _y == m_numOfColumns-1) {
			RevealCell(_x+1, _y);
			RevealCell(_x, _y-1);
			RevealCell(_x+1, _y-1);
		}else
		// Top Row
		if(_x == 0) {
			RevealCell(_x, _y-1);
			RevealCell(_x, _y+1);
			RevealCell(_x+1, _y-1);
			RevealCell(_x+1, _y);
			RevealCell(_x+1, _y+1);
		}else
		// Bottom Left
		if(_x == m_numOfRows-1 && _y == 0) {
			RevealCell(_x-1, _y);
			RevealCell(_x, _y+1);
			RevealCell(_x-1, _y+1);
		}else
		// Bottom Right
		if(_x == m_numOfRows-1 && _y == m_numOfColumns-1) {
			RevealCell(_x-1, _y);
			RevealCell(_x, _y-1);
			RevealCell(_x-1, _y-1);
		}else
		// Bottom Row
		if(_x == m_numOfRows-1) {
			RevealCell(_x, _y-1);
			RevealCell(_x, _y+1);
			RevealCell(_x-1, _y-1);
			RevealCell(_x-1, _y);
			RevealCell(_x-1, _y+1);
		}else
		// Left Column
		if(_y == 0) {
			RevealCell(_x-1, _y);
			RevealCell(_x+1, _y);
			RevealCell(_x-1, _y+1);
			RevealCell(_x, _y+1);
			RevealCell(_x+1, _y+1);
		}else
		// Right Column
		if(_y == m_numOfColumns-1) {
			RevealCell(_x-1, _y);
			RevealCell(_x+1, _y);
			RevealCell(_x-1, _y-1);
			RevealCell(_x, _y-1);
			RevealCell(_x+1, _y-1);
		} else {
			// Middle
			RevealCell(_x-1, _y-1);
			RevealCell(_x-1, _y);
			RevealCell(_x-1, _y+1);
			RevealCell(_x, _y-1);
			RevealCell(_x, _y+1);
			RevealCell(_x+1, _y-1);
			RevealCell(_x+1, _y);
			RevealCell(_x+1, _y+1);
		}
	}
}


void Grid::Draw() {

	printf(DEC_MODE);

	m_pauseCursorAndCounter = false;

	// HEADER
	m_outerWidth = 0;
	m_numOfColumns < 6 
		? m_outerWidth = 6 * m_columnStride
		: m_outerWidth = m_numOfColumns * m_columnStride;

	printf(DEC_CORNER_UL);
	for(size_t hs = 0; hs < m_outerWidth-1; hs++) { std::cout << DEC_LINE_HOR;	}
	printf(DEC_CORNER_UR);
	std::cout << "\n";

	printf("%-*s " COL_BF_RED "[%03d]" COL_DEFAULT "  %2s  " COL_BF_GREEN "[%03d]" COL_DEFAULT " %*s\n",
			m_numOfColumns < 6
			? 3
			: m_outerWidth/2 - 8,
			DEC_LINE_VERT, 
			m_totalBombs, 
			":)", 
			m_hasTimer ? m_timeElapsed : 999,
			m_numOfColumns < 6
			? 4
			: m_outerWidth/2 - 9,
			DEC_LINE_VERT
		  );

	printf(DEC_CORNER_BL);
	for(size_t hs = 0; hs < m_outerWidth-1; hs++) { std::cout << DEC_LINE_HOR; }
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
	m_outerWidth = 0;
	m_numOfColumns < 6
		? m_outerWidth = 6 * m_columnStride + 3
		: m_outerWidth = (m_numOfColumns * m_columnStride) + 3;


	printf( DEC_LINE_VERT " %s %*s " DEC_LINE_VERT "\n",
			COL_F_YELLOW "[S]" COL_BF_YELLOW "SAVE" COL_DEFAULT,
			m_outerWidth,
			COL_F_YELLOW "[M]" COL_BF_YELLOW "MENU" COL_DEFAULT
		  );

	printf(ASCII_MODE);

}


void Grid::EndGameConditions(int _x, int _y) {

	_x = (_x - m_startRow)/m_rowStride;
	_y = (_y - m_startColumn)/m_columnStride;

	EGridState grid_state = EGridState::NONE;

	auto RevealGrid = [&](){
		for(size_t i=0; i<m_numOfRows; i++) {
			for(size_t j=0; j<m_numOfColumns; j++) {
				RevealCell(i, j);
			}
		}
	};

	if(m_cells[_x][_y].m_isBomb) {
		RevealGrid();
		m_state = EGridState::LOST;
	} else {

		RevealCell(_x, _y);

		if(m_totalRevealed == m_totalCells - m_totalBombs) {
			RevealGrid();	
			m_state = EGridState::WON;
		}
	}	

	auto message = [&](const char *_msg) {
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
					_msg, 
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
		if(m_pauseCursorAndCounter) {
			std::cout << CUR_HIDE;
			continue;
		}
		printf("%s", flip_flop ? CUR_SHOW : CUR_HIDE);
		flip_flop = !flip_flop;
		std::this_thread::sleep_for(500ms);
		if(m_hasTimer && m_startCounter && flip_flop) {
			std::cout << CUR_SAVE CUR_HIDE;
			printf(CUR_MOVE_TO COL_BF_GREEN "%03d" COL_DEFAULT, 
					2, 
					m_numOfColumns < 6
					? (m_outerWidth/2)+4
					: (m_outerWidth/2)+5, 
					++m_timeElapsed
					);

			std::cout << CUR_LOAD CUR_SHOW;
		}
	}
}
