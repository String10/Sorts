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
	void mergeRun(T [], const Run &, const Run &, Comparer);
	size_t upperbound(const T [], const T &, size_t, size_t, Comparer);
	size_t upperboundDec(const T [], const T &, size_t, size_t, Comparer);
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
	for(size_t i = 0, j; i < len; i = j + 1) {
		j = i;
		if(j + 1 >= len) {
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
			bool is_incr = !cmp(arr[j + 1], arr[j]);
			size_t pos;
			while(j - i + 1 <= min_run_len) {
				if(!(is_incr && !cmp(arr[j + 1], arr[j]))) {
					if(is_incr) {
						pos = upperbound(arr, arr[j + 1], i, j, cmp), temp = arr[j + 1];
						for(size_t k = j + 1; k > pos; k--) {
							arr[k] = arr[k - 1];
						}
						arr[pos] = temp;
					}
					else {
						pos = upperboundDec(arr, arr[j + 1], i, j, cmp), temp = arr[j + 1];
						for(size_t k = j + 1; k > pos; k--) {
							arr[k] = arr[k - 1];
						}
						arr[pos] = temp;
					}
				}
				j++;
			}
			if(!is_incr) {
				reverse(arr, i, j);
			}
			sta.push({ i, j });
		}
		Run x, y, z;
		size_t x_len, y_len, z_len;
		while(sta.size() >= 3) {
			x = sta.top(), sta.pop(), x_len = x.second - x.first + 1;
			y = sta.top(), sta.pop(), y_len = y.second - y.first + 1;
			z = sta.top(), sta.pop(), z_len = z.second - z.first + 1;
			/* Need to meet: z.length > x.length + y.length, or merge(shorter one, y) */
			/* Need to meet: y.length > x.length, or merge(x, y) */

			if(z_len <= x_len + y_len) {
				if(z_len <= x_len) {
					mergeRun(arr, z, y, cmp);
					sta.push({ z.first, y.second });
					sta.push(x);
				}
				else {
					mergeRun(arr, y, x, cmp);
					sta.push(z);
					sta.push({ y.first, x.second });
				}
				continue;
			}

			if(y_len <= x_len) {
				mergeRun(arr, y, x, cmp);
				sta.push(z);
				sta.push({ y.first, x.second });
				continue;
			}

			sta.push(z), sta.push(y), sta.push(x);
			break;
		}
		while(sta.size() >= 2) {
			x = sta.top(), sta.pop(), x_len = x.second - x.first + 1;
			y = sta.top(), sta.pop(), y_len = y.second - y.first + 1;

			if(y_len <= x_len) {
				mergeRun(arr, y, x, cmp);
				sta.push({ y.first, x.second });
				continue;
			}

			sta.push(y), sta.push(x);
			break;
		}
	}
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
void TimSort<T>::mergeRun(T [], const Run &a, const Run &b, Comparer cmp) {
	/* TODO: GALLOP Mode. */

	/* TODO: Less memory use method. */

	/* TODO: A simple merge to test slicing. */
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
typename TimSort<T>::size_t TimSort<T>::upperboundDec(const T arr[], const T &val, size_t l, size_t r, Comparer cmp) {
	size_t mid;
	while(l < r) {
		mid = (l + r) / 2;
		if(cmp(arr[mid], val)) {
			r = mid;
		}
		else {
			l = mid + 1;
		}
	}
	return cmp(arr[l], val) ? l : (l + 1);
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
