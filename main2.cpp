#include <bits/stdc++.h>
#define pb push_back
#define X first
#define Y second
using namespace std;
typedef long long ll;
typedef pair<int,int> pii;

// map<int, pii> mp;

pii numVertices;  // X is n1, Y is n2
void processLine(string line)
{
    stringstream ss(line); string ver; vector<string> v;
    while(ss >> ver) v.pb(ver);
    assert(v.size() == 2);

    numVertices.X = stoi(v[0]);
    numVertices.Y = stoi(v[1]);
    // mp[stoi(v[2])] = {stoi(v[0]), stoi(v[1])};
}

int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    assert(argc == 2);

    // string file_name = "";
    // for(int i=0; argv[1][i] != '\0'; i++) file_name += argv[1][i];
    string file_name = argv[1];
    ifstream encoding;
    encoding.open(file_name + ".encoding");

    string line;
    while(getline(encoding, line))
    {
        if(line == "") break;
        processLine(line);
    }
    encoding.close();

    ifstream sat_output; ofstream mapping;
    sat_output.open(file_name + ".satoutput");
    mapping.open(file_name + ".mapping");

    sat_output >> line;
    if(line == "UNSAT")
    {
        mapping << "0\n";
        sat_output.close();
        mapping.close();
    }

    getline(sat_output, line);
    getline(sat_output, line);
    sat_output.close();

    stringstream ss(line); string var; int tmp;
    while(ss >> var)
    {
        tmp = stoi(var);
        if(tmp > 0){
          int firstCoordinate = ceil(float(((float)tmp / (numVertices.Y))));
          int secondCoordinate = tmp % numVertices.Y == 0 ? numVertices.Y : tmp % numVertices.Y;
          mapping << firstCoordinate << " " << secondCoordinate << "\n";
        }
    }
    mapping.close();

    return 0;
}
