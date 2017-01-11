#ifndef __EXTENSION_UTILS_H__
#define __EXTENSION_UTILS_H__


#include <vector>

#include "Attacks.h"


namespace CoQuiAAS {

class ExtensionUtils {

public:

	ExtensionUtils(Attacks &attacks);

	std::vector<int> groundedExtension();

	virtual ~ExtensionUtils();

private:

	Attacks &attacks;
};

}

#endif
