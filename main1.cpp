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

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);
    int n1 = 0, n2 = 0, u, v;   // n1 = # vertices in smaller graph (g1), n2 = # vertices in larger graph (g2)
    vector<pii> e1, e2;         // e1 = edge list of smaller graph (g1), e2 = edge list of larger graph (g2)
    cin>>u>>v;
    while(u && v)
    {
        e2.pb({u, v});
        n2 = max(n2, max(u, v));
        cin>>u>>v;
    }

    string line; pii e; getline(cin, line);
    while(getline(cin, line) && !cin.eof())
    {
        if(line == "") break;
        e = processLine(line);
        e1.pb(e);
        n1 = max(n1, max(e.X, e.Y));
    }

    vector<int> g1[n1+1], g2[n2+1];     // adjacency list of g1 and g2
    for(auto &i : e1) g1[i.X].pb(i.Y);
    for(auto &i : e2) g2[i.X].pb(i.Y);

    return 0;
}
