#ifndef __STAT_MAP_FACTORY_H__
#define __STAT_MAP_FACTORY_H__


#include <memory>

#include "DefaultStatMap.h"
#include "FakeStatMap.h"
#include "LogStatMap.h"


namespace CoQuiAAS {


class StatMapFactory {

public:

	static void createInstance(bool isFake) {
		if(isFake)
			StatMapFactory::instance = std::shared_ptr<StatMap>(std::make_shared<FakeStatMap>());
		else
			StatMapFactory::instance = std::shared_ptr<StatMap>(std::make_shared<LogStatMap>(stdout));
	}

	static std::shared_ptr<StatMap> getInstance() {

		return StatMapFactory::instance;
	}

public:
	static std::shared_ptr<StatMap> instance;
};

std::shared_ptr<StatMap> StatMapFactory::instance = nullptr;


}


#endif /* #ifndef __STAT_MAP_FACTORY_H__ */
