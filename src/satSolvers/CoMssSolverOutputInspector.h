/*
 * CoMssSolverOutputInspector.h
 *
 *  Created on: 7 déc. 2016
 *      Author: lonca
 */

#ifndef SRC_SATSOLVERS_COMSSSOLVEROUTPUTINSPECTOR_H_
#define SRC_SATSOLVERS_COMSSSOLVEROUTPUTINSPECTOR_H_

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>


namespace CoQuiAAS {

class CoMssSolverOutputInspector {

	virtual bool isHardPartStatusLine(char *line) = 0;

	virtual bool getHardPartStatus(char *line) = 0;

	virtual bool isCoMssLine(char *line) = 0;

	virtual std::vector<int> getCoMss(char *line, int nSoftCstrs, FILE *file) = 0;

	virtual bool isModelLine(char *line) = 0;

	virtual std::vector<int> getModel(char *line) = 0;

};

}


#endif /* SRC_SATSOLVERS_COMSSSOLVEROUTPUTINSPECTOR_H_ */
