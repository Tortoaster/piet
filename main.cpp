#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <cmath>

// LIGHT NONE is white and DARK NONE is black, NORMAL NONE is undefined
enum Hue {
	RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, NONE
};

enum Lightness {
	LIGHT, NORMAL, DARK
};

struct Color {
	Lightness lightness;
	Hue hue;
};

std::ostream& operator<<(std::ostream& os, const Color& color) {
	std::string a, b;
	
	switch(color.lightness) {
		case LIGHT:
			a = "LIGHT";
			break;
		case DARK:
			a = "DARK ";
			break;
		default:
			a = "     ";
	}
	
	switch(color.hue) {
		case RED:
			b = "    RED";
			break;
		case YELLOW:
			b =  "YELLOW";
			break;
		case GREEN:
			b = "  GREEN";
			break;
		case CYAN:
			b = "   CYAN";
			break;
		case BLUE:
			b = "   BLUE";
			break;
		case MAGENTA:
			b = "MAGENTA";
			break;
		default:
			a = "     ";
			b = color.lightness == LIGHT ? "WHITE  " : "BLACK  ";
	}
	
	os << a << " " << b;
	return os;
}

struct Position {
	int x;
	int y;
};

struct Block {
	Color color;
	std::vector<Position> positions;
	struct Block* neighbors[8];
};

struct State {
	Block current;
	std::stack<int> stack;
	short dp = 0;    // 0 is right, 1 is down, 2 is left, 3 is up
	short cc = 0;    // 0 is left, 1 is right
	short turned = 0;
	bool swapped = false;
};

bool operator==(const Position& p1, const Position& p2) {
	return p1.x == p2.x && p1.y == p2.y;
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
	int a = state.current.positions.size();
	
	state.stack.push(a);
	// std::cout << "push " << a << "; ";
}

void pop(State& state) {
	state.stack.pop();
	// std::cout << "pop; ";
}

void add(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a + b);
	// std::cout << "add " << a << " " << b << "; ";
}

void subtract(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a - b);
	// std::cout << "subtract " << a << " " << b << "; ";
}

void multiply(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a * b);
	// std::cout << "multiply " << a << " " << b << "; ";
}

void divide(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a / b);
	// std::cout << "divide " << a << " " << b << "; ";
}

void mod(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(((a % b) + b) % b);
	// std::cout << "mod " << a << " " << b << "; ";
}

void nott(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(!a);
	// std::cout << "not " << a << "; ";
}

void greater(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a > b);
	// std::cout << "greater " << a << " " << b << "; ";
}

void pointer(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.dp = (((state.dp + a) % 4) + 4) % 4;
	// std::cout << "pointer " << a << "; ";
}

void switchh(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.cc = (((state.cc + a) % 2) + 2) % 2;
	// std::cout << "switch " << a << "; ";
}

void duplicate(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	state.stack.push(a);
	state.stack.push(a);
	// std::cout << "duplicate " << a << "; ";
}

void roll(State& state) {
	int b = state.stack.top();
	state.stack.pop();
	
	int a = state.stack.top();
	state.stack.pop();
	
	std::stack<int> temp;
	
	for(int i = 0; i < b; i++) {
		int n = state.stack.top();
		state.stack.pop();
		
		for(int ii = 0; ii < a - 1; ii++) {
			temp.push(state.stack.top());
			state.stack.pop();
		}
		
		state.stack.push(n);
		
		while(!temp.empty()) {
			state.stack.push(temp.top());
			temp.pop();
		}
	}
	// std::cout << "roll " << a << " " << b << "; ";
}

void in_number(State& state) {
	int a;
	
	std::cin >> a;
	
	state.stack.push(a);
	// std::cout << "in " << a << "; ";
}

void in_char(State& state) {
	char a;
	
	std::cin >> a;
	
	state.stack.push(a);
	// std::cout << "in " << a << "; ";
}

void out_number(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	std::cout << a;
	// std::cout << "out " << a << "; ";
}

void out_char(State& state) {
	int a = state.stack.top();
	state.stack.pop();
	
	std::cout << static_cast<char>(a);
	// std::cout << "out " << a << "; ";
}

const command commands[3][6] = {{skip, add,      divide, greater, duplicate, in_char},
								{push, subtract, mod,    pointer, roll,      out_number},
								{pop,  multiply, nott,   switchh, in_number, out_char}};

const command& get_command(const Block& from, const Block& to) {
	if((from.color.hue == NONE and from.color.lightness == LIGHT) or (to.color.hue == NONE and to.color.lightness == LIGHT)) {
		// Either going to or coming from a white Color Block
		return commands[0][0];
	}
	
	short hue_change = (to.color.hue - from.color.hue + 6) % 6;
	short lightness_change = (to.color.lightness - from.color.lightness + 3) % 3;
	
	return commands[lightness_change][hue_change];
}

void next_state(State& state) {
	Block next = *state.current.neighbors[state.dp * 2 + state.cc];
	
	if(next.color.hue == NONE && next.color.lightness == DARK) {
		// Bumped into black block or fell off the edge
		if(state.swapped) {
			state.dp = (state.dp + 1) % 4;
			state.turned++;
		}
		
		state.cc = (state.cc + 1) % 2;
		state.swapped = !state.swapped;
	} else {
		// Perform operation associated with the color transition
		get_command(state.current, next)(state);
		state.current = next;
		
		state.turned = 0;
		state.swapped = false;
	}
}

void expand(const std::vector<std::vector<Color>>& colors, std::vector<std::vector<bool>>& done, int x, int y, std::vector<Position>& positions) {
	const Color color = colors[x][y];
	
	positions.push_back({x, y});
	
	done[x][y] = true;
	
	// White pixels should be color blocks on their own, even when their neighbors are also white, because that allows us to "slide across" them without adding code
	if(color.lightness != LIGHT || color.hue != NONE) {
		if(x > 0 && !done[x - 1][y] && colors[x - 1][y].lightness == color.lightness && colors[x - 1][y].hue == color.hue) {
			expand(colors, done, x - 1, y, positions);
		}
		
		if(y > 0 && !done[x][y - 1] and colors[x][y - 1].lightness == color.lightness && colors[x][y - 1].hue == color.hue) {
			expand(colors, done, x, y - 1, positions);
		}
		
		if(x < colors.size() - 1 && !done[x + 1][y] and colors[x + 1][y].lightness == color.lightness && colors[x + 1][y].hue == color.hue) {
			expand(colors, done, x + 1, y, positions);
		}
		
		if(y < colors[0].size() - 1 && !done[x][y + 1] and colors[x][y + 1].lightness == color.lightness && colors[x][y + 1].hue == color.hue) {
			expand(colors, done, x, y + 1, positions);
		}
	}
}

Block& find_block(const Position& pos, std::vector<Block>& blocks) {
	for(int i = 0; i < blocks.size() - 1; i++) {
		if(std::find(blocks[i].positions.begin(), blocks[i].positions.end(), pos) != blocks[i].positions.end()) {
			return blocks[i];
		}
	}
	
	return blocks.back();
}

std::vector<Block> load_image(const char* image, const int codel_size) {
	
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
	
	std::vector<std::vector<Color>> colors(width / codel_size, std::vector<Color>(height / codel_size));
	
	for(int y = 0; y < height / codel_size; y++) {
		for(int x = 0; x < width / codel_size; x++) {
			int blue = data[3 * ((height - y * codel_size - 1) * width + x * codel_size)];
			int green = data[3 * ((height - y * codel_size - 1) * width + x * codel_size) + 1];
			int red = data[3 * ((height - y * codel_size - 1) * width + x * codel_size) + 2];
			
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
	
	std::vector<std::vector<bool>> done(width / codel_size, std::vector<bool>(height / codel_size, false));
	
	std::vector<Block> blocks;
	
	for(int y = 0; y < height / codel_size; y++) {
		for(int x = 0; x < width / codel_size; x++) {
			if(!done[x][y]) {
				std::vector<Position> positions;
				
				expand(colors, done, x, y, positions);
				
				Block block = {colors[x][y], positions};
				
				blocks.push_back(block);
			}
		}
	}
	
	blocks.push_back({{DARK, NONE}});    // This black block will represent all edges of the program
	
	// Assign neighbors to all blocks
	
	for(int i = 0; i < blocks.size() - 1; i++) {
		
		std::vector<Position> right;
		std::vector<Position> down;
		std::vector<Position> left;
		std::vector<Position> up;
		
		for(auto & position : blocks[i].positions) {
			if(right.empty() || right[0].x <= position.x) {
				if(!right.empty() && right[0].x != position.x) right.clear();
				right.push_back(position);
			}
			
			if(down.empty() || down[0].y <= position.y) {
				if(!down.empty() && down[0].y != position.y) down.clear();
				down.push_back(position);
			}
			
			if(left.empty() || left[0].x >= position.x) {
				if(!left.empty() && left[0].x != position.x) left.clear();
				left.push_back(position);
			}
			
			if(up.empty() || up[0].y >= position.y) {
				if(!up.empty() && up[0].y != position.y) up.clear();
				up.push_back(position);
			}
		}
		
		blocks[i].neighbors[0] = &find_block({right[0].x + 1, (*std::min_element(right.begin(), right.end(), compare_y)).y}, blocks);
		blocks[i].neighbors[1] = &find_block({right[0].x + 1, (*std::max_element(right.begin(), right.end(), compare_y)).y}, blocks);
		blocks[i].neighbors[2] = &find_block({(*std::max_element(down.begin(), down.end(), compare_x)).x, down[0].y + 1}, blocks);
		blocks[i].neighbors[3] = &find_block({(*std::min_element(down.begin(), down.end(), compare_x)).x, down[0].y + 1}, blocks);
		blocks[i].neighbors[4] = &find_block({left[0].x - 1, (*std::max_element(left.begin(), left.end(), compare_y)).y}, blocks);
		blocks[i].neighbors[5] = &find_block({left[0].x - 1, (*std::min_element(left.begin(), left.end(), compare_y)).y}, blocks);
		blocks[i].neighbors[6] = &find_block({(*std::min_element(up.begin(), up.end(), compare_x)).x, up[0].y - 1}, blocks);
		blocks[i].neighbors[7] = &find_block({(*std::max_element(up.begin(), up.end(), compare_x)).x, up[0].y - 1}, blocks);
	}
	
	// Return starting Block
	
	return blocks;
}

int main() {
	const int codel_size = 1;
	const char* filename = "/home/rick/CLionProjects/piet/2xtPK.bmp";
	
	std::vector<Block> blocks = load_image(filename, codel_size);
	
	State state = {blocks.front()};

	while(state.turned < 4) {
		next_state(state);
	}
	
	return 0;
}