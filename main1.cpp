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

int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    assert(argc == 2);

    int n1 = 0, n2 = 0, u, v;   // n1 = # vertices in smaller graph (g1), n2 = # vertices in larger graph (g2)
    vector<pii> e1, e2;         // e1 = edge list of smaller graph (g1), e2 = edge list of larger graph (g2)

    // string file_name = "";
    // for(int i=0; argv[1][i] != '\0'; i++) file_name += argv[1][i];
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

    ofstream sat_input;
    sat_input.open(file_name + ".satinput");

    // TODO: Handle the case of n1 > n2

    string ans = "", tmp;
    int nov = 0, noc = 0; // nov = number of variables, noc = number of clauses

    ofstream encoding;
    encoding.open(file_name + ".encoding");
    encoding<< n1 << " " << n2 << "\n";
    encoding.close();
    // encoding for each variable
    map<pii, int> mp;
    for(int i=0;i<n1;i++)
        for(int j=0;j<n2;j++)
        {
            ++nov;
            if(g1_incoming[i].size() <= g2_incoming[j].size() && g1_outgoing[i].size() <= g2_outgoing[j].size()){
              mp[{i, j}] = nov;
            }
            else{
              mp[{i, j}] = -1;
            }
            // encoding << i+1 << " " << j+1 << " " << nov << "\n";
        }

    // atleast one mapping clauses: O(n1*n2)
    for(int i=0;i<n1;i++)
    {
        for(int j=0;j<n2;j++){
          if(mp[{i, j}] != -1)
            ans += to_string(mp[{i, j}]) + " ";
        }
        ans += "0\n"; noc++;
    }

    // exactly one mapping clauses: O(n1*n2*n2)
    for(int i=0;i<n1;i++)
        for(int j=0;j<n2;j++)
            for(int k=j+1;k<n2;k++) //NOTE: Is k from j+1 correct?
            {
                bool to_add = !((mp[{i, k}] == -1) || (mp[{i, j}] == -1));
                if(to_add){
                  ans += to_string(-mp[{i, j}]) + " " + to_string(-mp[{i, k}]) + " 0\n";
                  noc++;
                }
            }

    // one-one mapping: O(n1*n1*n2)
    for(int i=0;i<n2;i++)
    {
        for(int j=0;j<n1;j++)
        {
            tmp = to_string(-mp[{j, i}]) + " ";
            for(int k=j+1;k<n1;k++) //NOTE: Is k from j+1 correct?
            {
                bool to_add = !((mp[{k, i}] == -1) || (mp[{j, i}] == -1));
                if(to_add){
                  ans += tmp + to_string(-mp[{k, i}]) + " 0\n";
                  noc++;
                }
            }
        }
    }

    // neighbour clauses: O(n1*n2 + m1*m2)
    for(int i=0;i<n1;i++)
    {
        if(g1_outgoing[i].empty()) continue;
        for(int j=0;j<n2;j++)
        {
            if(g2_outgoing[j].empty()) continue;
            tmp = to_string(-mp[{i, j}]) + " ";
            if(mp[{i, j}] != -1){
              for(int k : g1_outgoing[i])
              {
                  ans += tmp;
                  for(int l : g2_outgoing[j]){
                    if(mp[{k, l}] != -1)
                      ans += to_string(mp[{k, l}]) + " ";
                  }
                  ans += "0\n"; noc++;
              }
            }
        }
    }

    // non-neighbour clauses: O(n1*n1*(n2+m2))
    for(int i=0;i<n1;i++)
    {
        bool isPresent[n1]{};
        for(int j : g1_outgoing[i]) isPresent[j] = 1;
        isPresent[i] = 1;
        for(int j=0;j<n1;j++)
        {
            if(isPresent[j]) continue;
            for(int k=0;k<n2;k++)
            {
                if(g2_outgoing[k].empty()) continue;
                tmp = to_string(-mp[{i, k}]) + " ";
                if(mp[{i, k}] != -1){
                  for(int l : g2_outgoing[k])
                  {   if(mp[{j, l}] != -1){
                        ans += tmp + to_string(-mp[{j, l}]) + " 0\n";
                        noc++;
                      }
                  }
                }
            }
        }
    }

    ans = "p cnf " + to_string(nov) + " " + to_string(noc) + "\n" + ans;
    sat_input << ans;
    sat_input.close();
    return 0;
}
