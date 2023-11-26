#include "modifiedkarp.hh"

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

  while (actTime < endTime) { //actTime < endTime
    if (ordClients.find(actTime) != ordClients.end()) {
      std::string id = (ordClients.find(actTime)->second);
      TicketInfo personTicket = clientsInfo.find(id)->second;
      VSI productsInfo = personTicket.products_list;
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
      int pickT;
      if (nwProducts[0].second >= 5)
        pickT = iI.pickingTimes[4];
      else
        pickT = iI.pickingTimes[nwProducts[0].second];
      int pickTime = pickT + cp.picking_offset;
      activeInfo act = activeInfo(startingPoint, false, cp.step_seconds + 1,
                                  nwProducts, pickTime);
      activeClients.insert({id, act});
    }
    for (auto it = activeClients.begin(); it != activeClients.end(); ++it) {
      std::string id = it->first;
      activeInfo aI = it->second;
      Pos currentPos = aI.pos;
      Pos toGo = itemInfoMap.find(aI.sortedPath[0].first)->second.pos;
      std::pair<int, int> toGoDir = Bfs(currentPos, toGo);
      std::cerr << toGoDir.first << " " << toGoDir.second << std::endl;
    }

    //++actTime;
  }
}