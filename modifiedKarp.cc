#include "modifiedkarp.hh"

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


/*--------------HELD KARP-----------------*/

void specialSwap(VS& setVertices, int midPosition, int endPosition)
{
    std::string aux = setVertices[endPosition];
    setVertices[endPosition] = setVertices[midPosition];
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
    std::cout << "tiempo entre: " << x << " y " << y << " : " << ite2->second << std::endl;
    return ite2->second;
}

//setVertices[0] MUST BE THE START
int heldKarp(VS& result, VS setVertices, int n, int startVector, int finalVector) //n number of unique items
{
    ++madre;
    if (n == 4)
    {
        std::cout << startVector + 2 << " " << finalVector << std::endl;
        int medio = oneDistance2(setVertices[startVector + 1], setVertices[startVector + 2]);
        int c12 = oneDistance2(setVertices[startVector], setVertices[startVector + 1]);
        int c13 = oneDistance2(setVertices[startVector], setVertices[startVector + 2]);
        int c24 = oneDistance2(setVertices[startVector + 1], setVertices[startVector + 3]);
        int c34 = oneDistance2(setVertices[startVector + 2], setVertices[startVector + 3]);
        int onGod = medio + std::min(c12 + c34, c13 + c24);
        std::cout << "b " <<  onGod << std::endl;
        return onGod;
    }
    
    int mini = 1e5;
    for (int i = 0; i < n - 2; ++i)
    {
        //std::cout << "Tamaño: " << n << std::endl;
        VS auxVector = setVertices;
        specialSwap(auxVector, i + startVector, finalVector - 1);
        //std::cout << madre << " " << initialVertex << " " << auxVector[finalVector] << std::endl;
        int aux33 = heldKarp(result, auxVector, n - 1, startVector, finalVector - 1);
        //std::cout << "aiuda polipolo" << std::endl;
        int aux43 = oneDistance2(setVertices[finalVector], auxVector[finalVector - 1]);
        //std::cout << aux33 << " " << aux43 << std::endl; 
        mini = std::min(mini, aux33 + aux43);
        if (mini == aux33 + aux43)
        {
            result.clear();
            for (int aa = 0; aa < auxVector.size(); ++aa)
            {
                result.push_back(auxVector[aa]);
            }
        }
    }

    std::cout << "a " << mini << std::endl;
    return mini;
}
/*--------------HELD KARP-----------------*/