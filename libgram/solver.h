/*
 * solver.h
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <string>
#include <cstring>
#include <unordered_map>
#include <cassert>
#include <iostream>

#include <libgram/query.h>
#include <libgram/emissionprovider.h>

namespace libgrams {

template<typename Value> class TemporaryState;

template<typename Value, typename Container = std::unordered_map<
		std::basic_string<Value>, TemporaryState<Value> > >
class Solver {
private:
	Query<Value> *m_query;
	EmissionProvider<Value> *m_emission_provider;
public:
	Solver() {
		m_query = NULL;
		m_emission_provider = NULL;
	}
	void setQuery(Query<Value> *query) {
		m_query = query;
	}
	void setEmissionProvider(EmissionProvider<Value> *emission_provider) {
		m_emission_provider = emission_provider;
	}
	std::basic_string<Value> solve();
};

template<typename Value>
class TemporaryState {
private:
	double m_cum_probability;
	double m_emission_probability;
	Value m_value;
	std::basic_string<Value> m_gram;
	std::basic_string<Value> m_history;
public:
	TemporaryState() {
		m_cum_probability = -1.0;
		m_emission_probability = 0.0;
	}
	TemporaryState(std::basic_string<Value> gram, double emission_probability) {
		m_cum_probability = -1.0;
		m_emission_probability = emission_probability;
		if (gram.length() > 0) {
			m_value = gram[gram.length() - 1];
			std::wcout << "m_value: " << m_value << " from: " << gram << std::endl;
		}
		m_gram = gram;
	}
	double relax(double cum_probability,
			const std::basic_string<Value> &history) {
		if (cum_probability * m_emission_probability > m_cum_probability) {
			m_cum_probability = cum_probability * m_emission_probability;
			if (m_gram.length() > 0)
				m_history = history + m_value;
			std::wcout << "History is: " << m_history << " made from " << history << std::endl;
		}
		return m_cum_probability;
	}
	const double cumProbability() const {
		return m_cum_probability;
	}
	const double emissionProbability() const {
		return m_emission_probability;
	}
	const std::basic_string<Value> &history() const {
		return m_history;
	}
};

template<typename Value, typename Container>
std::basic_string<Value> Solver<Value, Container>::solve() {
	typedef std::basic_string<Value> Gram;
	Container temporary_states[2];
	const std::vector<QuerySection<Value> > &sections = m_query->sections();

	// Configuring environment
	unsigned int maximum_gram = m_emission_provider->maximumGram();

	// Creating initial temporary node
	Gram empty_gram;
	TemporaryState<Value> initial_temporary_state(empty_gram, 1.0);
	initial_temporary_state.relax(1.0, empty_gram);
	temporary_states[0][empty_gram] = initial_temporary_state;

	// Processing each section of Query data
	for (unsigned int index = 0; index < sections.size(); index++) {
		// References to vectors of probabilities and values (labels) of the current query section
		const std::vector<double> &section_probabilities =
				sections[index].probabilities();
		const std::vector<Value> &section_values = sections[index].values();
		assert(section_probabilities.size() == section_values.size());
		Container &current_tmp_states = temporary_states[index % 2];
		Container &new_tmp_states = temporary_states[(index + 1) % 2];
		new_tmp_states.clear();
		// We iterate threw all temporary states created in the previous step
		std::wcout << "------------------ " << index << std::endl;
		for (typename Container::iterator it = current_tmp_states.begin();
				it != current_tmp_states.end(); it++) {
			// For each such temporary state, we build new target state for the next step
			// using candidates list from current Query section
			for (unsigned int candidate_index = 0;
					candidate_index < section_probabilities.size();
					candidate_index++) {
				Gram gram_key;
				if (it->first.length() >= maximum_gram)
					 gram_key.append(it->first.substr(1, it->first.length() - 1));
				else
					gram_key.append(it->first);
				gram_key += section_values[candidate_index];
				// Calculating probability in normal scale.
				double cum_probability = it->second.cumProbability()
						* section_probabilities[candidate_index];
				// TODO: Change it to logarithmic scale.
				// double cum_probability = log(it->second.cumProbability()) + log(section_probabilities[candidate_index]); Correct?
				if (new_tmp_states.find(gram_key) == new_tmp_states.end()) {
					// Creating new TemporaryState instance
					new_tmp_states[gram_key] = TemporaryState<Value>(gram_key,
							m_emission_provider->probability(gram_key));
				}
				new_tmp_states[gram_key].relax(cum_probability,
						it->second.history());
			}
		}
	}

	// Finding the best last node
	std::wcout << "finding the best: " << std::endl;
	double best_probability = -1.0;
	TemporaryState<Value> *best_final_state = NULL;
	Container &final_temporary_states = temporary_states[sections.size() % 2];
	for (typename Container::iterator it = final_temporary_states.begin();
			it != final_temporary_states.end(); it++) {
		std::wcout << "final testing: " << it->second.history() << " = " << it->second.cumProbability() << std::endl;
		double cum_probability = it->second.cumProbability();
		if (cum_probability > best_probability) {
			std::wcout << "new final testing: " << it->second.history() << " = " << it->second.cumProbability() << std::endl;
			best_probability = cum_probability;
			best_final_state = &(it->second);
		}
	}

	// Return copy
	std::wcout << "resulting: " << best_final_state << std::endl;
	std::basic_string<Value> result = best_final_state->history();
	std::wcout << "resulting: " << result << std::endl;
	return result;
}

} // namespace libgrams

#endif /* SOLVER_H_ */
