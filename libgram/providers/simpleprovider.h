/*
 * simpleprovider.h
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#ifndef SIMPLEPROVIDER_H_
#define SIMPLEPROVIDER_H_

#include <cstdio>
#include <unordered_map>
#include <iostream>

#include <libgram/query.h>
#include <libgram/emissionprovider.h>

namespace libgrams {

template<typename Value, typename Container = std::unordered_map<
		std::basic_string<Value>, double> >
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
	double probability(const std::basic_string<Value> &gram);
	int maximumGram();
};

template<typename Value, typename Container>
void SimpleProvider<Value, Container>::setAutoEpsilon() {
	double minimum = -1.0;
	for (typename Container::const_iterator it = m_container->begin();
			it != m_container->end(); it++) {
		if (it->second < minimum || minimum < 0) {
			minimum = it->second;
		}
	}
	std::cerr
			<< "Automatic estimation of epsilon value is slow. You should do it explicite in your code."
			<< std::endl;
	m_epsilon = minimum / 2.0;
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
	std::cerr
			<< "Automatic estimation of maximum gram length is slow. You should do it explicite in your code."
			<< std::endl;
	m_maximum_gram = maximum;
}

template<typename Value, typename Container>
double SimpleProvider<Value, Container>::probability(
		const std::basic_string<Value> &gram) {
	if (m_epsilon == -1.0) {
		setAutoEpsilon();
	}
	if (m_container->find(gram) == m_container->end()) {
		return m_epsilon;
	}
	return (*m_container)[gram];
}

template<typename Value, typename Container>
double SimpleProvider<Value, Container>::epsilon() {
	if (m_epsilon < 0.0) {
		setAutoEpsilon();
	}
	assert(m_epsilon >= 0.0);
	return m_epsilon;
}

template<typename Value, typename Container>
int SimpleProvider<Value, Container>::maximumGram() {
	if (m_maximum_gram == -1) {
		setAutoMaximumGram();
	}
	assert(m_maximum_gram > 0);
	return m_maximum_gram;
}

} // namespace libgrams

#endif /* SIMPLEPROVIDER_H_ */
