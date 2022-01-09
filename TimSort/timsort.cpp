#pragma once

#include <utility>
#include <stack>

template <typename T>
class TimSort {
	using size_t = unsigned long long;
	using Comparer = bool(*)(const T &, const T &);
	using Run = std::pair <size_t, size_t>;
public:
	TimSort();
	~TimSort();
	void sort(T [], size_t, Comparer=def_cmp);
private:
	T temp, *buf;
	size_t max_size;
	static bool def_cmp(const T &, const T &);
	void swap(T &, T&);
	void reverse(T [], size_t, size_t);
	size_t minRunLength(size_t);
	size_t upperbound(const T [], const T &, size_t, size_t, Comparer);
	void insertSort(T [], size_t, Comparer);
};

template <typename T>
TimSort<T>::TimSort() {
	max_size = (1 << 9);
	buf = new T[max_size];
}

template <typename T>
TimSort<T>::~TimSort() {
	delete[] buf;
	max_size = 0;
}

template <typename T>
void TimSort<T>::sort(T arr[], size_t len, Comparer cmp) {
	if(len <= 1) {
		return;
	}

	/* Resize the buf. TODO: maybe it can cost less memory. */
	bool resized = false;
	while(max_size <= len) {
		if(max_size == 0) {
			max_size = 1;
		}
		else {
			max_size <<= 1;
		}
		resized = true;
	}
	if(resized) {
		delete buf;
		buf = new T[max_size];
	}

	/* If len <= 64, just run insertSort. */
	if(len <= (1 << 6)) {
		insertSort(arr, len, cmp);
		return;
	}

	/* TODO: Slice the arr to different Run. */
	std::stack <Run> sta;
	size_t min_run_len = minRunLength(len);
	for(size_t i = 0; i < len; ) {
		size_t j = i + 1;
		if(j >= len) {
			Run run = sta.top();
			sta.pop();
			size_t pos = upperbound(arr, arr[i], run.first, run.second, cmp);
			temp = arr[i];
			for(j = i; j > pos; j--) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = temp;
			sta.push({ run.first, i });
		}
		else {
			bool is_incr = !cmp(arr[j], arr[i]);
			size_t pos;
			while(j - i + 1 <= min_run_len) {
				if(is_incr && !cmp(arr[j + 1], arr[j])) {
					j++;
				}
				else {
					if(is_incr) {
						pos = upperbound(arr, arr[j + 1], i, j, cmp), temp = arr[j + 1];
						for(size_t k = j + 1; k > pos; k--) {
							arr[k] = arr[k - 1];
						}
						arr[pos] = temp;
					}
					else {
						/* TODO: Add the case of strict descend run. */
					}
				}
			}
		}
	}

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
void TimSort<T>::reverse(T arr[], size_t l, size_t r) {
	while(l < r) {
		swap(arr[l], arr[r]);
		l++, r--;
	}
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