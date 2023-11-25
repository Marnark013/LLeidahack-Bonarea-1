#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <ctime>
#include <queue>

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

std::vector<std::pair<int, int>> dirs = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

enum Dir {
    Down, Right, Up, Left, None
};

struct TicketInfo
{
    std::time_t enter_time;
    std::string client_id;
    VSI products_list;
    std::string ticked_id;
};

struct CostumerProperties
{
    int step_seconds;
    int picking_offset;
};

using MSTI = std::map <std::string, TicketInfo>;
using MSCP = std::map <std::string, CostumerProperties>;

MSTI clientsInfo; //client id to his ticket info
MTS ordClients; //clients id ordered by the time they enter the store
MSCP costumerStats; //stats of the costumers, step_seconds and picking_offset

struct Pos {
    int x;
    int y;

    Pos(int px, int py) : x(px), y(py) {}
    Pos() : x(-1), y(-1) {}
};

struct itemInfo {
    VI pickingTimes;
    std::string itemName;
    Pos pos;

    itemInfo() : pickingTimes(VI(5, 0)), itemName(""), pos(-1, -1) {}
    itemInfo(std::string itemname, VI temps, Pos p) : pickingTimes(temps), itemName(itemname), pos(p) {}
};

using MSII = std::map <std::string, itemInfo>;
MSII itemInfoMap;
std::map<std::string, std::map<std::string, int>> distancesMap;
std::map<std::string, int> testMap;

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
            itemInfoMap[description] = itemInfo("", VI(5, 0), Pos(pickingX-1, pickingY-1));
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
        Pos pos = itemInfoMap[itemId].pos;
        itemInfoMap[itemId] = itemInfo(itemName, v, pos);
    }
    file.close();   
}

void ReadClientsProperties()
{
    std::fstream file;
    file.open("./data/hackathon_customers_properties.csv");

    std::string row, costumerid, stepseconds, pickingoffset;
    getline(file, row);
    struct CostumerProperties aux;

    while(getline(file, row))
    {
        std::stringstream s(row);
        getline(s, costumerid, ';');
        getline(s, stepseconds, ';');
        getline(s, pickingoffset, ';');

        aux.step_seconds = stoi(stepseconds);
        aux.picking_offset = stoi(pickingoffset);

        costumerStats.insert(make_pair(costumerid, aux));

        aux = {};
    }
}

void ReadTickets()
{
    std::fstream file;                                              //pointer to the file
    file.open("./data/hackathon_tickets.csv", std::ios::in);        //open file and selecting read

    std::string row, temp, word, auxNumeroElement; 
    int numeroElements;
    getline(file, row);                                             //ens petem la primera

    while (getline(file, row))
    {
        std::tm tm = {};
        struct TicketInfo aux;                                      //chequear esta declaracion dentro del while
        std::stringstream s(row);
        
        getline(s, word, ';');
        std::istringstream ss(word);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");               //se necesitaba el iomanip
        aux.enter_time = std::mktime(&tm);

        getline(s, word, ';');
        aux.client_id = word;

        getline(s, word, ';');
        getline(s, auxNumeroElement, ';');
        numeroElements = stoi(auxNumeroElement);

        MSTI::iterator ite = clientsInfo.find(aux.client_id);
        

        if (ite == clientsInfo.end())
        {
            ordClients.insert(make_pair(aux.enter_time, aux.client_id));
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

bool pos_ok(int x, int y) {
    return ((x >= 0 and x < 47) and (y < 20 and y >= 0));
}

void fillDistances(std::string itemId) {
    VVB visited(20, VB(47, false));
    VVI distances(20, VI(47, 0));
    std::queue<Pos> Q;
    Pos pos = itemInfoMap[itemId].pos;

    Q.push(pos);
    
    while(not Q.empty()) {
        Pos actPos = Q.front();
        Q.pop();

        if(boardInfo[actPos.y][actPos.x] != "") {
            distancesMap[itemId][boardInfo[actPos.y][actPos.x]] = distances[actPos.y][actPos.x];
        }
        visited[actPos.y][actPos.x] = true;
        for(int i = 0; i < 4; ++i) {
            std::pair<int, int> dir = dirs[i];
            Pos nwPos;
            nwPos.x = actPos.x + dir.first;
            nwPos.y = actPos.y + dir.second;
            if(pos_ok(nwPos.x, nwPos.y) and passableBoard[nwPos.y][nwPos.x] and not visited[nwPos.y][nwPos.x]) {
                Q.push(nwPos);
                distances[nwPos.y][nwPos.x] = distances[actPos.y][actPos.x] + 1;
                visited[nwPos.y][nwPos.x] = true;
            }
        }
    }
}

void calculateDistances() {
    for(int i = 0; i < 20; ++i) {
        for(int j = 0; j < 47; ++j) {
            if(boardInfo[i][j] != "") {
                
                fillDistances(boardInfo[i][j]);
            }
        }
    }
}

//CONSULTORAS


void printItemMap(const std::map<std::string, itemInfo>& myMap) {
    for (const auto& pair : myMap) {
        std::cout << "Key: " << pair.first << std::endl;
        std::cout << "Item Name: " << pair.second.itemName << std::endl;
        std::cout << "Position: " << pair.second.pos.x << " " << pair.second.pos.y << std::endl;
        std::cout << "Values: ";
        for (int value : pair.second.pickingTimes) {
            std::cout << value << " ";
        }
        std::cout << "\n\n"; // Separate entries with empty lines
    }
}

/*--------------HELD KARP-----------------*/

void specialSwap(VS& setVertices, int midPosition, int endPosition)
{
    std::string aux = setVertices[endPosition];
    setVertices[endPosition] = midPosition;
    setVertices[midPosition] = aux;
}

int oneDistance(VS& setVertices, int x, int y)
{
    auto ite = distancesMap.find(setVertices[x]);
    auto ite2 = ite->second.find(setVertices[y]);
    return ite2->second;
}

int oneDistance2(std::string x, std::string y)
{
    auto ite = distancesMap.find(x);
    auto ite2 = ite->second.find(y);
   //std::cout << "tiempo entre: " << x << " y " << y << " : " << ite2->second;
    return ite2->second;
}

//setVertices[0] MUST BE THE START
//startVector must be 1
int heldKarp(VS& result, VS setVertices, std::string initialVertex, std::string finalVertex, int n, int startVector, int finalVector) //n number of unique items
{
    if (n == 4)
    {
        return oneDistance2(setVertices[1], setVertices[2]) + std::min(oneDistance2(setVertices[0], setVertices[1]) + oneDistance2(setVertices[2], setVertices[3]), oneDistance2(setVertices[0], setVertices[2]) + oneDistance2(setVertices[1], setVertices[3]));
    }
    
    int mini = 1e5;
    for (int i = 0; i < n - 2; ++i)
    {
        VS auxVector = setVertices;
        specialSwap(auxVector, i + startVector, finalVector - 1);
        mini = std::min(mini, heldKarp(result, auxVector, initialVertex, auxVector[finalVector],  n - 1, startVector, finalVector - 1) + oneDistance2(setVertices[finalVector], auxVector[finalVector - 1]));
    }

    return mini;
}

/*--------------HELD KARP-----------------*/

int main() 
{
    
    readBoard("./data/planogram_table.csv");
    readArticleInfo("./data/hackathon_article_picking_time.csv");
    calculateDistances();
    ReadClientsProperties();
    ReadTickets();

    VS result;
    VS setVertices = {"a5724", "a8923", "a2255", "a2061", "a2089"};
    int opt = heldKarp(result, setVertices, "a5724", "a2061", 5, 0, 4);

    std::cout << opt << std::endl;
}