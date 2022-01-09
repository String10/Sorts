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
	void mergeRun(T [], Run, Run, Comparer);
	size_t lowerbound(const T [], const T &, size_t, size_t, Comparer);
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

	/* Resize the buf. */
	/* TODO: maybe it can cost less memory. */
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

	/* Slice the arr to different Run. */
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
			while(j + 1 < len && j - i + 1 <= min_run_len) {
				if(is_incr ^ (!cmp(arr[j + 1], arr[j]))) {
					if(!is_incr) {
						reverse(arr, i, j);
						is_incr = true;
					}
					pos = upperbound(arr, arr[j + 1], i, j, cmp), temp = arr[j + 1];
					for(size_t k = j + 1; k > pos; k--) {
						arr[k] = arr[k - 1];
					}
					arr[pos] = temp;
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
	Run run = sta.top();
	sta.pop();
	while(!sta.empty()) {
		mergeRun(arr, sta.top(), run, cmp);
		run.first = sta.top().first;
		sta.pop();
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
void TimSort<T>::mergeRun(T arr[], Run a, Run b, Comparer cmp) {
	/* TODO: GALLOP Mode. */

	/* Shorten the length. */
	if(!cmp(arr[b.first], arr[a.first])) {
		a.first = upperbound(arr, arr[b.first], a.first, a.second, cmp);
	}
	if(!cmp(arr[b.second], arr[a.second])) {
		b.second = lowerbound(arr, arr[a.second], b.first, b.second, cmp) - 1;
	}

	/* Copy-less-Method: 
	*  If a.length <= b.length, then copy a to buf and merge from small end.
	*  If a.length > b.length, then copy b to buf and merge from big end.
	*/

	size_t i, j, k;
	if(a.second - a.first + 1 <= b.second - b.first + 1) {
		for(i = a.first; i <= a.second; i++) {
			buf[i] = arr[i];
		}
		i = a.first, j = b.first, k = a.first;
		while(i <= a.second || j <= b.second) {
			if(i > a.second) {
				break;
			}
			if(j > b.second) {
				arr[k++] = buf[i++];
				continue;
			}
			if(cmp(arr[j], buf[i])) {
				arr[k++] = arr[j++];
			}
			else {
				arr[k++] = buf[i++];
			}
		}
	}
	else {
		for(j = b.first; j <= b.second; j++) {
			buf[j] = arr[j];
		}
		i = a.second, j = b.second, k = b.second;
		while(i >= a.first || j >= b.first) {
			if(j < b.first) {
				break;
			}
			if(i < a.first) {
				arr[k--] = buf[j--];
				continue;
			}
			if(cmp(buf[j], arr[i])) {
				arr[k--] = arr[i--];
			}
			else {
				arr[k--] = buf[j--];
			}
		}
	}
}

template <typename T>
typename TimSort<T>::size_t TimSort<T>::lowerbound(const T arr[], const T &val, size_t l, size_t r, Comparer cmp) {
	size_t mid;
	while(l < r) {
		mid = (l + r) / 2;
		if(cmp(arr[mid], val)) {
			l = mid + 1;
		}
		else {
			r = mid;
		}
	}
	return cmp(val, arr[l]) ? l : (l + 1);
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
