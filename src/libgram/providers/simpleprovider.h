/*
 * simpleprovider.h
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#ifndef SIMPLEPROVIDER_H_
#define SIMPLEPROVIDER_H_

#include <cstdio>
#include <iostream>

#include <libgram/query.h>
#include <libgram/emissionprovider.h>
#include <libgram/faststring.h>

namespace libgram {

template<typename Value, typename Container/* = std::unordered_map<
		std::basic_string<Value>, double> */>
class SimpleProvider: public EmissionProvider<Value> {
private:
	double m_epsilon;
	int m_maximum_gram;
	Container *m_container;
public:
	SimpleProvider() {
		m_epsilon = -1.0;
		m_maximum_gram = -1;
		m_container = NULL;
	}
	SimpleProvider(double epsilon) :
			m_epsilon(epsilon) {
		m_container = NULL;
		m_maximum_gram = -1;
	}
	void setContainer(Container *container) {
		m_container = container;
	}
	const Container *container() const {
		return container;
	}
	void setEpsilon(double epsilon) {
		m_epsilon = epsilon;
	}
	void setMaximumGram(int maximum_gram) {
		m_maximum_gram = maximum_gram;
	}
	void setAutoEpsilon();
	void setAutoMaximumGram();
	double epsilon();
	double probability(const FastString<Value> &gram);
	int maximumGram();
};

template<typename Value, typename Container>
void SimpleProvider<Value, Container>::setAutoEpsilon() {
	m_epsilon = 0.1;
}

template<typename Value, typename Container>
void SimpleProvider<Value, Container>::setAutoMaximumGram() {
	int maximum = -1;
	for (typename Container::const_iterator it = m_container->begin();
			it != m_container->end(); it++) {
		if ((int) it->first.length() > maximum) {
			maximum = it->first.length();
		}
	}
	m_maximum_gram = maximum;
}

template<typename Value, typename Container>
double SimpleProvider<Value, Container>::probability(
		const FastString<Value> &gram) {
	FastString<Value> subgram(0, gram);
	if (m_container->find(gram) == m_container->end()) {
		return m_epsilon;
	}
	if (m_container->find(subgram) == m_container->end()) {
		// WARNING: This should never happen!
		assert(m_container->find(subgram) != m_container->end());
	}
	return ((*m_container)[gram]) / ((*m_container)[subgram]);
}

template<typename Value, typename Container>
double SimpleProvider<Value, Container>::epsilon() {
	assert(m_epsilon >= 0.0);
	return m_epsilon;
}

template<typename Value, typename Container>
int SimpleProvider<Value, Container>::maximumGram() {
	assert(m_maximum_gram > 0);
	return m_maximum_gram;
}

} // namespace libgram

#endif /* SIMPLEPROVIDER_H_ */
