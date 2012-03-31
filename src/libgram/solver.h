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
#include <cassert>
#include <iostream>
#include <cmath>
#include <limits>

#include <libgram/query.h>
#include <libgram/emissionprovider.h>
#include <libgram/faststring.h>

#ifdef LIBGRAM_HAVE_BOOST
#	include <boost/unordered_map.hpp>
#	define LIBGRAM_SOLVER_DEFAULT_CONTAINER_NAME boost::unordered_map
#	define LIBGRAM_SOLVER_DEFAULT_CONTAINER_ARGS , boost::hash<libgram::FastString<Value> >
#else
#	ifdef LIBGRAM_HAVE_CXX0X
#		include <unordered_map>
#		define LIBGRAM_SOLVER_DEFAULT_CONTAINER_NAME std::unordered_map
#		define LIBGRAM_SOLVER_DEFAULT_CONTAINER_ARGS , std::hash<libgram::FastString<Value> >
#	else
#		ifdef LIBGRAM_HAVE_TR1
#			include <tr1/unordered_map>
#			define LIBGRAM_SOLVER_DEFAULT_CONTAINER_NAME std::tr1::unordered_map
#			define LIBGRAM_SOLVER_DEFAULT_CONTAINER_ARGS , std::hash<libgram::FastString<Value> >
#		else
#			include <map>
#			define LIBGRAM_SOLVER_DEFAULT_CONTAINER_NAME std::map
#			define LIBGRAM_SOLVER_DEFAULT_CONTAINER_ARGS
#		endif
#	endif
#endif

namespace libgram {

template<typename Value> class TemporaryState;

template<
		typename Value,
		typename Container = LIBGRAM_SOLVER_DEFAULT_CONTAINER_NAME<FastString<Value>, TemporaryState<Value>LIBGRAM_SOLVER_DEFAULT_CONTAINER_ARGS > >
class Solver {
private:
	EmissionProvider<Value> *m_emission_provider;
public:
	Solver() {
		m_emission_provider = NULL;
	}
	void setEmissionProvider(EmissionProvider<Value> *emission_provider) {
		m_emission_provider = emission_provider;
	}
	std::basic_string<Value> solve(Query<Value> &m_query);
};

template<typename Value>
class TemporaryState {
private:
	double m_cum_probability;
	double m_emission_probability;
	Value m_value;
	FastString<Value> m_gram;
	std::basic_string<Value> m_history;
public:
	TemporaryState() {
		m_cum_probability = -std::numeric_limits<double>::max();
		m_emission_probability = 0.0;
	}
	TemporaryState(FastString<Value> &gram, double emission_probability) {
		m_cum_probability = -std::numeric_limits<double>::max();
		m_emission_probability = emission_probability;
		m_value = gram.lastValue();
		m_gram = gram;
	}
	double relax(double cum_probability,
			const std::basic_string<Value> &history) {
		// We are already in logarithmic scale. Both cum_probability and m_emission_probability are in log scale.
		if (cum_probability + m_emission_probability > m_cum_probability) {
			m_cum_probability = cum_probability + m_emission_probability;
			if (m_value != 0)
				m_history = history + m_value;
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
std::basic_string<Value> Solver<Value, Container>::solve(Query<Value> &query) {
	typedef FastString<Value> Gram;
	Container temporary_states[2];
	const std::vector<QuerySection<Value> > &sections = query.sections();

	// Configuring environment
	unsigned int maximum_gram = m_emission_provider->maximumGram();

	// Creating initial temporary node
	Gram empty_gram(maximum_gram, (Value) 0); // TODO: See TODO below
	std::basic_string<Value> empty_history;
	TemporaryState<Value> initial_temporary_state(empty_gram, 1.0);
	initial_temporary_state.relax(log(1.0), empty_history /* history */);
	temporary_states[0][empty_gram] = initial_temporary_state;
	Gram gram_key(maximum_gram, (Value) 0); // TODO: We require from Value to have implicit constructor with numeric argument! Not good!
	double cum_probability;

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
		for (typename Container::iterator it = current_tmp_states.begin();
				it != current_tmp_states.end(); it++) {
			// For each such temporary state, we build new target state for the next step
			// using candidates list from current Query section
			for (unsigned int candidate_index = 0;
					candidate_index < section_probabilities.size();
					candidate_index++) {
				// Obsolete: Gram gram_key(it->first, section_values[candidate_index]);
				gram_key.acquire(it->first, section_values[candidate_index]);

				// Calculating probability in logarithmic scale
				cum_probability = it->second.cumProbability()
						+ log(section_probabilities[candidate_index]);
				if (new_tmp_states.find(gram_key) == new_tmp_states.end()) {
					// Creating new TemporaryState instance
					new_tmp_states[gram_key] = TemporaryState<Value>(gram_key,
							log(m_emission_provider->probability(gram_key)));
				}
				new_tmp_states[gram_key].relax(cum_probability,
						it->second.history());
			}
		}
	}

	// Searching the best last node
	double best_probability = 0.0;
	TemporaryState<Value> *best_final_state = NULL;
	Container &final_temporary_states = temporary_states[sections.size() % 2];
	for (typename Container::iterator it = final_temporary_states.begin();
			it != final_temporary_states.end(); it++) {
		double cum_probability = it->second.cumProbability();
		if (cum_probability > best_probability || best_final_state == NULL) {
			best_probability = cum_probability;
			best_final_state = &(it->second);
		}
	}

	// Return copy
	std::basic_string<Value> result = best_final_state->history();
	return result;
}

} // namespace libgram

#endif /* SOLVER_H_ */
