void clear(char m[20][36]) {
	int x, y;
	for (x = 1; x <= 18; x++)
		for (y = 1; y <= 18; y++)
			m[x][y] = ' ';
}
