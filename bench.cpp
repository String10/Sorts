#include "./TimSort/timsort.cpp"

#include <bits/stdc++.h>
using namespace std;

using test_type = pair <int, int>;

bool cmp(const test_type &a, const test_type &b) {
	return a.first < b.first;
}

int main() {
	TimSort <test_type> sort;
	test_type *arr = new test_type[100];
	int len;
	cin >> len;
	for(int i = 0; i < len; i++) {
		cin >> arr[i].first >> arr[i].second;
	}
	
	sort.sort(arr, len, cmp);

	for(int i = 0; i < len; i++) {
		cout << '#' << i << ':' << arr[i].first << ' ' << arr[i].second << endl;
	}

	delete[] arr;
}

/*
15

72 1
45 1
23 1
53 1
54 1
63 1
45 2
44 1
23 2
45 3
38 1
45 4
63 2
36 1
38 2
*/