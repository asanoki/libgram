/*
 * circularstring.h
 *
 *  Created on: Mar 28, 2012
 *      Author: asanoki
 */

#ifndef FASTSTRING_H_
#define FASTSTRING_H_

#include <cassert>
#include <cstring>
#include <iostream>

#include <boost/pool/pool_alloc.hpp>

namespace libgram {

template<class Type> class FastString;

template<class Type>
class FastString {
private:
	int m_size;
	Type *m_data;
public:
	FastString() {
		m_size = -1;
		m_data = 0;
	}
	FastString(int size, const Type symbol) :
			m_size(size) {
		m_data = new Type[size];
		// TO BE OPTIMIZED
		for (int i = 0; i < size; i++) {
			m_data[i] = symbol;
		}
	}
	FastString(int max_size, int size, const Type empty, const Type *input) :
			m_size(size) {
		m_data = new Type[size];
		// TO BE OPTIMIZED
		for (int i = 0; i < max_size; i++) {
			m_data[i] = empty;
		}
		// TO BE OPTIMIZED
		for (int i = max_size - size; i < max_size; i++) {
			m_data[i] = input[i];
		}
	}
	FastString(const FastString<Type> &parent) {
		m_size = parent.m_size;
		if (m_size == -1) {
			m_data = 0;
			return;
		}
		m_data = new Type[m_size];
		memcpy((char *) m_data, (char *) parent.m_data, sizeof(Type) * parent.m_size);
	}
	FastString(const FastString<Type> &parent, const Type symbol) {
		m_size = parent.m_size;
		m_data = new Type[m_size];
		memcpy((char *) m_data, (char *) (parent.m_data + 1), sizeof(Type) * (parent.m_size - 1));
		m_data[m_size - 1] = symbol;
	}
	FastString(const Type symbol, const FastString<Type> &parent) {
		m_size = parent.m_size;
		m_data = new Type[m_size];
		memcpy((char *) (m_data + 1), (char *) parent.m_data, sizeof(Type) * (parent.m_size - 1));
		m_data[0] = symbol;
	}
	void acquire(const FastString<Type> &parent, const Type symbol) {
		memcpy((char *) m_data, (char *) (parent.m_data + 1), sizeof(Type) * (parent.m_size - 1));
		m_data[parent.m_size - 1] = symbol;
	}
	void operator=(const FastString<Type>& parent) {
		if (m_size == -1) {
			m_size = parent.m_size;
			m_data = new Type[m_size];
		}
		//assert(m_size == parent.m_size);
		memcpy((char *) m_data, (char *) parent.m_data, sizeof(Type) * m_size);
	}
	bool equals(const FastString<Type> &reference) const {
		static int i;
		if (m_size != reference.m_size)
			return false;
		for (i = 0; i < m_size; i++) {
			if (reference.m_data[i] != m_data[i])
				return false;
		}
		return true;
	}
	Type &lastValue() {
		return m_data[m_size - 1];
	}
	void append(const Type symbol) {
		memmove((char *) m_data,
				(char *) (m_data + 1), sizeof(Type) * (m_size - 1));m_data[m_size - 1]= symbol;
	}
		std::basic_string<Type> convertToString() const {
			static Type *it;
			static int left;
			it = m_data;
			left = m_size;
			while (!it && left > 0) {
				it++;
				left--;
			}
			return std::basic_string<Type>(it, left);
		}
		size_t hash() const {
			static size_t hash;
			static int left;
			static Type *it;
			it = m_data;
			left = m_size;
			hash = 1;
			while (left--) {
				hash = hash * 101 + *it;
				it++;
			}
			return hash;
		}
		~FastString() {
			if (m_data)
			delete [] m_data;
		}
	};

inline std::size_t hash_value(const libgram::FastString<wchar_t>& f) {
	return f.hash();
}

inline bool operator==(const libgram::FastString<wchar_t>& a,
		const libgram::FastString<wchar_t>& b) {
	return a.equals(b);
}

} // namespace libgram

#endif /* FASTSTRING_H_ */
