#include "usedlibrary.hh"

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
Pos endPoint = {34, 19};