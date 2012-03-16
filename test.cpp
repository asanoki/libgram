/*
 * test.cpp
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#include <libgram/solver.h>
#include <libgram/query.h>
#include <libgram/providers/simpleprovider.h>

#include <unordered_map>
#include <string>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cassert>
#include <iostream>

void loadNGrams(std::string filename, unsigned int n,
		std::unordered_map<std::wstring, double> &result) {
	// Loading n-grams from a file
	std::wifstream data(filename);
	assert(data.good());
	std::wstring line;
	std::wstring line_gram;
	int line_value;
	while (!data.eof()) {
		std::getline(data, line);
		if (line.length() < n)
			continue;
		line_gram = line.substr(0, n);
		std::wstringstream stream(
				line.substr(n + 1, line.length() - n - 1).c_str());
		stream >> line_value;
		result[line_gram] = line_value;
	}
}

void loadQuery(std::string filename, libgrams::Query<wchar_t> &query) {
	// Loading query from a file
	std::wifstream data(filename);
	assert(data.good());
	std::wstring line_character;
	double line_probability;
	std::vector<wchar_t> labels;
	std::vector<double> emission_probabilities;
	while (!data.eof()) {
		data >> line_character;
		if (data.eof())
			continue;
		data >> line_probability;
		if (line_probability < 0) {
			libgrams::QuerySection<wchar_t> query_section(
					emission_probabilities, labels);
			query.addSection(query_section);
			labels.clear();
			emission_probabilities.clear();
		} else {
			assert(line_character.length() > 0);
			labels.push_back(line_character[0]);
			emission_probabilities.push_back(line_probability);
		}
	}
	std::wcout << "Sections: " << query.sections().size() << std::endl;
}

int main(int argc, char **argv) {
	// Configure locale to system locale. Be sure to set it to compatible with dictionaries.
	std::locale::global(std::locale(""));
	std::wcout << L"Locale configured" << std::endl;

	// Data with n-grams
	std::wcout << L"Creating containers for dictionaries" << std::endl;
	std::unordered_map<std::wstring, double> map;
	std::unordered_map<std::wstring, double> map_small;

	// Load data
	std::wcout << L"Loading n-grams..." << std::endl;
	loadNGrams("/opt/home/fedora/ngrams-3.txt", 3, map);
	loadNGrams("/opt/home/fedora/ngrams-2.txt", 2, map);

	// Convert occurences to probabilities
	std::wcout << L"Merging n-grams and calculating probabilities..."
			<< std::endl;
	double ngrams_sum = 0;
	for (std::unordered_map<std::wstring, double>::iterator it = map.begin();
			it != map.end(); it++) {
		ngrams_sum += it->second;
	}
	for (std::unordered_map<std::wstring, double>::iterator it = map.begin();
			it != map.end(); it++) {
		map[it->first] = it->second / ngrams_sum;
	}

	// Load query for the problem
	std::wcout << L"Loading query" << std::endl;
	libgrams::Query<wchar_t> query;
	loadQuery("/opt/home/fedora/query.txt", query);

	// Create and initialize the emission probability provider
	std::wcout << L"Initializing provider" << std::endl;
	libgrams::SimpleProvider<wchar_t> provider;
	provider.setContainer(&map);

	// Create and initialize the solver
	std::wcout << L"Initializing solver" << std::endl;
	libgrams::Solver<wchar_t> solver;
	solver.setQuery(&query);
	solver.setEmissionProvider(&provider);

	// Solving
	std::wcout << L"Executing solver..." << std::endl;
	solver.solve();

	return 0;
}
