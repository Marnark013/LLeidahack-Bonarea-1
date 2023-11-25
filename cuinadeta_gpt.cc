#include <iostream>
#include <vector>
#include <limits>
#include <cstring>

using namespace std;

const int MAX = 20; // Adjust this based on the maximum number of cities
const int INF = numeric_limits<int>::max();
int n; // Number of cities
int dist[MAX][MAX]; // Adjacency matrix for distances
int memo[MAX][1 << MAX]; // Memoization table

int tsp(int pos, int bitmask) {
    if (bitmask == (1 << n) - 1) // All cities have been visited
        return dist[pos][0]; // Return distance to starting city

    if (memo[pos][bitmask] != -1)
        return memo[pos][bitmask];

    int ans = INF;

    for (int nxt = 0; nxt < n; nxt++) { // Try to go to an unvisited city
        if (nxt != pos && !(bitmask & (1 << nxt))) { // If not already visited
            int newAns = dist[pos][nxt] + tsp(nxt, bitmask | (1 << nxt));
            ans = min(ans, newAns);
        }
    }

    return memo[pos][bitmask] = ans;
}

int main() {
    // Read number of cities
    cin >> n;

    // Read distance matrix
    /*
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            cin >> dist[i][j];

            */
    // Initialize memoization table

    memset(memo, -1, sizeof memo);

    cout << "The shortest path has length: " << tsp(0, 1) << endl;

    return 0;
}