#include "./TimSort/timsort.cpp"

#include <bits/stdc++.h>
using namespace std;

bool cmp(const int &a, const int &b) {
	return a > b;
}

int main() {
	TimSort <int> sort;
	int *arr = new int[100], len;
	cin >> len;
	for(int i = 0; i < len; i++) {
		cin >> arr[i];
	}
	
	vector <int> vec;

	sort.sort(arr, len, cmp);

	for(int i = 0; i < len; i++) {
		cout << arr[i] << ' ';
	}

	delete[] arr;
}