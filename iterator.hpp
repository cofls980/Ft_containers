#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include "iterator_traits.hpp"

namespace ft {

	template <class Iterator> class iterator_traits {
		public:
			typedef typename Iterator::difference_type		difference_type;
			typedef typename Iterator::value_type			value_type;
			typedef typename Iterator::pointer				pointer;
			typedef typename Iterator::reference			reference;
			typedef typename Iterator::iterator_category	iterator_category;
	};
	template <class T> class iterator_traits<T*>;
	template <class T> class iterator_traits<const T*>;

	template<typename Iterator>
	class iterator {
		public:
		typedef Iterator	iterator_type;
		typedef typename ft::iterator_traits<Iterator>::difference_type difference_type;
		typedef typename ft::iterator_traits<Iterator>::pointer pointer;
		typedef typename ft::iterator_traits<Iterator>::reference reference;
		typedef typename ft::iterator_traits<Iterator>::iterator_category iterator_category;
    };
}

#endif