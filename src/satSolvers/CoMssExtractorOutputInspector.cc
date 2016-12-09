/*
 * CoMssExtractorOutputInspectors.cc
 *
 *  Created on: 7 déc. 2016
 *      Author: lonca
 */


#include "CoMssExtractorOutputInspector.h"


using namespace CoQuiAAS;


#define EXTERNAL_COMSS_BUFFER_SIZE (1<<20)


CoMssExtractorOutputInspector::CoMssExtractorOutputInspector() {}


bool CoMssExtractorOutputInspector::isHardPartStatusLine(char *line) {
	if(!strncmp(line, "s HARD PART UNSATISFIABLE", 25)) return true;
	if(!strncmp(line, "s UNSATISFIABLE", 15)) return true;
	return false;
}


bool CoMssExtractorOutputInspector::getHardPartStatus(char *line) {
	return !strncmp(line, "s UNSATISFIABLE", 15);
}


bool CoMssExtractorOutputInspector::isCoMssLine(char *line) {
	return !strncmp(line, "v ", 2);
}


std::vector<int> CoMssExtractorOutputInspector::getCoMss(char *line, int nSoftCstrs, FILE *file) {
	char *pc = line+2;
	int nb = 0;
	bool readingNb = false;
	std::vector<int> tmpMss;
	for(int i=1; i<=nSoftCstrs; ++i) tmpMss.push_back(i);
	for(; *pc != '\n'; ++pc) {
		if(!*pc) {
			fgets(line, EXTERNAL_COMSS_BUFFER_SIZE, file);
			pc = line;
		}
		if((*pc == ' ' || *pc == '\t') && readingNb) {
			if(!nb) break;
			tmpMss[nb-1] = -1;
			nb = 0;
			readingNb = false;
		} else if(*pc >= '0' && *pc <= '9') {
			nb = 10*nb + (*pc - '0');
			readingNb = true;
		}
	}
	std::vector<int> newMss;
	for(unsigned int i=0; i<tmpMss.size(); ++i) {
		if(tmpMss[i] > 0) {
			newMss.push_back(tmpMss[i]);
		}
	}
	return newMss;
}


bool CoMssExtractorOutputInspector::isModelLine(char *line) {
	return false;
}


std::vector<int> CoMssExtractorOutputInspector::getModel(char *line) {
	std::cerr << "model not handled by coMssExtractor-like solvers" << std::endl;
	std::exit(2);
	std::vector<int> fakeVec;
	return fakeVec;
}


CoMssExtractorOutputInspector::~CoMssExtractorOutputInspector() {}
