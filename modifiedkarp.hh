#include "backupAlgorithm.hh"

std::pair<int, int> Bfs(const Pos initPos, const Pos endPos);

void specialSwap(VS& setVertices, int midPosition, int endPosition);

int oneDistance(VS& setVertices, int x, int y);

int oneDistance2(std::string x, std::string y);

int heldKarp(VS& result, VS setVertices, int n, int startVector, int finalVector);