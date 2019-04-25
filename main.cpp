#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <map>

enum hue {RED, YELLOW, GREEN, CYAN, BLUE, VIOLET, NONE};    // LIGHT NONE is white and DARK NONE is black, NORMAL NONE is undefined
enum lightness {LIGHT, NORMAL, DARK};
enum direction {RIGHT, DOWN, LEFT, UP};
enum chooser {LEFTMOST, RIGHTMOST};

struct color {
    hue hue;
    lightness lightness;
};

struct block {
    color color;
    unsigned long size;
    struct block* neighbors[8];
};

struct state {
    block current;
    std::stack<int> stack;
    direction dp = RIGHT;
    chooser cc = LEFTMOST;
    uint8_t tries = 0;
};

struct position {
    int x;
    int y;
};

bool compare_x(const position & p1, const position & p2) {
    return p1.x < p2.x;
}

bool compare_y(const position & p1, const position & p2) {
    return p1.y < p2.y;
}

typedef void (*command) (state&);

void skip(state & state) {
    // ¯\_(ツ)_/¯
}

void push(state & state) {
    state.stack.push(state.current.size);
}

void pop(state & state) {
    state.stack.pop();
}

void add(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    state.stack.push(b + a);
}

void subtract(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    state.stack.push(b - a);
}

void multiply(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    state.stack.push(b * a);
}

void divide(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    state.stack.push(b / a);
}

void mod(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    state.stack.push(b % a);
}

void nott(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    state.stack.push(!a);
}

void greater(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    state.stack.push(a > b);
}

void pointer(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    state.dp = static_cast<direction>(state.dp + a % 4);
}

void switchh(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    state.cc = static_cast<chooser>(state.cc + a % 4);
}

void duplicate(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    state.stack.push(a);
    state.stack.push(a);
}

void roll(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    int b = state.stack.top();
    state.stack.pop();

    for(int i = 0; i < a; i++) {
        int n = state.stack.top();
        state.stack.pop();

        // Should probably finish this function
    }
}

void in_number(state & state) {
    int a;

    std::cin >> a;

    state.stack.push(a);
}

void in_char(state & state) {
    char a;

    std::cin >> a;

    state.stack.push(a);
}

void out_number(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    std::cout << a;
}

void out_char(state & state) {
    int a = state.stack.top();
    state.stack.pop();

    std::cout << static_cast<char>(a);
}

const command commands[3][6] = {{skip, add, divide, greater, duplicate, in_char}, {push, subtract, mod, pointer, roll, out_number}, {pop, multiply, nott, switchh, in_number, out_char}};

command get_command(const block & from, const block & to) {
    if((from.color.hue == NONE and from.color.lightness == LIGHT) or (to.color.hue == NONE and to.color.lightness == LIGHT)) {  // Either going to or coming from a white color block
        return skip;
    }

    short hue_change = to.color.hue - from.color.hue + 6 % 6;
    short lightness_change = to.color.lightness - from.color.lightness + 3 % 3;

    return commands[hue_change][lightness_change];
}

void next_state(state & state) {
    block next = *state.current.neighbors[state.dp * 4 + state.cc];

    if(next.color.hue == NONE && next.color.lightness == DARK) {    // Bumped into black block
        state.tries++;
        if((state.tries & 1) == 0) state.dp = static_cast<direction>(state.dp + 1 % 4);
        state.cc = static_cast<chooser>(state.cc + 1 % 2);
    } else {
        get_command(state.current, next) (state);       // Perform operation associated with the color transition
        state.current = next;
    }
}

void expand(const color (& colors)[6][15], bool (& done)[6][15], int x, int y, std::vector<position> & positions) {
    const color color = colors[x][y];

    positions.push_back({x, y});

    done[x][y] = true;

    if(x > 0 and !done[x - 1][y] and colors[x - 1][y].lightness == color.lightness && colors[x - 1][y].hue == color.hue) {
        expand(colors, done, x - 1, y, positions);
    }

    if(y > 0 and !done[x][y - 1] and colors[x][y - 1].lightness == color.lightness && colors[x][y - 1].hue == color.hue) {
        expand(colors, done, x, y - 1, positions);
    }

    if(x < 14 and !done[x + 1][y] and colors[x + 1][y].lightness == color.lightness && colors[x + 1][y].hue == color.hue) {
        expand(colors, done, x + 1, y, positions);
    }

    if(y < 5 and !done[x][y + 1] and colors[x][y + 1].lightness == color.lightness && colors[x][y + 1].hue == color.hue) {
        expand(colors, done, x, y + 1, positions);
    }
}

block find_blocks(const std::string &image) {

    // Read image

    const int width = 6, height = 15;

    // Transform image to colors

    color colors[width][height] = {{{RED, NORMAL}, {BLUE, LIGHT}, {GREEN, LIGHT}, {GREEN, NORMAL}, {GREEN, DARK}, {CYAN, LIGHT}, {CYAN, NORMAL}, {CYAN, DARK}, {YELLOW, LIGHT}, {BLUE, LIGHT}, {YELLOW, NORMAL}, {NONE, LIGHT}, {YELLOW, NORMAL}, {NONE, DARK}, {NONE, DARK}},
                            {{NONE, DARK}, {NONE, DARK}, {BLUE, LIGHT}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {VIOLET, NORMAL}, {NONE, DARK}, {NONE, LIGHT}, {NONE, DARK}, {NONE, DARK}},
                            {{NONE, DARK}, {NONE, DARK}, {BLUE, NORMAL}, {CYAN, LIGHT}, {CYAN, DARK}, {CYAN, NORMAL}, {GREEN, LIGHT}, {BLUE, DARK}, {BLUE, NORMAL}, {BLUE, LIGHT}, {BLUE, DARK}, {NONE, DARK}, {NONE, LIGHT}, {NONE, DARK}, {NONE, DARK}},
                            {{NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, LIGHT}, {NONE, DARK}, {NONE, DARK}},
                            {{NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {YELLOW, NORMAL}, {YELLOW, NORMAL}, {YELLOW, NORMAL}, {NONE, DARK}},
                            {{NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}, {NONE, DARK}}};

    // Find color blocks

    bool done[width][height] = {false};

    std::vector<block> blocks;
    std::map<position, block> posToBlock;
    std::map<block, std::vector<position>> blockToPos;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(colors[x][y].hue == NONE and colors[x][y].lightness == LIGHT) {  // White pixel, should be a color block on its own
                std::vector<position> positions;

                positions.push_back({x, y});

                done[x][y] = true;

                block block = {colors[x][y], positions.size()};

                blocks.push_back(block);

                posToBlock[{x, y}] = block;
                blockToPos[block] = positions;
            } else if(!done[x][y]) {
                std::vector<position> positions;

                expand(colors, done, x, y, positions);

                block block = {colors[x][y], positions.size()};

                blocks.push_back(block);

                for(auto position : positions) {
                    posToBlock[position] = block;
                }
                blockToPos[block] = positions;
            }
        }
    }

    // Find neighboring blocks

    for(auto block : blocks) {

        struct block (& neighbors)[8];

        std::vector<position> positions = blockToPos[block];

        std::vector<position> right;
        std::vector<position> down;
        std::vector<position> left;
        std::vector<position> up;

        right.push_back(positions[0]);
        down.push_back(positions[0]);
        left.push_back(positions[0]);
        up.push_back(positions[0]);

        for(int i = 1; i < positions.size(); i++) {
            position current = positions[i];

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

        neighbors[0] = posToBlock.at(*std::min_element(right.begin(), right.end(), & compare_y));
        neighbors[1] = posToBlock.at(*std::max_element(right.begin(), right.end(), & compare_y));
        neighbors[2] = posToBlock.at(*std::max_element(down.begin(), down.end(), & compare_x));
        neighbors[3] = posToBlock.at(*std::min_element(down.begin(), down.end(), & compare_x));
        neighbors[4] = posToBlock.at(*std::max_element(left.begin(), left.end(), & compare_y));
        neighbors[5] = posToBlock.at(*std::min_element(left.begin(), left.end(), & compare_y));
        neighbors[6] = posToBlock.at(*std::min_element(up.begin(), up.end(), & compare_x));
        neighbors[7] = posToBlock.at(*std::max_element(up.begin(), up.end(), & compare_x));
    }

    // Return starting block

    return blocks[0];
}

int main() {
    state state = {find_blocks("lol")};

    while(state.tries < 8) {
        next_state(state);
    }

    return 0;
}