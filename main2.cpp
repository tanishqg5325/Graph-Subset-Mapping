#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <assert.h>

#define pb push_back
#define X first
#define Y second

using namespace std;
typedef long long ll;
typedef pair<int,int> pii;

map<int, pii> mp;

void processLine(string line)
{
    stringstream ss(line); string ver; vector<string> v;
    while(ss >> ver) v.pb(ver);
    assert(v.size() == 3);
    mp[stoi(v[2])] = {stoi(v[0]), stoi(v[1])};
}

int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    assert(argc == 2);

    string file_name = argv[1], line;

    ifstream sat_output(file_name + ".satoutput");
    ofstream mapping(file_name + ".mapping");

    sat_output >> line;
    if(line == "UNSAT")
    {
        mapping << "0\n";
        sat_output.close();
        mapping.close();
        return 0;
    }

    ifstream encoding(file_name + ".encoding");

    while(getline(encoding, line))
    {
        if(line == "") break;
        processLine(line);
    }
    encoding.close();

    getline(sat_output, line);
    getline(sat_output, line);
    sat_output.close();

    stringstream ss(line); string var; int tmp;
    while(ss >> var)
    {
        tmp = stoi(var);
        if(tmp > 0)
            mapping << mp[tmp].X << " " << mp[tmp].Y << "\n";
    }
    mapping.close();

    return 0;
}
