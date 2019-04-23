#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <map>

enum hue {RED, YELLOW, GREEN, CYAN, BLUE, VIOLET, NONE};
enum lightness {LIGHT, NORMAL, DARK};
enum direction {RIGHT, DOWN, LEFT, UP};
enum chooser {LEFTMOST, RIGHTMOST};

struct color {
    hue hue;
    lightness lightness;
};

struct position {
    int x;
    int y;
};

struct stain {
    color color;
    std::vector<position> coordinates;
};

struct codel {
    color color;
    unsigned int size;
    struct codel* neighbors[8];
};

struct state {
    codel current;
    std::stack<int> stack;
    direction dp = RIGHT;
    chooser cc = LEFTMOST;
};

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

command get_command(const codel & from, const codel & to) {
    if((from.color.hue == NONE and from.color.lightness == LIGHT) or (to.color.hue == NONE and to.color.lightness == LIGHT)) {  // Either from or to is white
        return skip;
    }

    short hue_change = to.color.hue - from.color.hue + 6 % 6;
    short lightness_change = to.color.lightness - from.color.lightness + 3 % 3;

    return commands[hue_change][lightness_change];
}

void next_state(state & state) {
    codel next = *state.current.neighbors[state.dp * 4 + state.cc];

    if(next.color.hue == NONE && next.color.lightness == DARK) {    // Bumped into black codel
        state.dp = static_cast<direction>(state.dp + 1 % 4);
        state.cc = static_cast<chooser>(state.cc + 1 % 2);
    } else {
        get_command(state.current, next) (state);       // Perform operation
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

codel find_codels(const std::string & image) {

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

    std::vector<stain> stains;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(!done[x][y]) {
                std::vector<position> positions;

                expand(colors, done, x, y, positions);

                stain stain = {colors[x][y], positions};
            }
        }
    }

    // Transform color blocks to codels

    std::map<stain, codel> codelmap;
    std::vector<codel> codels;

    for(auto & stain : stains) {
        codel codel = {stain.color, static_cast<unsigned int>(stain.coordinates.size())};
        codelmap[stain] = codel;
        codels.push_back(codel);
    }

    // Find neighboring codels

    for(int i = 0; i < stains.size(); i++) {
        stain stain = stains.at(i);

        struct codel (& neighbors)[8];

        // Right

        std::vector<position> pixels;
        pixels.push_back(stain.coordinates[0]);
        for(int n = 1; n < stain.coordinates.size(); n++) {
            int current = pixels[0].x;

            if(current >= stain.coordinates[n].x) {
                if(current != stain.coordinates[n].x) {
                    pixels.clear();
                }

                pixels.push_back(stain.coordinates[n]);
            }
        }

        int min = pixels[0].y;
        int max = pixels[0].y;
        for(int n = 1; n < pixels.size(); n++) {
            if(pixels[n].y < min) min = pixels[n].y;
            if(pixels[n].y > max) max = pixels[n].y;
        }

        for(int n = 0; n < stains.size(); n++) {
            if(std::find(stains[n].coordinates.begin(), stains[n].coordinates.end(), {pixels[0].x + 1, min}) != stains[n].coordinates.end()) {
                neighbors[0] = & codelmap.at(stains[n]);
            } else if(std::find(stains[n].coordinates.begin(), stains[n].coordinates.end(), {pixels[0].x + 1, max}) != stains[n].coordinates.end()) {
                neighbors[1] = & codelmap.at(stains[n]);
            }
        }

        // Down

        // Left

        // Up

        codelmap.at(stain).neighbors = neighbors;
    }

    // Return starting codel

    return codels[0];
}

int main() {
    state state = {find_codels("lol")};

    while(true) {
        next_state(state);
    }

    return 0;
}