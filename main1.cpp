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

    if(argc != 2) {cout<<"Insufficient Parameters\n"; return 0;}

    int n1 = 0, n2 = 0, u, v;   // n1 = # vertices in smaller graph (g1), n2 = # vertices in larger graph (g2)
    vector<pii> e1, e2;         // e1 = edge list of smaller graph (g1), e2 = edge list of larger graph (g2)
    cin>>u>>v;
    while(u && v)
    {
        e2.pb({u-1, v-1});
        n2 = max(n2, max(u, v));
        cin>>u>>v;
    }

    string line; pii e; getline(cin, line);
    while(getline(cin, line))
    {
        if(line == "") break;
        e = processLine(line);
        e1.pb({e.X-1, e.Y-1});
        n1 = max(n1, max(e.X, e.Y));
    }

    vector<int> g1[n1], g2[n2];     // adjacency list of g1 and g2
    for(auto &i : e1) g1[i.X].pb(i.Y);
    for(auto &i : e2) g2[i.X].pb(i.Y);
    
    ofstream sat_input;
    string file_name = "";
    for(int i=0; argv[1][i] != '\0'; i++) file_name += argv[1][i];
    file_name += ".satinput";
    sat_input.open(file_name);
    
    // TODO: Handle the case of n1 > n2

    string ans = "", tmp;
    int nov=0, noc = 0; // nov = number of variables, noc = number of clauses

    // encoding for each variable
    map<pii, int> mp;
    for(int i=0;i<n1;i++)
        for(int j=0;j<n2;j++)
            mp[{i, j}] = ++nov;
    
    // atleast one mapping clauses: O(n1*n2)
    for(int i=0;i<n1;i++)
    {
        for(int j=0;j<n2;j++)
            ans += to_string(mp[{i, j}]) + " ";
        ans += "0\n"; noc++;
    }

    // exactly one mapping clauses: O(n1*n2*n2)
    for(int i=0;i<n1;i++)
        for(int j=0;j<n2;j++)
            for(int k=j+1;k<n2;k++)
            {
                ans += to_string(-mp[{i, j}]) + " " + to_string(-mp[{i, k}]) + " 0\n";
                noc++;
            }

    // neighbour clauses
    for(int i=0;i<n1;i++)
    {
        if(g1[i].empty()) continue;
        for(int j=0;j<n2;j++)
        {
            if(g2[j].empty()) continue;
            tmp = to_string(-mp[{i, j}]) + " ";
            for(int k : g1[i])
            {
                ans += tmp;
                for(int l : g2[j])
                    ans += to_string(mp[{k, l}]) + " ";
                ans += "0\n"; noc++;
            }
        }
    }
    
    ans = "p cnf " + to_string(nov) + " " + to_string(noc) + "\n" + ans;
    sat_input << ans;
    sat_input.close();
    return 0;
}