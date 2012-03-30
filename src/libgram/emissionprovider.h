/*
 * emissionprovider.h
 *
 *  Created on: Mar 13, 2012
 *      Author: asanoki
 */

#ifndef EMISSIONPROVIDER_H_
#define EMISSIONPROVIDER_H_

#include <string>
#include <libgram/faststring.h>

namespace libgram {

template<typename Value>
class EmissionProvider {
public:
	EmissionProvider() {
	}
	double virtual probability(const FastString<Value> &gram) = 0;
	int virtual maximumGram() = 0;
};

} // namespace libgram

#endif /* EMISSIONPROVIDER_H_ */
