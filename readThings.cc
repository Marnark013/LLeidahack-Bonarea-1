#include "readThings.hh";

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

bool PosOk(int x, int y) {
  return ((x >= 0 and x < 47) and (y < 20 and y >= 0));
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
