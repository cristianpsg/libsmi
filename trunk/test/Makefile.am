
test: test-smilint


test-smilint: .PHONY
	rm -rf test-smilint
	if [ ! -d test-smilint ] ; then mkdir test-smilint ; fi
	for d in `echo @smipath@ | sed -e 's/:/ /g'` ; do \
	    for f in $$d/* ; do \
		echo $$f ; \
		../tools/smilint -l9 $$f > test-smilint/`basename $$f` 2>&1 ; \
	    done ; \
	done
	@echo ""
	@echo "see directory test-smilint for results."
