/*
 * CoMssExtractorOutputInspectors.h
 *
 *  Created on: 7 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_SATSOLVERS_COMSSEXTRACTOROUTPUTINSPECTOR_H_
#define SRC_SATSOLVERS_COMSSEXTRACTOROUTPUTINSPECTOR_H_


#include "CoMssSolverOutputInspector.h"


namespace CoQuiAAS {

class CoMssExtractorOutputInspector : public CoMssSolverOutputInspector {

public:

	CoMssExtractorOutputInspector();

	virtual bool isHardPartStatusLine(char *line);

	virtual bool getHardPartStatus(char *line);

	virtual bool isCoMssLine(char *line);

	virtual std::vector<int> getCoMss(char *line, int nSoftCstrs, FILE *file);

	virtual bool isModelLine(char *line);

	virtual std::vector<int> getModel(char *line);

	~CoMssExtractorOutputInspector();
};

}


#endif /* SRC_SATSOLVERS_COMSSEXTRACTOROUTPUTINSPECTOR_H_ */
