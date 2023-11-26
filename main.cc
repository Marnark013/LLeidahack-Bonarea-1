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
      VS sorted;
      heldKarp(sorted, products, products.size(), 0, products.size() - 1);
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

      else if(currentPos.x != endPoint.x or currentPos.y != endPoint.y){                                                        //si no esta on ha de recollir
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
      else {
        activeClients.erase(idIn);
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