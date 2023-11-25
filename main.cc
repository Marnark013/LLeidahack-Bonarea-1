#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <ctime>

using VI = std::vector<int>;
using VVI = std::vector<VI>;
using VS = std::vector<std::string>;
using VVS = std::vector<VS>;
using VB = std::vector<bool>;
using VVB = std::vector<VB>;
using VSI = std::vector <std::pair<std::string, int>>;
using MTS = std::map <std::time_t, std::string>;
using VS = std::vector <std::string>;

VVS boardInfo = VVS(20, VS(47, ""));
VVI passableBoard = VVI(20, VI(47, true));

struct TicketInfo
{
    std::time_t enter_time;
    std::string client_id;
    VSI products_list;
    std::string ticked_id;
};

using MSTI = std::map <std::string, TicketInfo>;

MSTI clientsInfo; //client id to his ticket info
MTS ordClients; //clients id ordered by the time they enter the store

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

void ReadTickets()
{
    std::fstream file;                                              //pointer to the file
    file.open("./data/hackathon_tickets.csv", std::ios::in);        //open file and selecting read

    std::string row, temp, word, auxNumeroElement; 
    std::tm tm;
    int numeroElements;
    getline(file, row);                                             //ens petem la primera

    while (getline(file, row))
    {
        struct TicketInfo aux;                                      //chequear esta declaracion dentro del while
        std::stringstream s(row);
        
        getline(s, word, ';');
        std::istringstream timeStream(word);
        timeStream >> std::get_time(&tm, "%H:%M:%S");               //se necesitaba el iomanip
        aux.enter_time = std::mktime(&tm);

        getline(s, word, ';');
        aux.client_id = word;

        getline(s, word, ';');
        getline(s, auxNumeroElement, ';');
        numeroElements = stoi(auxNumeroElement);

        MSTI::iterator ite = clientsInfo.find(aux.client_id);
        

        if (ite == clientsInfo.end())
        {
            aux.products_list.push_back(std::make_pair(word, numeroElements));

            getline(s, word, ';');
            aux.ticked_id = word;
            clientsInfo.insert(make_pair(aux.client_id, aux));
        }
        else
        {
            ite->second.products_list.push_back(std::make_pair(word, numeroElements));
            getline(s, word, ';');
        }
    }
}

int main() 
{

}