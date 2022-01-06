#pragma once

template <typename T>
class MergeSort {
    using size_t = unsigned long long;
    using Comparer = bool(*)(const T &, const T &);
public:
    MergeSort();
    ~MergeSort();
    void sort(T [], size_t, Comparer=def_cmp);
private:
    T temp, *arr_helper;
    size_t max_size;
    static bool def_cmp(const T &, const T &);
    void swap_helper(T &, T&);
    void sort_helper(T [], size_t, size_t, Comparer);
};

template <typename T>
MergeSort<T>::MergeSort() {
    max_size = (1 << 9);
    arr_helper = new T[max_size];
}

template <typename T>
MergeSort<T>::~MergeSort() {
    delete[] arr_helper;
    max_size = 0;
}

template <typename T>
void MergeSort<T>::sort(T arr[], size_t len, Comparer cmp) {
    if(len <= 1) {
        return;
    }

    while(max_size <= len) {
        if(max_size == 0) {
            max_size = 1;
        }
        else {
            max_size <<= 1;
        }
    }

    sort_helper(arr, 0, len, cmp);
}

template <typename T>
bool MergeSort<T>::def_cmp(const T &a, const T &b) {
    return a < b;
}

template <typename T>
void MergeSort<T>::swap_helper(T &a, T &b) {
    temp = a, a = b, b = temp;
}

template <typename T>
void MergeSort<T>::sort_helper(T arr[], size_t offset, size_t len, Comparer cmp) {
    if(len == 1) {
        return;
    }
    if(len == 2) {
        if(!cmp(arr[offset], arr[offset + 1])) {
            swap_helper(arr[offset], arr[offset + 1]);
        }
        return;
    }

    int len_a = len / 2, len_b = len - len_a, offset_a = offset, offset_b = offset + len_a;
    sort_helper(arr, offset_a, len_a, cmp), sort_helper(arr, offset_b, len_b, cmp);

    if(arr[offset_a + len_a - 1] <= arr[offset_b]) {
        return;
    }
    
    if(arr[offset_b + len_b - 1] < arr[offset_a]) {
        for(int i = offset_a; i < offset_a + len_a; i++) {
            arr_helper[i + len_b] = arr[i];
        }
        for(int i = offset; i < offset + len; i++) {
            if(i < offset + len_b) {
                arr[i] = arr[i + len_a];
            }
            else {
                arr[i] = arr_helper[i];
            }
        }
        return;
    }

    int ptr_a = 0, ptr_b = 0;
    if(arr[offset_a + len_a - 1] > arr[offset_b + len_b - 1]) {
        while(ptr_a < len_a) {
            while(ptr_b < len_b && arr[offset_a + ptr_a] > arr[offset_b + ptr_b]) {
                arr_helper[offset + ptr_a + ptr_b] = arr[offset_b + ptr_b];
                ptr_b++;
            }
            arr_helper[offset + ptr_a + ptr_b] = arr[offset_a + ptr_a];
            ptr_a++;
        }
    }
    else {
        while(ptr_b < len_b) {
            while(ptr_a < len_a && arr[offset_a + ptr_a] <= arr[offset_b + ptr_b]) {
                arr_helper[offset + ptr_a + ptr_b] = arr[offset_a + ptr_a];
                ptr_a++;
            }
            arr_helper[offset + ptr_a + ptr_b] = arr[offset_b + ptr_b];
            ptr_b++;
        }
    }

    for(int i = offset; i < offset + len; i++) {
        arr[i] = arr_helper[i];
    }

    return;
}