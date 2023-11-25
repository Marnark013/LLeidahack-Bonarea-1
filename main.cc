#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

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
    VI pickingTimes;
    std::string itemName;

    itemInfo() : pickingTimes(VI(5, 0)), itemName("") {}
    itemInfo(std::string itemname, VI temps) : pickingTimes(temps), itemName(itemname) {}
};

std::map<std::string, itemInfo> itemInfoMap;

//posa a boardInfo id de producte quan hi ha producte (picking_pos) i false als llocs on no es pot pathear de passableBoard
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
        std::getline(linestream, tmp, ';'); // agafa x
        x = stoi(tmp);
        std::getline(linestream, tmp, ';'); // agafa y
        y = stoi(tmp);
        std::getline(linestream, description, ';'); //agafa descripció

        if(description == "almacen" or description == "paso") {
            if(description == "almacen") passableBoard[y-1][x-1] = false;
            std::getline(linestream, tmp, ';'); //treu punts i coma
            std::getline(linestream, tmp, ';');
        }
        else if(description[0] == 'a'){
            
            passableBoard[y-1][x-1] = false;
            std::getline(linestream, tmp, ';'); //agafa pickingX
            pickingX = stoi(tmp);
            std::getline(linestream, tmp, ';'); //agafa pickingY
            pickingY = stoi(tmp);
            boardInfo[pickingY-1][pickingX-1] = description;
        }
    }
    file.close();
}

void readArticleInfo(const std::string& filename) {
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
        std::string itemName, itemId, tmp;
        std::vector<int> v(5);
        std::getline(linestream, itemId, ';'); //agafa itemID
        std::getline(linestream, itemName, ';'); //agafa el nom del item
        for(int i = 0; i < 5; ++i) {
            std::getline(linestream, tmp, ';'); //agafa vector de temps
            int tmp2 = stoi(tmp);
            v[i] = tmp2;
        }
        itemInfoMap[itemId] = itemInfo(itemName, v);
    }
    file.close();   
}

int main() {
    readBoard("./data/planogram_table.csv");
    readArticleInfo("./data/hackathon_article_picking_time.csv");
}