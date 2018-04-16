#include <iostream>
#include <queue>
#include <vector>
#include <cstring>
#include <fstream>
#include <string.h>
#include <cmath>
#include <algorithm>
#include <functional>
using namespace std;
//////////////////////////////////// Traveling saleman
#define MAX_N 10000
int n;
int graph[MAX_N][MAX_N];
bool check[MAX_N][MAX_N];
vector<int> result;


struct node { // Cấu trúc node của bài toán người bán hàng
	int level;
	vector<int> path;
	int bound;
};

class mycomparison // class so sánh của bài toán người bán hàng
{
	bool reverse;
public:
	mycomparison(const bool& revparam = false)
	{
		reverse = revparam;
	}
	bool operator() (const node& lhs, const node&rhs) const
	{
		if (reverse) return (lhs.bound > rhs.bound);
		else return (lhs.bound < rhs.bound);
	}
};


int TSP_Bound(const std::vector<int>& v) { // Tính cận dưới của bài toán người bán hàng
	int res = 0;
	memset(check, 0, sizeof(check));
	for (int i = 0; i < v.size() - 1; i++) {
		for (int j = 0; j < n; j++) {
			check[v[i]][j] = true;
		}
		check[v[i + 1]][v[i]] = true;
		for (int j = 0; j < n; j++) {
			check[j][v[i + 1]] = true;
		}
		res += graph[v[i]][v[i + 1]];
	}
	for (int i = 0; i < n; i++) {
		int _min = 1e9;
		bool flag = false;
		for (int j = 0; j < n; j++) {
			if (j != i && check[i][j] == false) {
				flag = true;
				_min = min(graph[i][j], _min);
			}
		}
		if (!flag) {
			_min = 0;
		}
		res += _min;
	}
	return res;
}

bool isIn(int j, const vector<int> &x) { // Kiểm tra xem thành phố j có nằm trong chu trình không
	for (int i = 0; i < x.size(); i++) {
		if (j == x[i])
			return true;
	}
	return false;
}


int Length(const vector<int> &x) { // Tính độ dài đường đi của chu trình
	int res = 0;
	for (int i = 0; i < x.size() - 1; i++) {
		res += graph[x[i]][x[i + 1]];
	}
	return res;
}

int TSM(int src) { // Hàm chính , bài toán người bán hàng
	int best = 1e9;

	vector<int> path(1, 0);
	node u{ src, path, TSP_Bound(path) };
	node v;
	typedef priority_queue <node, vector<node>, mycomparison> mypq_type;
	mypq_type pq(true);

	pq.push(u);

	while (pq.size()) {
		u = pq.top();
		pq.pop();
		if (u.bound < best) {
			v.level = u.level + 1;
			for (int i = 1; i < n; i++) {
				bool flag = false; // biến cờ để xét xem i có nằm trong path hay không 
				for (int j = 0; j < u.path.size(); j++) {
					if (u.path[j] == i) {
						flag = true;
						break;
					}
				}
				if (!flag) {
					v.path = u.path;
					v.path.push_back(i);

					if (v.level == n - 2) {
						int j;
						for (j = 1; j < n; j++) {
							if (!isIn(j, v.path))
								break;
						}
						v.path.push_back(j);
						v.path.push_back(0);
						int LenghtV = Length(v.path);
						if (LenghtV < best) {
							best = LenghtV;
							result = v.path;
						}
					}
					else {
						v.bound = TSP_Bound(v.path);
						if (v.bound < best)
							pq.push(v);
					}
				}
			}
		}
	}
	return best;
}


//////////////////////////////////// 0 - 1 Knapsack

int W;
int choose;


vector<bool> resultData;
struct thing { // Struct chứa thông tin đồ vật
	int v;
	int w;
	float v_w;
};

struct nodeKS { // Struct node trong bài toán KS
	int level;
	int profit;
	int weight;
	float bound;
	vector<bool> data;
};

bool compareThing(const thing& a, const thing& b) {
	return a.v_w > b.v_w;
}

class CompareKS
{
	bool reverse;
public:
	CompareKS(const bool& revparam = false)
	{
		reverse = revparam;
	}
	bool operator() (const nodeKS& lhs, const nodeKS&rhs) const
	{
		if (reverse) return (lhs.bound < rhs.bound);
		else return (lhs.bound>rhs.bound);
	}
};

vector<thing> arr;

float Bound0(const nodeKS& u) { // Hàm bound 
	if (u.weight >= W) return 0;
	float p = u.profit;
	int j = u.level + 1;
	if (j < n)
		p += (W*1.0 - u.weight*1.0) * arr[j].v_w;
	return ceil(p);
}
float Bound1(const nodeKS& u) { // Hàm bound cải tiến
	if (u.weight >= W) return 0;
	float p = u.profit;
	int j = u.level + 1;
	float accWeight = u.weight;
	while (j < n && accWeight + arr[j].w <= W) {
		accWeight += arr[j].w;
		p += arr[j].v;
		j++;
	}
	if (j < n) {
		p += (float(W) - accWeight)* (arr[j].v*1.0) / (arr[j].w*1.0);
	}
	return ceil(p);
}


int Knapsack_BB1() {
	typedef priority_queue <nodeKS, vector<nodeKS>, CompareKS> my_pq;
	my_pq pq(true);

	nodeKS u, v;
	u.level = -1;
	u.profit = u.weight = 0;
	u.bound = Bound1(u);
	u.data.resize(n, false);
	pq.push(u);

	int best = 0;

	while (!pq.empty()) {
		u = pq.top();
		pq.pop();
		if (u.bound > best) {
			v.data = u.data;
			v.data[u.level + 1] = true;
			v.level = u.level + 1;
			v.weight = u.weight + arr[v.level].w;
			v.profit = u.profit + arr[v.level].v;
			if (v.weight <= W && v.profit > best) {
				best = v.profit;
				resultData = v.data;
			}
			v.bound = Bound1(v);
			if (v.bound > best)
				pq.push(v);

			v.weight = u.weight;
			v.profit = u.profit;
			v.data[u.level + 1] = false;
			v.bound = Bound1(v);
			if (v.bound > best)
				pq.push(v);
		}
	}
	return best;
}

int Knapsack_BB0() {
	typedef priority_queue <nodeKS, vector<nodeKS>, CompareKS> my_pq;
	my_pq pq(true);

	nodeKS u, v;
	u.level = -1;
	u.profit = u.weight = 0;
	u.bound = Bound0(u);
	u.data.resize(n, false);
	pq.push(u);

	int best = 0;

	while (!pq.empty()) {
		u = pq.top();
		pq.pop();
		if (u.bound > best) {
			v.data = u.data;
			v.data[u.level + 1] = true;
			v.level = u.level + 1;
			v.weight = u.weight + arr[v.level].w;
			v.profit = u.profit + arr[v.level].v;
			if (v.weight <= W && v.profit > best) {
				best = v.profit;
				resultData = v.data;
			}
			v.bound = Bound0(v);
			if (v.bound > best)
				pq.push(v);

			v.weight = u.weight;
			v.profit = u.profit;
			v.data[u.level + 1] = false;
			v.bound = Bound0(v);
			if (v.bound > best)
				pq.push(v);
		}
	}
	return best;
}
///////////////////////////////////////////////
fstream fin, fout;

int main(int argc, char **argv) {
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	fin.tie(0);
	fout.tie(0);

	if (argc == 4 && strcmp(argv[1], "tsp") == 0) {
		fin.open(argv[2], ios::ios_base::in);
		if (!fin.good()) {
			cout << "Khong mo duoc file " << endl;
			return -1;
		}
		fout.open(argv[3], ios::ios_base::out);
		if (!fout.good()) {
			cout << "Khong mo duoc file " << endl;
			return -1;
		}

		fin >> n;
		int temp;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				fin >> graph[i][j];
			}
		}

		int ret = TSM(0);
		fout << ret << endl;
		for (int i = 0; i < result.size(); i++) {
			fout << result[i] << " ";
		}


		fin.close();
		fout.close();
	}
	else if (argc == 5 && strcmp(argv[1], "knap") == 0 ) {
		fin.open(argv[2], ios::ios_base::in);
		if (!fin.good()) {
			cout << "Khong mo duoc file " << endl;
			return -1;
		}
		fout.open(argv[3], ios::ios_base::out);
		if (!fout.good()) {
			cout << "Khong mo duoc file " << endl;
			return -1;
		}
		choose = atoi(argv[4]);

		fin >> W >> n;
		int v, w;
		for (int i = 0; i < n; i++) {
			fin >> w >> v;
			thing temp = { v, w, (float)(v*1.0) / (w*1.0) };
			arr.push_back(temp);
		}

		sort(arr.begin(), arr.end(), compareThing);
		int ret;
		if (choose == 0) {
			ret = Knapsack_BB0();
		}
		else if (choose == 1) {
			ret = Knapsack_BB1();
		}
		else {
			cout << "Loi tham so choose " << endl;
			return -1;
		}
		fout << ret << endl;
		for (int i = 0; i < n; i++) {
			if (resultData[i]) {
				fout << arr[i].w << " " << arr[i].v << endl;
			}
		}

		fin.close();
		fout.close();
	}
	else
	{
		cout << "Loi tham so";
		return -1;
	}
	return 0;
}