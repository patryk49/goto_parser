#include "SPL/Arrays.hpp"
#include <stdio.h>
#include <stdlib.h>


// SCANNIG FOR NUMBERS
template<class A>
Array<char> read_text(FILE *input, A &allocator, size_t expected_length = 64) noexcept{
	Memblock initial_memory = alloc(allocator, expected_length);
	Array<char> res = {initial_memory.ptr, 0, initial_memory.size};
	
	for (;;){
		int c = getc(input);
		[[unlikely]] if (c == EOF) break;

		if (res.size == res.cap){
			auto[new_ptr, new_cap] = realloc(allocator, Memblock{res.ptr, res.cap}, res.size*2);
			if (new_ptr == nullptr){
				free(allocator, Memblock{res.ptr, res.cap});
				return Array<char>{nullptr, 0, 0};
			}
			res.ptr = new_ptr;
			res.cap = new_cap;
		}

		res[res.size] = (char)c;
		++res.size;
	}
	return res;
}

/*
template<class A>
Array<Span<char>> scan_text(FILE *input, A &allocator, Span<char> delim) noexcept{
	Array<char> res = {alloc(allocator, 128), 0};

	uint8_t delim_counter[32];
	for (auto &it : delim_counter) it = 0;

	for (;;){
		int c = getc(input);
		[[unlikely]] if ((char)c==delim || c==EOF) break;

		if (res.size == res.data.size){
			Memblock new_data = realloc(res.data, allocator, res.size*2);
			if (new_data.ptr == nullptr){
				free(allocator, res.data);
				return Array<char>{{nullptr, 0}, 0};
			}
			res.data = new_data;
		}

		res[res.size] = (char)c;
		++res.size;
	}
	return res;
}
*/

template<class T>
struct ReadDataRes{ T res; bool not_found; char last; };

template<class T> static
ReadDataRes<T> read_data(FILE *input) noexcept{
	ReadDataRes<T> res;
	res.not_found = false;

	if constexpr (std::is_integral_v<T>){
		if constexpr (std::is_same_v<T, char>){
			res.res = getc(input);
			res.last = getc(input);
			return res;
		} else{
			do{ res.last = getc(input); } while (res.last==' ' || res.last=='\t');
			if constexpr (std::is_unsigned_v<T>){
				if (res.last<'0' || res.last>'9'){
					res.not_found = true;
					return res;
				}
		
				res.res = (T)((int8_t)res.last - '0');
				for (;;){
					res.last = getc(input);
					if (res.last<'0' || res.last>'9') return res;
					res.res = res.res*10 + (T)((int8_t)res.last-'0');
				}
			} else{
				bool neg = false;
				if (res.last=='+' || res.last=='-'){
					neg = res.last == '-';
					res.last = getc(input);
				}

				if (res.last<'0' || res.last>'9'){
					res.not_found = true;
					return res;
				}

				res.res = (T)((int8_t)res.last - '0');
				for (;;){
					res.last = getc(input);
					if (res.last<'0' || res.last>'9'){
						if (neg) res.res = -res.res;
						return res;
					}
					res.res = res.res*10 + (T)((int8_t)res.last-'0');
				}
			}
		}
	} else if constexpr (std::is_floating_point_v<T>){
		do{ res.last = getc(input); } while (res.last==' ' || res.last=='\t');
		bool neg = false;

		if (res.last=='+' || res.last=='-'){
			neg = res.last == '-';
			res.last = getc(input);
		}
		
		if (res.last<'0' || res.last>'9'){
			res.not_found = true;
			return res;
		}

		res.res = (T)((int8_t)res.last - '0');
		for (;;){
			res.last = getc(input);
			if (res.last == '.') break;
			if (res.last<'0' || res.last>'9'){
				if (neg) res.res = -res.res;
				return res;
			}
			res.res = res.res*10.0 + (T)((int8_t)res.last-'0');
		}
		
		for (double scale = 0.1;; scale *= 0.1){
			res.last = getc(input);
			if (res.last<'0' || res.last>'9'){
				if (neg) res.res = -res.res;
				return res;
			}
			res.res += (T)((int8_t)res.last-'0') * scale;
		}
	} else if constexpr (std::is_same_v<T, char>){
		int i = getc(input);
		res.err = i == EOF;
		res.res = (char)i;
		return res;
	} else if constexpr(is_tuple<T>){
		auto[head_data, err, last] = read_data<decltype(res.res.head)>(input);
		res.last = last;
		if (err){ res.not_found = true; return res; }
		res.res.head = head_data;

		if constexpr (len(res.res) != 1){
			auto[tail_data, err, last] = read_data<decltype(res.res.tail)>(input);
			res.last = last;
			if (err){ res.not_found = true; return res; }
			res.res.tail = tail_data;
		}
		
		return res;
	} else{
		static_assert(true, "wrong type");
	}
}


template<class T>
struct ReadArrayRes{
	Array<T> res = {nullptr, 0, 0};
	bool alloc_error = false;
	char last;
};

template<class T, class A> static
ReadArrayRes<T> read_array(FILE *input, A &allocator, char separator = ' ') noexcept{
	ReadArrayRes<T> res = {};
	ReadDataRes<T> elem;
	for (;;){
		elem = read_data<T>(input);
		[[unlikely]] if (elem.not_found) break;
	
		res.alloc_error = push_value(res.res, elem.res, allocator);
		if (res.alloc_error) break;
		
		if (elem.last != separator) break;
	}
	res.last = elem.last;
	return res;
}


template<class T>
struct ReadMatrixRes{
	Array<T> res = {};
	size_t rows = 0;
	size_t cols;
	bool col_mismatch = false;
	bool alloc_error = false;
	char last;
};

template<class T, class A> static
ReadMatrixRes<T> read_matrix(
	FILE *input,
	A &allocator,
	char col_separator = ' ',
	char row_separator = '\n'
) noexcept{
	ReadMatrixRes<T> res = {};

	auto[array, alloc_err, last] = read_array<T>(input, allocator, col_separator);
	res.cols = array.size;
	res.alloc_error = alloc_err;
	[[unlikely]] if (res.cols == 0) goto Return;
	res.rows += 1;
	[[unlikely]] if (res.alloc_error) goto Return;

	while (last == row_separator){
		for (size_t i=0; i!=res.cols; i+=1){
			auto elem = read_data<T>(input);
			last = elem.last;

			if (elem.not_found){
				res.col_mismatch = i!=0;
				goto Return;
			}
		
			res.alloc_error = push_value(array, elem.res, allocator);
			[[unlikely]] if (res.alloc_error) goto Return;
		}
		res.rows += 1;
	}
Return:
	res.res = array;
	res.last = last;
	return res;
}
