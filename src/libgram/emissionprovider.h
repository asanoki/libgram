/*
 * emissionprovider.h
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#ifndef EMISSIONPROVIDER_H_
#define EMISSIONPROVIDER_H_

#include <string>

namespace libgram {

template<typename Value>
class EmissionProvider {
public:
	EmissionProvider() {
	}
	double virtual probability(const std::basic_string<Value> &gram) = 0;
	int virtual maximumGram() = 0;
};

} // namespace libgram

#endif /* EMISSIONPROVIDER_H_ */
