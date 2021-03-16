.PHONY: comssextractor-ng comssextractor-ng-clean

all: comssextractor-ng
all-local: comssextractor-ng

clean: comssextractor-ng-clean
clean-local: comssextractor-ng-clean

comssextractor-ng:
	export MROOT="`pwd`/comssextractor_ng" && make -C "$(top_srcdir)/resources/comssextractor_ng"

comssextractor-ng-clean:
	export MROOT="`pwd`/comssextractor_ng" && make -C "$(top_srcdir)/resources/comssextractor_ng" clean
