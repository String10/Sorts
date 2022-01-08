#pragma once

template <typename T>
class TimSort {
	using size_t = unsigned long long;
	using Comparer = bool(*)(const T &, const T &);
public:
	TimSort();
	~TimSort();
	void sort(T [], size_t, Comparer=def_cmp);
private:
	T temp, *arr_helper;
	size_t max_size;
	static bool def_cmp(const T &, const T &);
	void swap(T &, T&);
	size_t minRunLength(size_t);
	size_t upperbound(const T [], const T &, size_t, size_t, Comparer);
	void insertSort(T [], size_t, Comparer);
};

template <typename T>
TimSort<T>::TimSort() {
	max_size = (1 << 9);
	arr_helper = new T[max_size];
}

template <typename T>
TimSort<T>::~TimSort() {
	delete[] arr_helper;
	max_size = 0;
}

template <typename T>
void TimSort<T>::sort(T arr[], size_t len, Comparer cmp) {
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

	/* If len <= 64, just run insertSort. */
	if(len <= (1 << 6)) {
		insertSort(arr, len, cmp);
		return;
	}

	/* TODO: Slide the arr to different Run. */

	/* TODO: GALLOP Mode. */

	/* TODO: Less memory use method. */
}

template <typename T>
bool TimSort<T>::def_cmp(const T &a, const T &b) {
	return a < b;
}

template <typename T>
void TimSort<T>::swap(T &a, T &b) {
	temp = a, a = b, b = temp;
}

template <typename T>
typename TimSort<T>::size_t TimSort<T>::minRunLength(size_t len) {
	size_t sticky = 0;
	while(len >= (1 << 6)) {
		sticky |= (len & 1);
		len >>= 1;
	}

	return len + sticky;
}

template <typename T>
typename TimSort<T>::size_t TimSort<T>::upperbound(const T arr[], const T &val, size_t l, size_t r, Comparer cmp) {
	size_t mid;
	while(l < r) {
		mid = (l + r) / 2;
		if(cmp(val, arr[mid])) {
			r = mid;
		}
		else {
			l = mid + 1;
		}
	}
	return cmp(val, arr[l]) ? l : (l + 1);
}

template <typename T>
void TimSort<T>::insertSort(T arr[], size_t len, Comparer cmp) {
	size_t pos;
	for(size_t i = 1; i < len; i++) {
		if(!cmp(arr[i], arr[i - 1])) {
			continue;
		}
		pos = upperbound(arr, arr[i], 0, i - 1, cmp), temp = arr[i];
		for(size_t j = i; j > pos; j--) {
			arr[j] = arr[j - 1];
		}
		arr[pos] = temp;
	}
}