/*
 * query.h
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#ifndef QUERY_H_
#define QUERY_H_

#include <vector>

namespace libgram {

template<typename Value> class QuerySection {
private:
	std::vector<double> m_probabilities;
	std::vector<Value> m_values;
public:
	QuerySection(std::vector<double> probabilities, std::vector<Value> values) :
			m_probabilities(probabilities), m_values(values) {
	}
	QuerySection(double *probabilities, Value *values);
	const std::vector<double> &probabilities() const {
		return m_probabilities;
	}
	const std::vector<Value> &values() const {
		return m_values;
	}
};

template<typename Value> class Query {
	std::vector<QuerySection<Value> > m_sections;
public:
	Query() {
	}
public:
	void addSection(QuerySection<Value> section) {
		m_sections.push_back(section);
	}
	const std::vector<QuerySection<Value> > &sections() const {
		return m_sections;
	}
};

template<typename Value>
QuerySection<Value>::QuerySection(double *probabilities, Value *values) {
	while (probabilities) {
		m_probabilities.push_back(*probabilities);
		probabilities++;
	}
	while (values) {
		m_values.push_back(*values);
		values++;
	}
}

} // namespace libgram

#endif /* MODEL_H_ */
