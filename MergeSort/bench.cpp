#include "mergesort.cpp"

#include <iostream>
using namespace std;

bool cmp(const int &a, const int &b) {
    return a < b;
}

int main() {
    MergeSort <int> mergesort;
    int *arr = new int[100], len;
    cin >> len;
    for(int i = 0; i < len; i++) {
        cin >> arr[i];
    }

    mergesort.sort(arr, len);

    for(int i = 0; i < len; i++) {
        cout << arr[i] << ' ';
    }

    delete[] arr;
}