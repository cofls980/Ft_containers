#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <limits>
#include "vector_iterator.hpp"
#include "../utils/reverse_iterator.hpp"
#include "../utils/is_integral.hpp"
#include "../utils/enable_if.hpp"
#include "../utils/equal.hpp"
#include "../utils/lexicographical_compare.hpp"

namespace ft {
	template < class T, class Alloc = std::allocator<T> > class vector {
		public:
			typedef T value_type;
			typedef Alloc allocator_type;
			typedef typename allocator_type::pointer pointer;
			typedef typename allocator_type::reference reference;
			typedef typename allocator_type::const_pointer const_pointer;
			typedef typename allocator_type::const_reference const_reference;
			typedef typename allocator_type::difference_type difference_type;
			typedef typename allocator_type::size_type size_type;
			typedef typename ft::vector_iterator<value_type> iterator;
			typedef typename ft::vector_iterator<const value_type> const_iterator;
			typedef typename ft::reverse_iterator<iterator> reverse_iterator;
			typedef typename ft::reverse_iterator<const_iterator> const_reverse_iterator;

		private:
			allocator_type _alloc;
			pointer _begin;
			pointer _end;
			pointer _end_cap;

		public:
			explicit vector (const allocator_type& alloc = allocator_type()) : _alloc(alloc), _begin(0), _end(_begin), _end_cap(_begin) {}
			explicit vector (size_type n, const value_type& val = value_type(), const allocator_type& alloc = allocator_type()) {
				_alloc = alloc;
				_begin = _alloc.allocate(n);
				_end = _begin;
				_end_cap = _begin + n;
				for (size_type i = 0;i < n;i++) {
					_alloc.construct(_end, val);
					_end++;
				}
			}
			template <class InputIterator>
			vector (InputIterator first, InputIterator last, const allocator_type& alloc = allocator_type(),
					typename ft::enable_if<!ft::is_integral<InputIterator>::value>::type* = 0) {
				size_t dist = last - first;
				_alloc = alloc;
				_begin = _alloc.allocate(dist);
				_end = _begin;
				_end_cap = _begin + dist;
				for (InputIterator it = first;it != last;it++) {
					_alloc.construct(_end, *it);
					_end++;
				}
			}
			vector (const vector& x) {
				_alloc = x.get_allocator();
				_begin = _alloc.allocate(x.capacity());
				_end = _begin;
				_end_cap = _begin + x.capacity();
				for (size_type i = 0;i < x.size();i++) {
					_alloc.construct(_end, *(x.begin() + i));
					_end++;
				}
			}

			~vector() {
				size_type curr_cap = capacity();
				clear();
				_alloc.deallocate(_begin, curr_cap);
			}

			vector& operator= (const vector& x) {
				if (this != &x) {
					size_type curr_cap = capacity();
					clear();
					_alloc.deallocate(_begin, curr_cap);
					_alloc = x.get_allocator();
					_begin = _alloc.allocate(x.size());
					_end_cap = _begin + x.size();
					_end = _begin;
					for (size_type i = 0;i < x.size();i++) {
						_alloc.construct(_end, *(x.begin() + i));
						_end++;
					}
				}
				return (*this);
			}
			
			iterator begin() {return (iterator(_begin));}
			const_iterator begin() const {return (const_iterator(_begin));}
			iterator end() {return (iterator(_end));}
			const_iterator end() const {return (const_iterator(_end));}
			reverse_iterator rbegin() {return (reverse_iterator(end()));}
			const_reverse_iterator rbegin() const {return (const_reverse_iterator(end()));}
			reverse_iterator rend() {return (reverse_iterator(begin()));}
			const_reverse_iterator rend() const {return (const_reverse_iterator(begin()));}

			size_type size() const {return (_end - _begin);}
			size_type max_size() const {
				return std::numeric_limits<size_type>::max() / (sizeof(value_type) == 1 ? sizeof(value_type) + 1 : sizeof(value_type));
			}
			size_type capacity() const {return (_end_cap - _begin);}
			bool empty() const {return (size() == 0);}
			void resize (size_type n, value_type val = value_type()) {
				size_type curr_size = size();
				if (n < curr_size) {
					for (size_type i = n;i < curr_size;i++) {
						pop_back();
					}
				}
				else if (n > curr_size) {
					if (n > capacity()) {
						do_allocate(capacity() * 2 > n ? capacity() * 2 : n);
					}
					for (size_type i = curr_size;i < n;i++) {
						_alloc.construct(_begin + i, val);
					}
				}
				_end = _begin + n;
			}
			void reserve (size_type n) {
				if (n > max_size()) {
					throw (std::length_error("vector"));
				}
				do_allocate(n);
			}
			
			reference operator[] (size_type n) {return (*(_begin + n));}
			const_reference operator[] (size_type n) const {return (*(_begin + n));}
			reference at (size_type n) {
				if (n >= size()) {
					throw std::out_of_range("vector");
				}
				return (*(_begin + n));
			}
			const_reference at (size_type n) const {
				if (n >= size()) {
					throw std::out_of_range("vector");
				}
				return (*(_begin + n));
			}
			reference front() {return (*_begin);}
			const_reference front() const {return (*_begin);}
			reference back() {return (*(_end - 1));}
			const_reference back() const {return (*(_end - 1));}
			
			template <class InputIterator>
			void assign (InputIterator first, InputIterator last,
						typename ft::enable_if<!ft::is_integral<InputIterator>::value>::type* = 0) {
				size_type it_size = last - first;
				if (it_size > capacity()) {
					do_allocate(it_size);
				}
				clear();
				for (size_type i = 0;i < it_size;i++) {
					_alloc.construct(_begin + i, *(first + i));
				}
				_end = _begin + it_size;
			}
			void assign (size_type n, const value_type& val) {
				if (n > capacity()) {
					do_allocate(n);
				} 
				clear();
				for (size_type i = 0;i < n;i++) {
					_alloc.construct(_begin + i, val);
				}
				_end = _begin + n;
			}

			void push_back (const value_type& val) {
				size_type curr_size = size();
				if (curr_size == capacity()) {
					do_allocate(curr_size == 0 ? 1 : curr_size * 2);
				}
				_alloc.construct(_end, val);
				_end++;
			}
			void pop_back() {
				if (_begin != _end) {
					--_end;
					_alloc.destroy(_end);
				}
			}
			
			iterator insert (iterator position, const value_type& val) {
				size_type dist = position - begin();
				size_type curr_size = size();
				size_type curr_cap = capacity();
				if (curr_size == curr_cap) {
					do_allocate(curr_cap == 0 ? 1 : curr_cap * 2);
				}
				for (size_type i = curr_size;i > dist;i--) {
					_alloc.construct(_begin + i, *(_begin + i - 1));
					_alloc.destroy(_begin + i - 1);
				}
				_alloc.construct(_begin + dist, val);
				_end++;
				return (begin() + dist);
			}
			void insert (iterator position, size_type n, const value_type& val) {
				if (n == 0) {
					return ;
				}
				size_type dist = position - begin();
				size_type curr_cap = capacity();
				size_type curr_size = size();
				if (curr_size + n > curr_cap) {
					do_allocate(curr_cap * 2 >= curr_size + n ? curr_cap * 2 : curr_size + n);
				}
				for (size_type i = curr_size;i > dist;i--) {
					_alloc.construct(_begin + i + n - 1, *(_begin + i - 1));
					_alloc.destroy(_begin + i - 1);
				}
				for (size_type i = 0;i < n;i++) {
					_alloc.construct(_begin + dist + i, val);
				}
				_end += n;
			}
			template <class InputIterator>
			void insert (iterator position, InputIterator first, InputIterator last,
						typename ft::enable_if<!ft::is_integral<InputIterator>::value>::type* = 0) {
				if (first == last) {
					return ;
				}
				size_type dist = position - begin();
				size_type it_size = last - first;
				size_type curr_cap = capacity();
				size_type curr_size = size();
				if (curr_size + it_size > curr_cap) {
					do_allocate(curr_cap * 2 >= curr_size + it_size ? curr_cap * 2 : curr_size + it_size);
				}
				for (size_type i = curr_size;i > dist;i--) {
					_alloc.construct(_begin + i + it_size - 1, *(_begin + i - 1));
					_alloc.destroy(_begin + i - 1);
				}
				for (size_type i = 0;i < it_size;i++) {
					_alloc.construct(_begin + dist + i, *(first + i));
				}
				_end += it_size;
			}

			iterator erase (iterator position) {
				if (position == end()) {
					return (position);
				}
				size_type dist = position - begin();
				_alloc.destroy(_begin + dist);
				for (size_type i = dist;i < size() - 1;i++) {
					_alloc.construct(_begin + i, *(_begin + i + 1));
					_alloc.destroy(_begin + i + 1);
				}
				_end--;
				return begin() + dist;
			}
			iterator erase (iterator first, iterator last) {
				if (first == end() || first == last) {
					return (first);
				}
				size_type it_size = last - first;
				size_type dist = first - begin();
				for (size_type i = 0;i < it_size;i++) {
					_alloc.destroy(_begin + dist + i);
				}
				for (size_type i = dist;i < size() - it_size;i++) {
					_alloc.construct(_begin + i, *(_begin + i + it_size));
					_alloc.destroy(_begin + i + it_size);
				}
				_end -= it_size;
				return begin() + dist;
			}

			void swap (vector& x) {
				if (this == &x)
					return;
				std::swap(x._begin, _begin);
				std::swap(x._end, _end);
				std::swap(x._end_cap, _end_cap);
				std::swap(x._alloc, _alloc);
			}

			void clear() {
				while (_begin != _end) {
					--_end;
					_alloc.destroy(_end);
				}
			}
			
			allocator_type get_allocator() const {return (_alloc);}

		private:
			void do_allocate(size_type n) {
				if (n > capacity()) {
					pointer new_begin = _alloc.allocate(n);
					size_type curr_size = size();
					size_type curr_cap = capacity();
					for (size_type i = 0;i < curr_size;i++) {
						_alloc.construct(new_begin + i, *(_begin + i));
					}
					clear();
					_alloc.deallocate(_begin, curr_cap);
					_begin = new_begin;
					_end = _begin + curr_size;
					_end_cap = _begin + n;
				}
			}
	};
	
	template <class T, class Alloc>
	bool operator== (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
		if (lhs.size() != rhs.size()) {
			return (false);
		}
		return (ft::equal(lhs.begin(), lhs.end(), rhs.begin()));
	}
	template <class T, class Alloc>
	bool operator!= (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
		return (!(lhs == rhs));
	}
	template <class T, class Alloc>
	bool operator<  (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
		return ft::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}
	template <class T, class Alloc>
	bool operator<= (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
		return (!(rhs < lhs));
	}
	template <class T, class Alloc>
	bool operator>  (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
		return (rhs < lhs);
	}
	template <class T, class Alloc>
	bool operator>= (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs) {
		return (!(lhs < rhs));
	}
	template <class T, class Alloc>
	void swap (vector<T,Alloc>& x, vector<T,Alloc>& y) {
		x.swap(y);
	}
}

#endif