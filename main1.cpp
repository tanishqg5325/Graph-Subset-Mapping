#include <bits/stdc++.h>
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

void set_apsp_number(vector< vector<int> > &apsp_num_nodes, vector<int> adjacency_list[]){ //apsp_num_nodes[i][j] gives number of nodes reachable from i in maximum k steps

  int cutoff_height = apsp_num_nodes[0].size();
  int numNodes = apsp_num_nodes.size();
  for(int i=0; i<numNodes; ++i){
    // i is the source
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
          for(int &j : adjacency_list[node])
              if(!visited[j]){
                  bfs_queue.push(pii(j, new_level));
                  visited[j] = true;
                  apsp_num_nodes[i][new_level] += 1;
              }
      }

      for(int j=1; j<cutoff_height; ++j){
        apsp_num_nodes[i][j] += apsp_num_nodes[i][j-1];
      }
  }
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

    graph_input >> u >> v;  //NOTE: Can integers be directly read from file?

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

    vector<int> g1_incoming[n1], g1_outgoing[n1], g2_incoming[n2], g2_outgoing[n2];     // Incoming and Outgoing adjacency lists of g1 and g2
    for(auto &i : e1){ g1_outgoing[i.X].pb(i.Y); g1_incoming[i.Y].pb(i.X); }
    for(auto &i : e2){ g2_outgoing[i.X].pb(i.Y); g2_incoming[i.Y].pb(i.X); }

    vector<vector<int>> apsp_g1(n1, vector<int>(n1, INT_MAX));
    vector<vector<int>> apsp_g2(n2, vector<int>(n2, INT_MAX));

    vector< vector<int> > g1_num_nodes_arriving(n1, vector<int>(n1, 0));
    vector< vector<int> > g1_num_nodes_reachable(n1, vector<int>(n1, 0));
    vector< vector<int> > g2_num_nodes_arriving(n2, vector<int>(n1, 0));  //Size is n1 only, since n1<=n2
    vector< vector<int> > g2_num_nodes_reachable(n2, vector<int>(n1, 0));

    set_apsp_number(g1_num_nodes_arriving, g1_incoming);
    set_apsp_number(g2_num_nodes_arriving, g2_incoming);
    set_apsp_number(g1_num_nodes_reachable, g1_outgoing);
    set_apsp_number(g2_num_nodes_reachable, g2_outgoing);

    //setting apsp_g1

    // for(int i=0; i<n1; ++i){
    //   // i is the source
    //     apsp_g1[i][i] = 0;
    //     queue<int> bfs_queue;
    //     vector<bool> visited(n1, false);
    //     bfs_queue.push(i);
    //     visited[i] = true;
    //     while(!bfs_queue.empty()){
    //         int node = bfs_queue.front();
    //         bfs_queue.pop();
    //         for(int &j : g1_outgoing[node])
    //             if(!visited[j]){
    //                 bfs_queue.push(j);
    //                 visited[j] = true;
    //                 apsp_g1[i][j] = apsp_g1[i][node] + 1;
    //             }
    //     }
    // }
    //
    // //setting apsp_g2
    //
    // for(int i=0; i<n2; ++i){
    //   // i is the source
    //     apsp_g2[i][i] = 0;
    //     queue<int> bfs_queue;
    //     vector<bool> visited(n2, false);
    //     bfs_queue.push(i);
    //     visited[i] = true;
    //     while(!bfs_queue.empty()){
    //         int node = bfs_queue.front();
    //         bfs_queue.pop();
    //         for(int &j : g2_outgoing[node])
    //             if(!visited[j]){
    //                 bfs_queue.push(j);
    //                 visited[j] = true;
    //                 apsp_g2[i][j] = apsp_g2[i][node] + 1;
    //             }
    //     }
    // }


    ofstream sat_input;
    sat_input.open(file_name + ".satinput");

    string ans = "", tmp;
    int nov = 0, noc = 0; // nov = number of variables, noc = number of clauses

    ofstream encoding;
    encoding.open(file_name + ".encoding");
    // encoding<< n1 << " " << n2 << "\n";

    // encoding for each variable
    map<pii, int> mp; bool flag = 1;
    vector<int> domain[n1];
    for(int i=0;i<n1;i++)
    {
        for(int j=0;j<n2;j++)
        {
            bool to_insert_in_domain = true;

            for(int k=1; k<n1; ++k){
              //Number of nodes arriving with shortest path of length <= k and Number of nodes reachable with shortest path of length <= k
              if(g1_num_nodes_reachable[i][k] > g2_num_nodes_reachable[j][k] || g1_num_nodes_arriving[i][k] > g2_num_nodes_arriving[j][k]){
                to_insert_in_domain = false;
                break;
              }
            }
            if(to_insert_in_domain){
                mp[{i, j}] = ++nov;
                domain[i].pb(j);
                encoding << i+1 << " " << j+1 << " " << nov << "\n";
            }
        }
        if(domain[i].empty()) {flag = 0; break;}
    }
    encoding.close();

    if(n1 > n2 || flag == 0)
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
        for(int &j : domain[i])
            ans += to_string(mp[{i, j}]) + " ";
        ans += "0\n"; noc++;
    }

    // exactly one mapping clauses: O(n1*n2*n2)
    for(int i=0;i<n1;i++)
    {
        int l = domain[i].size();
        for(int j=0;j<l;j++)
            for(int k=j+1;k<l;k++) {
                ans += to_string(-mp[{i, domain[i][j]}]) + " " + to_string(-mp[{i, domain[i][k]}]) + " 0\n";
                noc++;
            }
    }

    // one-one mapping: O(n1*n1*n2)
    for(int i=0;i<n1;i++)
        for(int j=i+1;j<n1;j++)
        {
            int p, q;
            if(domain[i].size() <= domain[j].size()) p = i, q = j;
            else p = j, q = i;
            for(int &l : domain[p])
                if(mp[{q, l}]) {
                    ans += to_string(-mp[{p, l}]) + " " + to_string(-mp[{q, l}]) + " 0\n";
                    noc++;
                }
        }

/*
    //Shortest path clauses O(n1*n1*n2*n2 + n2*(n2+m2))
    for(int i=0; i<n2; ++i){
        // shortest_path_array = shortest_paths(from i)
        for(int j=i+1; j<n2; ++j){
            for(int k=0; k<n1; ++k){
                for(int l=k+1; l<n1; ++l){
                    if(apsp_g1[k][l] < apsp_g2[i][j] || apsp_g1[l][k] < apsp_g2[j][i])
                        if(mp[{k, i}] && mp[{l, j}]){
                            ans += to_string(-mp[{k, i}]) + " " + to_string(-mp[{l, j}]) + " 0\n";
                            noc++;
                        }

                    if(apsp_g1[k][l] < apsp_g2[j][i] || apsp_g1[l][k] < apsp_g2[i][j])
                        if(mp[{l, i}] && mp[{k, j}]){
                            ans += to_string(-mp[{l, i}]) + " " + to_string(-mp[{k, j}]) + " 0\n";
                            noc++;
                        }
                }
            }
        }
    }
*/

    vector<int> not_out_g1[n1], not_out_g2[n2]; // complement graphs
    bool isPresent[n2]{};
    // g1 complement: O(n1*n1)
    for(int i=0;i<n1;i++)
    {
        for(int &j : g1_outgoing[i]) isPresent[j] = 1;
        isPresent[i] = 1;
        for(int j=0;j<n1;j++) {
            if(!isPresent[j])
                not_out_g1[i].pb(j);
            else
                isPresent[j] = 0;
        }
    }

    // g2 complement: O(n2*n2)
    for(int i=0;i<n2;i++)
    {
        for(int &j : g2_outgoing[i]) isPresent[j] = 1;
        isPresent[i] = 1;
        for(int j=0;j<n2;j++) {
            if(!isPresent[j])
                not_out_g2[i].pb(j);
            else
                isPresent[j] = 0;
        }
    }

    // edges in g1, not in g2 clauses: O(m1*(n2*n2-m2))
    for(auto &i : e1)
        for(int &j : domain[i.X])
            for(int &l : not_out_g2[j])
                if(mp[{i.Y, l}]) {
                    ans += to_string(-mp[{i.X, j}]) + " " + to_string(-mp[{i.Y, l}]) + " 0\n";
                    noc++;
                }

    // edges in g2, not in g1 clauses: O(m2*(n1*n1-m1))
    for(auto &i : e2)
        for(int j=0;j<n1;j++) {
            if(mp[{j, i.X}] == 0) continue;
            for(int &l : not_out_g1[j])
                if(mp[{l, i.Y}]) {
                    ans += to_string(-mp[{j, i.X}]) + " " + to_string(-mp[{l, i.Y}]) + " 0\n";
                    noc++;
                }
        }

/*
    // neighbour clauses: O(n1*n2 + m1*m2)
    for(int i=0;i<n1;i++)
    {
        if(g1_outgoing[i].empty()) continue;
        for(int &j : domain[i])
        {
            tmp = to_string(-mp[{i, j}]) + " ";
            for(int &k : g1_outgoing[i])
            {
                ans += tmp;
                for(int &l : g2_outgoing[j])
                    if(mp[{k, l}])
                        ans += to_string(mp[{k, l}]) + " ";
                ans += "0\n"; noc++;
            }
        }
    }

    // non-neighbour clauses: O(n1*n1*(n2+m2))
    for(int i=0;i<n1;i++)
    {
        bool isPresent[n1]{};
        for(int &j : g1_outgoing[i]) isPresent[j] = 1;
        isPresent[i] = 1;
        for(int j=0;j<n1;j++)
        {
            if(isPresent[j]) continue;
            for(int &k : domain[i])
            {
                if(g2_outgoing[k].empty()) continue;
                tmp = to_string(-mp[{i, k}]) + " ";
                for(int &l : g2_outgoing[k])
                    if(mp[{j, l}]) {
                        ans += tmp + to_string(-mp[{j, l}]) + " 0\n";
                        noc++;
                    }
            }
        }
    }
*/
    ans = "p cnf " + to_string(nov) + " " + to_string(noc) + "\n" + ans;
    sat_input << ans;
    sat_input.close();
    return 0;
}
