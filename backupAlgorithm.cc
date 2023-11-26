#include "backupAlgorithm.hh"

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