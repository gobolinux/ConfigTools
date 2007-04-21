VERSION=
PROGRAM=ConfigTools

PACKAGE_DIR=$(HOME)
PACKAGE_ROOT=$(PACKAGE_DIR)/$(PROGRAM)
PACKAGE_BASE=$(PACKAGE_ROOT)/$(VERSION)
PACKAGE_FILE=$(PACKAGE_DIR)/$(PACKAGE_NAME)--$(VERSION)--$(shell uname -m).tar.bz2
CVSTAG=`echo $(PROGRAM)_$(VERSION) | tr "[:lower:]" "[:upper:]" | sed  's,\.,_,g'`

PYTHON_VERSION=2.3
PYTHON_LIBS=GenGrubMenu GenLiloConf MouseInfo
PYTHON_SITE=lib/python$(PYTHON_VERSION)/site-packages

all:
	make -C src all
	[ -e $(PYTHON_SITE) ] || mkdir -p $(PYTHON_SITE)
	for f in $(PYTHON_LIBS); \
	do libf=$(PYTHON_SITE)/$$f.py; \
	   rm -f $$libf; ln -nfs ../../../bin/$$f $$libf; \
	done
	cd $(PYTHON_SITE) && \
	for f in *.py; \
		do python -c "import `basename $$f .py`"; \
	done

clean :
	make -C src clean
	for f in $(PYTHON_LIBS); do libf=$(PYTHON_SITE)/$$f; rm -f $$libf.{pyo,pyc,py}; done

version_check:
	@[ "$(VERSION)" = "" ] && { echo -e "Error: run make with VERSION=<version-number>.\n"; exit 1 ;} || exit 0

dist: version_check all
	rm -rf $(PACKAGE_ROOT)
	mkdir -p $(PACKAGE_BASE)
	find * -not -path "*/CVS" -and -not -path "*/CVS/*" -and -not -path "*.py[oc]" -and -not -path "*~" | cpio -p $(PACKAGE_BASE)
	cd $(PACKAGE_DIR); tar cvp $(PROGRAM) | bzip2 > $(PACKAGE_FILE)
	rm -rf $(PACKAGE_ROOT)
	@echo; echo "Package at $(PACKAGE_FILE)"
	@echo; echo "Now run 'cvs tag $(CVSTAG)'"; echo
