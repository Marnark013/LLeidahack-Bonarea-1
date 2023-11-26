#include "dataStructures.hh"

void ReadBoard(const std::string &filename);

void ReadArticleInfo(const std::string &filename);

void ReadClientsProperties();

void ReadTickets();

bool PosOk(int x, int y);

void FillDistances(std::string itemId);

void CalculateDistances();

std::string ClosestLeftToStart(std::set<std::string> items, std::string item);

std::vector<std::string> greedySearch(std::vector<std::string> items);
