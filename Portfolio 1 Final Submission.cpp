#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

struct Position {
    int x;
    int y;

    bool operator==(const Position& other) const {
        return (x == other.x && y == other.y);
    }
};

class Guard {
private:
    Position pos;
    char facing; 

public:
    Guard(int start_x, int start_y, char start_facing) 
        : pos{start_x, start_y}, facing{start_facing} {}

    Position getPosition() const { return pos; }
    char getFacing() const { return facing; }

    Position getMovementVector() const {
        if (facing == '>') return Position{1, 0};
        if (facing == '<') return Position{-1, 0};
        if (facing == '^') return Position{0, -1};
        if (facing == 'v') return Position{0, 1};
        return Position{0, 0};
    }

    void flipDirection() {
        if (facing == '>') facing = '<';
        else if (facing == '<') facing = '>';
        else if (facing == '^') facing = 'v';
        else if (facing == 'v') facing = '^';
    }

    void move(Position newPos) {
        pos = newPos;
    }
};

class Level {
private:
    std::string name;
    std::vector<std::string> grid;
    Position playerPos;
    Position goalPos;
    std::vector<Guard> guards;
    bool isLoaded;

public:
    Level() : name(""), isLoaded(false) {}

    Level(std::string lvl_name, std::vector<std::string> lvl_grid) 
        : name(lvl_name), grid(lvl_grid), isLoaded(true) {
        parseGridEntities();
    }

    std::string getName() const { return name; }
    bool isValid() const { return isLoaded; }

    void parseGridEntities() {
        guards.clear();
        for (int y = 0; y < grid.size(); ++y) {
            for (int x = 0; x < grid[y].size(); ++x) {
                char cell = grid[y][x];
                if (cell == '@') {
                    playerPos = Position{x, y};
                    grid[y][x] = ' '; 
                } else if (cell == '$') {
                    goalPos = Position{x, y};
                    grid[y][x] = ' '; 
                } else if (cell == '<' || cell == '>' || cell == '^' || cell == 'v') {
                    guards.push_back(Guard(x, y, cell));
                    grid[y][x] = ' '; 
                }
            }
        }
    }

    bool isWall(Position p) const {
        if (p.y < 0 || p.y >= grid.size() || p.x < 0 || p.x >= grid[p.y].size()) {
            return true;
        }
        return grid[p.y][p.x] == '#';
    }

    bool checkGuardVision() {
        for (const auto& guard : guards) {
            Position dir = guard.getMovementVector();
            Position check = guard.getPosition();

            while (true) {
                check.x += dir.x;
                check.y += dir.y;

                if (isWall(check) || check == goalPos) {
                    break; 
                }

                bool guardBlocked = false;
                for (const auto& g : guards) {
                    if (g.getPosition() == check) {
                        guardBlocked = true;
                        break;
                    }
                }
                if (guardBlocked) break;

                if (check == playerPos) {
                    return true; 
                }
            }
        }
        return false;
    }

    void updateGuards() {
        for (auto& guard : guards) {
            Position dir = guard.getMovementVector();
            Position nextPos = Position{guard.getPosition().x + dir.x, guard.getPosition().y + dir.y};

            if (isWall(nextPos) || nextPos == goalPos) {
                guard.flipDirection();
                dir = guard.getMovementVector();
                nextPos = Position{guard.getPosition().x + dir.x, guard.getPosition().y + dir.y};
            }

            if (!isWall(nextPos) && !(nextPos == goalPos)) {
                guard.move(nextPos);
            }
        }
    }

    void display() {
        std::vector<std::string> displayGrid = grid;
        
        displayGrid[goalPos.y][goalPos.x] = '$';
        for (const auto& guard : guards) {
            Position gp = guard.getPosition();
            displayGrid[gp.y][gp.x] = guard.getFacing();
        }
        displayGrid[playerPos.y][playerPos.x] = '@';

        std::cout << "\n";
        for (const auto& row : displayGrid) {
            std::cout << row << "\n";
        }
    }

    void play() {
        while (true) {
            display();

            if (checkGuardVision()) {
                std::cout << "\nSpotted! Game Over!\n";
                return;
            }

            std::cout << "Move (W/A/S/D): ";
            char moveChar;
            std::cin >> moveChar;
            moveChar = std::toupper(moveChar);

            Position nextPlayerPos = playerPos;
            if (moveChar == 'W') nextPlayerPos.y--;
            else if (moveChar == 'S') nextPlayerPos.y++;
            else if (moveChar == 'A') nextPlayerPos.x--;
            else if (moveChar == 'D') nextPlayerPos.x++;
            else {
                std::cout << "Invalid key!\n";
                continue;
            }

            if (isWall(nextPlayerPos)) {
                std::cout << "Blocked by a wall!\n";
                continue; 
            }

            playerPos = nextPlayerPos;

            if (playerPos == goalPos) {
                display();
                std::cout << "\nYou Win!\n";
                return;
            }

            for (const auto& guard : guards) {
                if (playerPos == guard.getPosition()) {
                    std::cout << "\nCaught by a guard! Game Over!\n";
                    return;
                }
            }

            updateGuards();

            for (const auto& guard : guards) {
                if (playerPos == guard.getPosition()) {
                    std::cout << "\nA guard ran into you! Game Over!\n";
                    return;
                }
            }
        }
    }
};

std::string toLowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return str;
}

int main() {
    std::vector<Level> levels;

    levels.push_back(Level("A New Enemy", {
        "#########",
        "##@      #",
        "##  >  ###",
        "##      $#",
        "#########"
    }));

    levels.push_back(Level("The Truth of the Weapon", {
        "#############",
        "##@         #",
        "##### # # # #",
        "##   v  # # #",
        "##      # #$#",
        "#############"
    }));

    levels.push_back(Level("Revenge of the General", {
        "#############",
        "##@  >       #",
        "### # ### ####",
        "### # # v # #",
        "####$#   ####",
        "###         #",
        "#############"
    }));

    while (true) {
        std::cout << "Welcome to Ultra-Spy!\n";
        std::cout << "Select a level:\n";
        std::cout << " 1) " << levels[0].getName() << "\n";
        std::cout << " 2) " << levels[1].getName() << "\n";
        std::cout << " 3) " << levels[2].getName() << "\n";
        std::cout << "Type 'exit' to quit.\n";
        std::cout << "\nEnter choice: ";

        std::string choice;
        std::cin >> std::ws; 
        std::getline(std::cin, choice);

        std::string lowerChoice = toLowerCase(choice);
        if (lowerChoice == "exit") {
            break;
        }

        Level selectedLevel;
        
        if (choice == "1" || lowerChoice == "1) a new enemy" || lowerChoice == toLowerCase(levels[0].getName())) {
            selectedLevel = levels[0];
        } else if (choice == "2" || lowerChoice == "2) the truth of the weapon" || lowerChoice == toLowerCase(levels[1].getName())) {
            selectedLevel = levels[2]; // Points directly to the expanded level array index
            selectedLevel = levels[1];
        } else if (choice == "3" || lowerChoice == "3) revenge of the general" || lowerChoice == toLowerCase(levels[2].getName())) {
            selectedLevel = levels[2];
        }

        if (selectedLevel.isValid()) {
            selectedLevel.play();
        } else {
            std::cout << "\nInvalid choice!\n\n";
        }
    }

    return 0;
}