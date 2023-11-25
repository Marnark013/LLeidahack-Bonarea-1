#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

using VI = std::vector<int>;
using VVI = std::vector<VI>;
using VS = std::vector<std::string>;
using VVS = std::vector<VS>;
using VB = std::vector<bool>;
using VVB = std::vector<VB>;

VVS boardInfo = VVS(20, VS(47, ""));
VVI passableBoard = VVI(20, VI(47, true));

struct Pos {
    int x = -1;
    int y = -1;

    Pos(int px, int py) : x(px), y(py) {}
};

struct itemInfo {
    VI pickingTimes = VI(5, 0);
    std::string itemName = " ";

};

void readBoard(const std::string& filename) {
    std::ifstream file;
    file.open(filename);
    if(file.fail()) {
        std::cerr << "No s'ha obert" << std::endl;
        return;
    }
    std::string header, line;
    getline(file, header);
    while(getline(file, line)) {
        std::istringstream linestream(line);
        int x, y, pickingX, pickingY;
        std::string tmp, description;
        char semicolon;
        std::getline(linestream, tmp, ';');
        x = stoi(tmp);
        std::getline(linestream, tmp, ';');
        y = stoi(tmp);
        std::getline(linestream, description, ';');

        if(description == "almacen" or description == "paso") {
            if(description == "almacen") passableBoard[y-1][x-1] = false;
            std::getline(linestream, tmp, ';');
            std::getline(linestream, tmp, ';');
        }
        else if(description[0] == 'a'){
            
            passableBoard[y-1][x-1] = false;
            std::getline(linestream, tmp, ';');
            pickingX = stoi(tmp);
            std::getline(linestream, tmp, ';');
            pickingY = stoi(tmp);
            boardInfo[pickingY-1][pickingX-1] = description;
        }
    }
    file.close();
}

int main() {

    readBoard("planogram_table.csv");
    
    for(int i = 0; i < passableBoard.size(); ++i) {
        for(int j = 0;  j < passableBoard[0].size(); ++j) {
            if(passableBoard[i][j]) std::cout << "  ";
            else std::cout << "X ";
        }
        std::cout << std::endl;
    }
    for(int i = 0; i < boardInfo.size(); ++i) {
        for(int j = 0;  j < boardInfo[0].size(); ++j) {
            if(boardInfo[i][j] == "") std::cout << "  ";
            else std::cout << "A ";
        }
        std::cout << std::endl;
    }
}