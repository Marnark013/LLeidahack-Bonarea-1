#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

using VI = vector<int>;
using VVI = vector<VI>;
using VS = vector<string>;
using VVS = vector<VS>;

VVS boardInfo = VVS(20, VS(47));

void readBoard() {
    string filePath = "./data/planogram_table.csv";
    ifstream file;

    file.open(filePath);
    if(file.fail()) {
        cerr << "No s'ha obert" << endl;
        return;
    }
    string line;
    while(getline(file, line)) {
        istringstream linestream(line);
        int x, y;
        string tileInfo;
        char comma;
        linestream >> x >> comma >> y >> tileInfo >> comma >> comma;
        boardInfo[x-1][y-1] = tileInfo;
    }
    file.close();

}

int main() {
    for(int i = 0; i < boardInfo.size(); ++i) {
        for(int j = 0;  j < boardInfo[0].size(); ++j) {
            cout << boardInfo[i][j];
        }
        cout << endl;
    }
}