.PHONY: comssextractor comssextractor-clean

all: comssextractor
all-local: comssextractor

clean: comssextractor-clean
clean-local: comssextractor-clean

comssextractor:
	export MROOT="`pwd`/comssextractor" && make -C "$(top_srcdir)/resources/comssextractor/core" libs

comssextractor-clean:
	export MROOT="`pwd`/comssextractor" && make -C "$(top_srcdir)/resources/comssextractor/core" clean
