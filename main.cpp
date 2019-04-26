#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>

// LIGHT NONE is white and DARK NONE is black, NORMAL NONE is undefined
enum Hue {
	RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, NONE
};

enum Lightness {
	LIGHT, NORMAL, DARK
};

enum Direction {
	RIGHT, DOWN, LEFT, UP
};

enum Chooser {
	LEFTMOST, RIGHTMOST
};

struct Color {
	Lightness lightness;
	Hue hue;
};

struct Position {
	int x;
	int y;
};

struct Block {
	Color color;
	unsigned long size;
	std::vector<Position> positions;
	struct Block** neighbors;
};

struct State {
	Block current;
	std::stack<int> stack;
	Direction dp = RIGHT;
	Chooser cc = LEFTMOST;
	uint8_t tries = 0;
};

bool operator==(const Position& p1, const Position& p2) {
	return p1.x == p2.x and p1.y == p2.y;
}

bool compare_x(const Position& p1, const Position& p2) {
	return p1.x < p2.x;
}

bool compare_y(const Position& p1, const Position& p2) {
	return p1.y < p2.y;
}

typedef void (* command)(State&);

void skip(State& state) {
	// ¯\_(ツ)_/¯
}

void push(State& state) {
	state.stack.push(state.current.size);
}

void pop(State& state) {
	state.stack.pop();
}

void add(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	state.stack.push(b + a);
}

void subtract(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	state.stack.push(b - a);
}

void multiply(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	state.stack.push(b * a);
}

void divide(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	state.stack.push(b / a);
}

void mod(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	state.stack.push(b % a);
}

void nott(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(!a);
}

void greater(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a > b);
}

void pointer(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.dp = static_cast<Direction>((state.dp + a) % 4);
}

void switchh(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.cc = static_cast<Chooser>((state.cc + a) % 4);
}

void duplicate(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a);
	state.stack.push(a);
}

void roll(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	int b = state.stack.top();
	state.stack.pop();
	
	std::stack<int> temp;
	
	for(int i = 0; i < a; i++) {
		int n = state.stack.top();
		state.stack.pop();
		
		for(int ii = 0; ii < b - 1; ii++) {
			temp.push(state.stack.top());
			state.stack.pop();
		}
		
		state.stack.push(n);
		
		while(!temp.empty()) {
			state.stack.push(temp.top());
			temp.pop();
		}
	}
}

void in_number(State& state) {
	int a;
	
	std::cin >> a;
	
	state.stack.push(a);
}

void in_char(State& state) {
	char a;
	
	std::cin >> a;
	
	state.stack.push(a);
}

void out_number(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	std::cout << a;
}

void out_char(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	std::cout << static_cast<char>(a);
}

const command commands[3][6] = {{skip, add,      divide, greater, duplicate, in_char},
								{push, subtract, mod,    pointer, roll,      out_number},
								{pop,  multiply, nott,   switchh, in_number, out_char}};

command get_command(const Block& from, const Block& to) {
	if((from.color.hue == NONE and from.color.lightness == LIGHT) or (to.color.hue == NONE and to.color.lightness == LIGHT)) {
		// Either going to or coming from a white Color Block
		return skip;
	}
	
	short hue_change = to.color.hue - from.color.hue + 6 % 6;
	short lightness_change = to.color.lightness - from.color.lightness + 3 % 3;
	
	return commands[hue_change][lightness_change];
}

void next_state(State& state) {
	Block next = *state.current.neighbors[state.dp * 4 + state.cc];
	
	if(next.color.hue == NONE && next.color.lightness == DARK) {
		// Bumped into black Block
		state.tries++;
		if((state.tries & 1u) == 0) state.dp = static_cast<Direction>((state.dp + 1) % 4);
		state.cc = static_cast<Chooser>((state.cc + 1) % 2);
	} else {
		// Perform operation associated with the Color transition
		get_command(state.current, next)(state);
		state.current = next;
	}
}

void expand(const std::vector<std::vector<Color>>& colors, std::vector<std::vector<bool>>& done, int x, int y, std::vector<Position>& positions) {
	const Color color = colors[x][y];
	
	positions.push_back({x, y});
	
	done[x][y] = true;
	
	if(x > 0 and !done[x - 1][y] and colors[x - 1][y].lightness == color.lightness &&
	   colors[x - 1][y].hue == color.hue) {
		expand(colors, done, x - 1, y, positions);
	}
	
	if(y > 0 and !done[x][y - 1] and colors[x][y - 1].lightness == color.lightness &&
	   colors[x][y - 1].hue == color.hue) {
		expand(colors, done, x, y - 1, positions);
	}
	
	if(x < 14 and !done[x + 1][y] and colors[x + 1][y].lightness == color.lightness &&
	   colors[x + 1][y].hue == color.hue) {
		expand(colors, done, x + 1, y, positions);
	}
	
	if(y < 5 and !done[x][y + 1] and colors[x][y + 1].lightness == color.lightness &&
	   colors[x][y + 1].hue == color.hue) {
		expand(colors, done, x, y + 1, positions);
	}
}

Block* pos2block(const Position& pos, std::vector<Block>& blocks) {
	for(auto& block : blocks) {
		if(std::find(block.positions.begin(), block.positions.end(), pos) != block.positions.end()) {
			return &block;
		}
	}
	
	return {};
}

Block load_image(const char* image) {
	
	// Read image
	
	unsigned char header[54];
	
	FILE* file = fopen(image, "rb");
	
	fread(header, sizeof(unsigned char), 54, file);
	
	int width = *(int*) &header[18];
	int height = *(int*) &header[22];
	int size = 3 * width * height;
	
	auto* data = new unsigned char[size];
	fread(data, sizeof(unsigned char), size, file);
	
	fclose(file);
	
	// Transform image to colors
	
	std::vector<std::vector<Color>> colors(width, std::vector<Color>(height));
	
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			int blue = data[3 * ((height - y - 1) * width + x)];
			int green = data[3 * ((height - y - 1) * width + x) + 1];
			int red = data[3 * ((height - y - 1) * width + x) + 2];
			
			switch(red) {
				case 0:
					switch(green) {
						case 0:
							switch(blue) {
								case 0:
									colors[x][y] = {DARK, NONE};
									break;
								case 192:
									colors[x][y] = {DARK, BLUE};
									break;
								default:
									colors[x][y] = {NORMAL, BLUE};
							}
							break;
						case 192:
							if(blue == 0) {
								colors[x][y] = {DARK, GREEN};
							} else {
								colors[x][y] = {DARK, CYAN};
							}
							break;
						default:
							if(blue == 0) {
								colors[x][y] = {NORMAL, GREEN};
							} else {
								colors[x][y] = {NORMAL, CYAN};
							}
					}
					break;
				case 192:
					switch(green) {
						case 0:
							if(blue == 0) {
								colors[x][y] = {DARK, RED};
							} else {
								colors[x][y] = {DARK, MAGENTA};
							}
							break;
						case 192:
							if(blue == 0) {
								colors[x][y] = {DARK, YELLOW};
							} else {
								colors[x][y] = {LIGHT, BLUE};
							}
							break;
						default:
							if(blue == 192) {
								colors[x][y] = {LIGHT, GREEN};
							} else {
								colors[x][y] = {LIGHT, CYAN};
							}
					}
					break;
				default:
					switch(green) {
						case 0:
							if(blue == 0) {
								colors[x][y] = {NORMAL, RED};
							} else {
								colors[x][y] = {NORMAL, MAGENTA};
							}
							break;
						case 192:
							if(blue == 192) {
								colors[x][y] = {LIGHT, RED};
							} else {
								colors[x][y] = {LIGHT, MAGENTA};
							}
							break;
						default:
							switch(blue) {
								case 0:
									colors[x][y] = {NORMAL, YELLOW};
									break;
								case 192:
									colors[x][y] = {LIGHT, YELLOW};
									break;
								default:
									colors[x][y] = {LIGHT, NONE};
							}
					}
			}
		}
	}
	
	delete[] data;
	
	// Find Color blocks
	
	std::vector<std::vector<bool>> done(width, std::vector<bool>(height, false));
	
	std::vector<Block> blocks;
	
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(colors[x][y].hue == NONE and colors[x][y].lightness == LIGHT) {
				// White pixel, should be a Color Block on its own
				std::vector<Position> positions;
				
				positions.push_back({x, y});
				
				done[x][y] = true;
				
				Block block = {colors[x][y], positions.size(), positions};
				
				blocks.push_back(block);
			} else if(!done[x][y]) {
				std::vector<Position> positions;
				
				expand(colors, done, x, y, positions);
				
				Block block = {colors[x][y], positions.size(), positions};
				
				blocks.push_back(block);
			}
		}
	}
	
	// Assign neighbors to all blocks
	
	for(auto block : blocks) {
		
		struct Block* neighbors[8];
		
		std::vector<Position> positions = block.positions;
		
		std::vector<Position> right;
		std::vector<Position> down;
		std::vector<Position> left;
		std::vector<Position> up;
		
		right.push_back(positions[0]);
		down.push_back(positions[0]);
		left.push_back(positions[0]);
		up.push_back(positions[0]);
		
		for(int i = 1; i < positions.size(); i++) {
			Position current = positions[i];
			
			if(right[0].x <= current.x) {
				if(right[0].x != current.x) right.clear();
				right.push_back(current);
			} else if(current.x <= left[0].x) {
				if(left[0].x != current.x) left.clear();
				left.push_back(current);
			}
			
			if(down[0].y <= current.y) {
				if(down[0].y != current.y) down.clear();
				down.push_back(current);
			} else if(current.y <= up[0].y) {
				if(up[0].y != current.y) up.clear();
				up.push_back(current);
			}
		}
		
		neighbors[0] = pos2block(*std::min_element(right.begin(), right.end(), &compare_y), blocks);
		neighbors[1] = pos2block(*std::max_element(right.begin(), right.end(), &compare_y), blocks);
		neighbors[2] = pos2block(*std::max_element(down.begin(), down.end(), &compare_x), blocks);
		neighbors[3] = pos2block(*std::min_element(down.begin(), down.end(), &compare_x), blocks);
		neighbors[4] = pos2block(*std::max_element(left.begin(), left.end(), &compare_y), blocks);
		neighbors[5] = pos2block(*std::min_element(left.begin(), left.end(), &compare_y), blocks);
		neighbors[6] = pos2block(*std::min_element(up.begin(), up.end(), &compare_x), blocks);
		neighbors[7] = pos2block(*std::max_element(up.begin(), up.end(), &compare_x), blocks);
		
		block.neighbors = neighbors;
	}
	
	// Return starting Block
	
	return blocks[0];
}

int main() {
	State state = {load_image("/home/rick/CLionProjects/piet/test.bmp")};
	
//	while(state.tries < 8) {
//		next_state(state);
//	}
	
	return 0;
}