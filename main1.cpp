#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <assert.h>
#include <map>

#define pb push_back
#define X first
#define Y second

using namespace std;
typedef long long ll;
typedef pair<int,int> pii;

pii processLine(string line)
{
    stringstream ss(line); string ver; vector<string> v;
    while(ss >> ver) v.pb(ver);
    assert(v.size() == 2);
    pii ans = {stoi(v[0]), stoi(v[1])};
    return ans;
}

//Returns the maximum height reached in bfs
int set_apsp_number(vector< vector<int> > &apsp_num_nodes, vector<int> adjacency_list[], int cycle_bounds[], int cutoff_height){ 
    //apsp_num_nodes[i][j] gives number of nodes reachable from i in maximum k steps
  int max_height_reached = 0;
  int numNodes = apsp_num_nodes.size();
  for(int i=0; i<numNodes; ++i){
    // i is the source
      cycle_bounds[i] = INT32_MAX;
      apsp_num_nodes[i][0] = 1;
      queue<pii> bfs_queue; //Stores the (vertex, level) pair
      vector<bool> visited(numNodes, false);
      bfs_queue.push(pii(i,0));
      visited[i] = true;
      int curr_level = 0;
      while(!bfs_queue.empty() && (curr_level + 1)<cutoff_height){
          pii node_level_pair = bfs_queue.front();
          bfs_queue.pop();
          int node = node_level_pair.X;
          curr_level = node_level_pair.Y;
          int new_level = curr_level + 1;
          for(int &j : adjacency_list[node]) {
              if(!visited[j]){
                  bfs_queue.push(pii(j, new_level));
                  visited[j] = true;
                  apsp_num_nodes[i][new_level] += 1;
              }
              else if(j == i) 
                    cycle_bounds[i] = min(cycle_bounds[i], new_level);
          }
      }

      for(int j=1; j<cutoff_height; ++j){
        if(apsp_num_nodes[i][j])
          apsp_num_nodes[i][j] += apsp_num_nodes[i][j-1];
        else{
          max_height_reached = max(max_height_reached, j);
          break;
        }
      }

      if(apsp_num_nodes[i][cutoff_height-1])
        max_height_reached = max(max_height_reached, cutoff_height);
  }

  return max_height_reached;
}

int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    assert(argc == 2);

    int n1 = 0, n2 = 0, u, v;   // n1 = # vertices in smaller graph (g1), n2 = # vertices in larger graph (g2)
    vector<pii> e1, e2;         // e1 = edge list of smaller graph (g1), e2 = edge list of larger graph (g2)

    string file_name = argv[1];
    ifstream graph_input;
    graph_input.open(file_name + ".graphs");

    graph_input >> u >> v;

    while(u && v)
    {
        e2.pb({u-1, v-1});
        n2 = max(n2, max(u, v));
        graph_input >> u >> v;
    }

    string line; pii e; getline(graph_input, line);
    while(getline(graph_input, line))
    {
        if(line == "") break;
        e = processLine(line);
        e1.pb({e.X-1, e.Y-1});
        n1 = max(n1, max(e.X, e.Y));
    }
    graph_input.close();

    ofstream sat_input(file_name + ".satinput");
    int m1 = e1.size(), m2 = e2.size(), max1 = n1*(n1-1), max2 = n2*(n2-1);

    if(n1 > n2 || m1 > m2 || (m2 == max2 && m1 < max1))
    {
        sat_input << "p cnf 1 2\n";
        sat_input << "1 0\n";
        sat_input << "-1 0\n";
        sat_input.close();
        return 0;
    }

    ofstream encoding(file_name + ".encoding");
    if(m2 == max2 && m1 == max1)
    {
        for(int i=1;i<=n1;i++) encoding << i << " " << i << " " << i << "\n";
        sat_input << "p cnf " << n1 << " " << n1 << "\n";
        for(int i=1;i<=n1;i++) sat_input << i << " 0\n";
        encoding.close();
        sat_input.close();
        return 0;
    }

    int mp_temp[n1][n2];
    for(int i=0; i<n1; ++i){
        for(int j=0; j<n2; ++j){
            mp_temp[i][j] = 0;
        }
    }

    vector<int> g1_incoming[n1], g1_outgoing[n1], g2_incoming[n2], g2_outgoing[n2];     // Incoming and Outgoing adjacency lists of g1 and g2
    vector<int> g1_undirected[n1], g2_undirected[n2]; //Undirected adjacency lists
    for(auto &i : e1){ g1_outgoing[i.X].pb(i.Y); g1_incoming[i.Y].pb(i.X); g1_undirected[i.X].pb(i.Y); g1_undirected[i.Y].pb(i.X);}
    for(auto &i : e2){ g2_outgoing[i.X].pb(i.Y); g2_incoming[i.Y].pb(i.X); g2_undirected[i.X].pb(i.Y); g2_undirected[i.Y].pb(i.X);}

    vector<int> g1_isolated, g2_isolated;

    for(int i=0;i<n1;i++)
        if(g1_incoming[i].empty() && g1_outgoing[i].empty())
            g1_isolated.pb(i);

    for(int i=0;i<n2;i++)
        if(g2_incoming[i].empty() && g2_outgoing[i].empty())
            g2_isolated.pb(i);

    if(g1_isolated.size() > g2_isolated.size())
    {
        sat_input << "p cnf 1 2\n";
        sat_input << "1 0\n";
        sat_input << "-1 0\n";
        sat_input.close();
        return 0;
    }
    
    vector< vector<int> > g1_num_nodes_arriving(n1, vector<int>(n1, 0));
    vector< vector<int> > g1_num_nodes_reachable(n1, vector<int>(n1, 0));
    vector< vector<int> > g2_num_nodes_arriving(n2, vector<int>(n1, 0));  //Size is n1 only, since n1<=n2
    vector< vector<int> > g2_num_nodes_reachable(n2, vector<int>(n1, 0));

    vector< vector<int> > g1_num_nodes_undirected(n1, vector<int>(n1, 0));  //Size is n1 only, since n1<=n2
    vector< vector<int> > g2_num_nodes_undirected(n2, vector<int>(n1, 0));

    //For checking directed cycles
    int g1_cycle_bounds_directed_normal[n1]; //Array for storing special min_cycle_length
    int g2_cycle_bounds_directed_normal[n2];
    int g1_cycle_bounds[n1];  // unusable
    int g2_cycle_bounds[n2];  // unusable
    
    int arriving_cutoff_height = set_apsp_number(g1_num_nodes_arriving, g1_incoming, g1_cycle_bounds, n1);
    int reachable_cutoff_height = set_apsp_number(g1_num_nodes_reachable, g1_outgoing, g1_cycle_bounds_directed_normal, n1);
    set_apsp_number(g2_num_nodes_arriving, g2_incoming, g2_cycle_bounds, arriving_cutoff_height);
    set_apsp_number(g2_num_nodes_reachable, g2_outgoing, g2_cycle_bounds_directed_normal, reachable_cutoff_height);

    int undirected_cutoff_height = set_apsp_number(g1_num_nodes_undirected, g1_undirected, g1_cycle_bounds, n1);
    set_apsp_number(g2_num_nodes_undirected, g2_undirected, g2_cycle_bounds, undirected_cutoff_height);
    
    string ans = "", tmp;
    int nov = 0, noc = 0; // nov = number of variables, noc = number of clauses

    // encoding for each variable
    map<pii, int> mp; bool flag = 1, to_insert_in_domain;
    vector<int> domain[n1];

    int g1_isolated_mapping[n1]{}, k = g1_isolated.size(); bool isAlreadyMapped[n2]{};
    for(int i=0;i<k;i++)
    {
        int p = g1_isolated[i], q = g2_isolated[i];
        g1_isolated_mapping[p] = q+1;
        isAlreadyMapped[q] = 1;
    }

    for(int i=0;i<n1;i++)
    {
        if(g1_isolated_mapping[i])
        {
            encoding << i+1 << " " << g1_isolated_mapping[i] << " " << (++nov) << "\n";
            ans += to_string(nov) + " 0\n"; noc++;
            continue;
        }
        for(int j=0;j<n2;j++)
        {
            if(isAlreadyMapped[j]) continue;
            to_insert_in_domain = true;
            // to_insert_in_domain = (g1_outgoing[i].size() <= g2_outgoing[j].size()) && (g1_incoming[i].size() <= g2_incoming[j].size());
            if(g1_cycle_bounds_directed_normal[i] < g2_cycle_bounds_directed_normal[j])
                to_insert_in_domain = false;
                // break;

            //Checking for All Pair Shortest Path Lengths
            if(to_insert_in_domain){
                for(int k=1; k<n1; ++k){
                    //Number of nodes arriving with shortest path of length <= k and Number of nodes reachable with shortest path of length <= k
                    bool to_break = (g1_num_nodes_reachable[i][k] == 0) && (g1_num_nodes_arriving[i][k] == 0) && (g1_num_nodes_undirected[i][k] == 0);
                    if(to_break)
                        break;

                    if(g2_num_nodes_reachable[j][k] == 0)
                        g2_num_nodes_reachable[j][k] = g2_num_nodes_reachable[j][k-1];

                    if(g2_num_nodes_arriving[j][k] == 0)
                        g2_num_nodes_arriving[j][k] = g2_num_nodes_arriving[j][k-1];

                    if(g2_num_nodes_undirected[j][k] == 0)
                        g2_num_nodes_undirected[j][k] = g2_num_nodes_undirected[j][k-1];

                    bool to_break_without_insertion =
                        (g1_num_nodes_reachable[i][k] > g2_num_nodes_reachable[j][k]) || (g1_num_nodes_arriving[i][k] > g2_num_nodes_arriving[j][k]) || g1_num_nodes_undirected[i][k] > g2_num_nodes_undirected[j][k];
                    
                    if(to_break_without_insertion){
                        to_insert_in_domain = false;
                        break;
                    }
                }
            }

            if(to_insert_in_domain){
                mp_temp[i][j] = ++nov;
                domain[i].pb(j);
                encoding << i+1 << " " << j+1 << " " << nov << "\n";
            }
        }
        if(domain[i].empty()) {flag = 0; break;}
    }
    encoding.close();

    if(flag == 0)
    {
        sat_input << "p cnf 1 2\n";
        sat_input << "1 0\n";
        sat_input << "-1 0\n";
        sat_input.close();
        return 0;
    }

    // atleast one mapping clauses: O(n1*n2)
    for(int i=0;i<n1;i++)
    {
        if(g1_isolated_mapping[i]) continue;
        for(int &j : domain[i])
            ans += to_string(mp_temp[i][j]) + " ";
        ans += "0\n"; noc++;
    }

    // exactly one mapping clauses: O(n1*n2*n2)
    for(int i=0;i<n1;i++)
    {
        int l = domain[i].size();
        // if (l < n2 - 2) continue;
        for(int j=0;j<l;j++)
            for(int k=j+1;k<l;k++) {
                ans += to_string(-mp_temp[i][domain[i][j]]) + " " + to_string(-mp_temp[i][domain[i][k]]) + " 0\n";
                noc++;
            }
    }

    // one-one mapping: O(n1*n1*n2)
    for(int i=0;i<n1;i++)
    {
        if(g1_isolated_mapping[i]) continue;
        for(int j=i+1;j<n1;j++)
        {
            if(g1_isolated_mapping[j]) continue;
            int p, q;
            if(domain[i].size() <= domain[j].size()) p = i, q = j;
            else p = j, q = i;
            for(int &l : domain[p])
                if(mp_temp[q][l]) {
                    ans += to_string(-mp_temp[p][l]) + " " + to_string(-mp_temp[q][l]) + " 0\n";
                    noc++;
                }
        }
    }

    // // neighbour clauses: O(n1*n2 + m1*m2)
    for(int i=0;i<n1;i++)
    {
        if(g1_outgoing[i].empty()) continue;
        for(int &j : domain[i])
        {
            tmp = to_string(-mp_temp[i][j]) + " ";
            for(int &k : g1_outgoing[i])
            {
                ans += tmp;
                for(int &l : g2_outgoing[j])
                    if(mp_temp[k][l])
                        ans += to_string(mp_temp[k][l]) + " ";
                ans += "0\n"; noc++;
            }
        }
    }

    // non-neighbour clauses: O(n1*n1*(n2+m2))
    bool isPresent[n1]{};
    for(int i=0;i<n1;i++)
    {
        if(g1_isolated_mapping[i]) continue;
        for(int &j : g1_outgoing[i]) isPresent[j] = 1;
        isPresent[i] = 1;
        for(int j=0;j<n1;j++)
        {
            if(isPresent[j]) {isPresent[j] = 0; continue;}
            if(g1_isolated_mapping[j]) continue;
            for(int &k : domain[i])
            {
                if(g2_outgoing[k].empty()) continue;
                tmp = to_string(-mp_temp[i][k]) + " ";
                for(int &l : g2_outgoing[k])
                    if(mp_temp[j][l]) {
                        ans += tmp + to_string(-mp_temp[j][l]) + " 0\n";
                        noc++;
                    }
            }
        }
    }

    ans = "p cnf " + to_string(nov) + " " + to_string(noc) + "\n" + ans;
    sat_input << ans;
    sat_input.close();
    return 0;
}
