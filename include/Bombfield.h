#pragma once

class Grid;
class Menu;

void PoolInputs(
		Menu &menu, 
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
	
