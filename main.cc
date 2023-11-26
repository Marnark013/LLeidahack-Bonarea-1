#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using VI = std::vector<int>;
using VVI = std::vector<VI>;
using VS = std::vector<std::string>;
using VVS = std::vector<VS>;
using VB = std::vector<bool>;
using VVB = std::vector<VB>;
using VSI = std::vector<std::pair<std::string, int>>;
using MTS = std::map<std::time_t, std::string>;
using VS = std::vector<std::string>;

VVS boardInfo = VVS(20, VS(47, ""));
VVI passableBoard = VVI(20, VI(47, true));

std::vector<std::pair<int, int>> dirs = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

enum Dir { Down, Right, Up, Left, None };

struct TicketInfo {
  std::time_t enter_time;
  std::string client_id;
  VSI products_list;
  std::string ticked_id;
};

int madre = 0;

struct CostumerProperties {
  int step_seconds;
  int picking_offset;
};

using MSTI = std::map<std::string, TicketInfo>;
using MSCP = std::map<std::string, CostumerProperties>;

MSTI clientsInfo;   // client id to his ticket info
MTS ordClients;     // clients id ordered by the time they enter the store
MSCP costumerStats; // stats of the costumers, step_seconds and picking_offset

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
  itemInfo(std::string itemname, VI temps, Pos p)
      : pickingTimes(temps), itemName(itemname), pos(p) {}
};

struct activeInfo {
  Pos pos;
  bool picking;
  int stepCd;
  VSI sortedPath;
  int itemTimeLeft;

  activeInfo()
      : pos({-1, -1}), picking(false), stepCd(-1), sortedPath(VSI()),
        itemTimeLeft(-1) {}
  activeInfo(Pos p, bool pick, int stCd, VSI sorted, int iTimeLeft)
      : pos(p), picking(pick), stepCd(stCd), sortedPath(sorted),
        itemTimeLeft(iTimeLeft) {}

  void SetPicking(bool b) { picking = b; }
  void SetStepCd(int stcd) { stepCd = stcd; }
  void SetItemTimeLeft(int iTime) { itemTimeLeft = iTime; }
  void SubPath() {
    sortedPath[0].second = sortedPath[0].second - 1;
    if (sortedPath[0].second <= 0)
      sortedPath.erase(sortedPath.begin());
  }
  void SetPos(Pos p) {
    pos = p;
  }
};

using MSII = std::map<std::string, itemInfo>;
MSII itemInfoMap;
std::map<std::string, std::map<std::string, int>> distancesMap;
std::map<std::string, int> testMap;

Pos startingPoint = {28, 19};

bool PosOk(int x, int y) {
  return ((x >= 0 and x < 47) and (y < 20 and y >= 0));
}

std::pair<int, int> Bfs(const Pos initPos, const Pos endPos) {
  std::queue<std::pair<Pos, std::pair<int, int>>> q_pos;
  VVB visited(40, VB(80, false));

  q_pos.push({initPos, {0, 0}});
  while (not q_pos.empty()) {
    Pos actPos = q_pos.front().first;
    std::pair<int, int> fstDir = q_pos.front().second;
    q_pos.pop();
    if (actPos.x == endPos.x and actPos.y == endPos.y)
      return fstDir;

    visited[actPos.y][actPos.x] = true;

    for (int i = 0; i < 4; ++i) {
      std::pair<int, int> dir = dirs[i];
      Pos nwPos;
      nwPos.x = actPos.x + dir.first;
      nwPos.y = actPos.y + dir.second;
      if (PosOk(nwPos.x, nwPos.y) and passableBoard[nwPos.y][nwPos.x] and
          !visited[nwPos.y][nwPos.x]) {

        q_pos.push(
            {nwPos, (fstDir.first == 0 and fstDir.second == 0) ? dir : fstDir});
        visited[nwPos.y][nwPos.x] = true;
      }
    }
  }
  return {0, 0};
}

// posa a boardInfo id de producte quan hi ha producte (picking_pos) i false als
// llocs on no es pot pathear de passableBoard
void ReadBoard(const std::string &filename) {
  std::ifstream file;
  file.open(filename);
  if (file.fail()) {
    std::cerr << "No s'ha obert" << std::endl;
    return;
  }
  std::string header, line;
  getline(file, header);
  while (getline(file, line)) {
    std::istringstream linestream(line);
    int x, y, pickingX, pickingY;
    std::string tmp, description;
    std::getline(linestream, tmp, ';'); // agafa x
    x = stoi(tmp);
    std::getline(linestream, tmp, ';'); // agafa y
    y = stoi(tmp);
    std::getline(linestream, description, ';'); // agafa descripció

    if (description == "almacen" or description == "paso") {
      if (description == "almacen")
        passableBoard[y - 1][x - 1] = false;
      std::getline(linestream, tmp, ';'); // treu punts i coma
      std::getline(linestream, tmp, ';');
    } else if (description[0] == 'a') {

      passableBoard[y - 1][x - 1] = false;
      std::getline(linestream, tmp, ';'); // agafa pickingX
      pickingX = stoi(tmp);
      std::getline(linestream, tmp, ';'); // agafa pickingY
      pickingY = stoi(tmp);
      boardInfo[pickingY - 1][pickingX - 1] = description;
      itemInfoMap[description] =
          itemInfo("", VI(5, 0), Pos(pickingX - 1, pickingY - 1));
    }
  }
  file.close();
}

void ReadArticleInfo(const std::string &filename) {
  std::ifstream file;
  file.open(filename);
  if (file.fail()) {
    std::cerr << "No s'ha obert" << std::endl;
    return;
  }
  std::string header, line;
  getline(file, header);
  while (getline(file, line)) {
    std::istringstream linestream(line);
    std::string itemName, itemId, tmp;
    std::vector<int> v(5);
    std::getline(linestream, itemId, ';');   // agafa itemID
    std::getline(linestream, itemName, ';'); // agafa el nom del item
    for (int i = 0; i < 5; ++i) {
      std::getline(linestream, tmp, ';'); // agafa vector de temps
      int tmp2 = stoi(tmp);
      v[i] = tmp2;
    }
    Pos pos = itemInfoMap[itemId].pos;
    itemInfoMap[itemId] = itemInfo(itemName, v, pos);
  }
  file.close();
}

void ReadClientsProperties() {
  std::fstream file;
  file.open("./data/hackathon_customers_properties.csv");

  std::string row, costumerid, stepseconds, pickingoffset;
  getline(file, row);
  struct CostumerProperties aux;

  while (getline(file, row)) {
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

void ReadTickets() {
  std::fstream file; // pointer to the file
  file.open("./data/hackathon_tickets.csv",
            std::ios::in); // open file and selecting read

  std::string row, temp, word, auxNumeroElement;
  int numeroElements;
  getline(file, row); // ens petem la primera

  while (getline(file, row)) {
    std::tm tm = {};
    struct TicketInfo aux; // chequear esta declaracion dentro del while
    std::stringstream s(row);

    getline(s, word, ';');
    std::istringstream ss(word);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S"); // se necesitaba el iomanip
    aux.enter_time = std::mktime(&tm);

    getline(s, word, ';');
    aux.client_id = word;

    getline(s, word, ';');
    getline(s, auxNumeroElement, ';');
    numeroElements = stoi(auxNumeroElement);

    MSTI::iterator ite = clientsInfo.find(aux.client_id);

    if (ite == clientsInfo.end()) {
      ordClients.insert(make_pair(aux.enter_time, aux.client_id));
      aux.products_list.push_back(std::make_pair(word, numeroElements));

      getline(s, word, ';');
      aux.ticked_id = word;
      clientsInfo.insert(make_pair(aux.client_id, aux));
    } else {
      ite->second.products_list.push_back(std::make_pair(word, numeroElements));
      getline(s, word, ';');
    }
  }
}

void FillDistances(std::string itemId) {
  VVB visited(20, VB(47, false));
  VVI distances(20, VI(47, 0));
  std::queue<Pos> Q;
  Pos pos;
  if (itemId == "starting")
    pos = startingPoint;
  else
    pos = itemInfoMap[itemId].pos;

  Q.push(pos);

  while (not Q.empty()) {
    Pos actPos = Q.front();
    Q.pop();

    if (boardInfo[actPos.y][actPos.x] != "") {
      distancesMap[itemId][boardInfo[actPos.y][actPos.x]] =
          distances[actPos.y][actPos.x];
    }
    visited[actPos.y][actPos.x] = true;
    for (int i = 0; i < 4; ++i) {
      std::pair<int, int> dir = dirs[i];
      Pos nwPos;
      nwPos.x = actPos.x + dir.first;
      nwPos.y = actPos.y + dir.second;
      if (PosOk(nwPos.x, nwPos.y) and passableBoard[nwPos.y][nwPos.x] and
          not visited[nwPos.y][nwPos.x]) {
        Q.push(nwPos);
        distances[nwPos.y][nwPos.x] = distances[actPos.y][actPos.x] + 1;
        visited[nwPos.y][nwPos.x] = true;
      }
    }
  }
}

void CalculateDistances() {
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 47; ++j) {
      if (boardInfo[i][j] != "") {

        FillDistances(boardInfo[i][j]);
      }
    }
  }
  FillDistances("starting");
}

std::string ClosestLeftToStart(std::set<std::string> items, std::string item) {
  int itemX;
  if (item == "starting")
    itemX = startingPoint.x;
  else {
    itemX = itemInfoMap[item].pos.x;
  }
  auto it = distancesMap.find("starting");
  std::string closestToStart = "starting";
  std::string closestLeftToStart = "starting";
  int minDist = 1e9;
  int minLeft = 1e9;
  for (auto ite = items.begin(); ite != items.end(); ++ite) {
    Pos itemPos = itemInfoMap[*ite].pos;

    if (itemPos.x < itemX) {
      // std::cerr << itemPos.x << " " << itemX << std::endl;
      if ((it->second.find(*ite))->second < minLeft) {
        minLeft = (it->second.find(*ite))->second;
        closestLeftToStart = (it->second.find(*ite))->first;
      }
    }

    if ((it->second.find(*ite))->second < minDist) {
      minDist = (it->second.find(*ite))->second;
      closestToStart = (it->second.find(*ite))->first;
    }
  }
  if (closestLeftToStart != "starting")
    return closestLeftToStart;
  else
    return closestToStart;
}

std::vector<std::string> greedySearch(std::vector<std::string> items) {
  std::vector<std::string> ret;
  std::string actItem = "starting";
  std::set<std::string> buf;
  for (auto m : items)
    buf.insert(m);

  for (int i = 0; i < items.size(); ++i) {
    std::string tmp = ClosestLeftToStart(buf, actItem);
    ret.push_back(tmp);
    buf.erase(tmp);
  }
  return ret;
}

int main() {

  //-- INICI LECTURA --

  ReadBoard("./data/planogram_table.csv");
  ReadArticleInfo("./data/hackathon_article_picking_time.csv");
  ReadClientsProperties();
  ReadTickets();
  CalculateDistances();

  //-- LECTURA ACABADA --

  time_t startingTime = ordClients.begin()->first;
  time_t endTime = ordClients.rbegin()->first;
  time_t actTime = startingTime;

  std::map<std::string, activeInfo> activeClients;

  while (actTime < endTime) {
    if (ordClients.find(actTime) != ordClients.end()) { // si s'ha de inicialitzar un fill en el temps actual
      std::string id = (ordClients.find(actTime)->second); //id del fill a inicialitzar
      TicketInfo personTicket = clientsInfo.find(id)->second;   //agafem ticketinfo
      VSI productsInfo = personTicket.products_list;            //retorna vector<string, int> amb nomProducte, quantitat a agafar
      std::map<std::string, int> auxMap;
      for (auto m : productsInfo)
        auxMap[m.first] = m.second;
      VS products;
      for (auto m : productsInfo)
        products.push_back(m.first);
      VS sorted = greedySearch(products);
      VSI nwProducts;
      for (auto m : sorted)
        nwProducts.push_back({m, auxMap[m]});
      CostumerProperties cp = costumerStats.find(id)->second;
      itemInfo iI = itemInfoMap.find(nwProducts[0].first)->second;
      int pickT;                    //pickT es temps de picking sense cd de customer
      if (nwProducts[0].second >= 5)
        pickT = iI.pickingTimes[4];
      else
        pickT = iI.pickingTimes[nwProducts[0].second];
      int pickTime = pickT + cp.picking_offset;
      activeInfo act = activeInfo(startingPoint, false, cp.step_seconds + 1,
                                  nwProducts, pickTime);
      activeClients.insert({id, act});
    }
    for (auto it = activeClients.begin(); it != activeClients.end(); ++it) {    //actualització correcta dels customers actius
      std::string idIn = it->first;
      activeInfo aI = it->second;
      
      Pos toGo = itemInfoMap.find(aI.sortedPath[0].first)->second.pos;
      Pos currentPos = aI.pos;

      if(currentPos.x == toGo.x and currentPos.y == toGo.y) {       //si esta a on ha de recollir
        it->second.SetPicking(true);                                //posa picking a true
        it->second.SetItemTimeLeft(it->second.itemTimeLeft - 1);//resta un segon de picking
        if(it->second.itemTimeLeft == 0) {                   //si s'ha acabat el temps de picking
            auto it2 = it->second.sortedPath.begin(); //it a path<string, int> a calcular
            it2->second = it2->second - 1;                      // li resta 1 al int del path (s'ha pickeat 1 unitat)
            int cPickTime = costumerStats.find(idIn)->second.picking_offset;//hem de setear el temps de picking nou
            int pickT;
            std::string itemId = it->second.sortedPath.begin()->first;
            VI pickingTimes = itemInfoMap.find(itemId)->second.pickingTimes;
            if(it2->second >= 5) {
                pickT = pickingTimes[4];
            }
            else if(it2->second != 0) {
                pickT = pickingTimes[it2->second];
            }
            int totalPickTime = cPickTime + pickT;
            it->second.itemTimeLeft = totalPickTime;

            if(it2->second == 0) {                              //si ja no queden
                it->second.sortedPath.erase(it->second.sortedPath.begin()); //et carregues la instancia
                it->second.SetPicking(false);               //poses picking a false
            }
        }
        
      }

      else {                                                        //si no esta on ha de recollir
        it->second.SetStepCd(it->second.stepCd - 1);
        if(it->second.stepCd == 0) {                                //si pot fer una passa
            it->second.SetStepCd(costumerStats.find(idIn)->second.step_seconds);  //resetejem stepCD
            std::pair<int, int> toGoDir = Bfs(currentPos, toGo);        //calculem direcció a la que anar
            Pos nwPos;
            nwPos.x = currentPos.x + toGoDir.first;
            nwPos.y = currentPos.y + toGoDir.second;
            it->second.SetPos(nwPos);                                  //seteem la nova posició
            it->second.SetPicking(false);                              //posem picking a false
        } 
          
      }
      std::cout << idIn << ';' << clientsInfo.find(idIn)->second.ticked_id << std::endl;  
      //char buffer[80];
      //struct tm * timeInfo = localtime(&actTime);
      //strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeInfo);

      //std::cout << idIn << ';' << clientsInfo.find(idIn)->second.ticked_id << ';' << it->second.pos.x << ';' << it->second.pos.y << ';' << it->second.picking << ';' << buffer << std::endl;
    }
    
    ++actTime;
  }
}