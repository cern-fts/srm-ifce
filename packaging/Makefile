NAME=srm-ifce
SPEC=rpm/$(NAME).spec
VERSION=${shell grep '^Version:' $(SPEC) | awk '{print $$2}' }
# Leave blank. To be overriden by CI tools.
RELEASE=

CWD=${shell pwd}

RPMBUILD=/tmp/rpmbuild
SRPMS=$(CWD)
RPMS=$(CWD)/out

MOCK_CHROOT=epel-6-cernonly-x86_64
MOCK_FLAGS=--verbose


RPMDEFINES_SRC=--define='_topdir $(RPMBUILD)' \
	--define='_sourcedir $(CWD)' \
	--define='_builddir %{_topdir}/BUILD' \
	--define='_srcrpmdir $(SRPMS)' \
	--define='_rpmdir $(RPMS)' \
    --define "_source_filedigest_algorithm md5"

RPMDEFINES_BIN=--define='_topdir $(RPMBUILD)' \
	--define='_sourcedir %{_topdir}/SOURCES' \
	--define='_builddir %{_topdir}/BUILD' \
	--define='_srcrpmdir $(SRPMS).' \
	--define='_rpmdir $(RPMS)' \
    --define "_binary_filedigest_algorithm md5"


PBUILDER_FLAGS=
PBUILDER_TMP="/tmp/"


all: srpm

clean:
	rm -fv *.tar.gz
	rm -fv *.rpm
	rm -fv *.log
	rm -rfv out
	rm -rfv "$(RPMBUILD)"
	rm -fv *.dsc *.gz *.changes

dist: clean
	tar vczf "$(NAME)-$(VERSION).tar.gz" --exclude="packaging" --exclude=".git*" --exclude="*.pyc" --transform="s,^,$(NAME)-$(VERSION)/," ..

$(RPMBUILD):
	mkdir -p "$(RPMBUILD)"

override_release: $(SPEC)
	$(if $(RELEASE), sed -i "s/Release:.*/Release: $(RELEASE)%{?dist}/g" "$(SPEC)")

srpm: dist $(SPEC) $(RPMBUILD) override_release
	/usr/bin/rpmbuild --nodeps -bs $(RPMDEFINES_SRC) $(SPEC)

rpm: srpm
	/usr/bin/rpmbuild --rebuild $(RPMDEFINES_BIN) $(NAME)-$(VERSION)-*.src.rpm

mock: srpm
	/usr/bin/mock $(MOCK_FLAGS) -r $(MOCK_CHROOT) $(NAME)-$(VERSION)-*.src.rpm

deb-src: dist
	rm -rf "$(PBUILDER_TMP)/$(NAME)-$(VERSION)"
	tar xzf "$(NAME)-$(VERSION).tar.gz" -C "$(PBUILDER_TMP)"
	cp -rv debian "$(PBUILDER_TMP)/$(NAME)-$(VERSION)"
	$(if $(RELEASE), sed -ri "s/($(NAME) )\((([0-9]+\.)+[0-9]+)-[0-9]+\)/\\1(\\2-$(RELEASE))/g" "$(PBUILDER_TMP)/$(NAME)-$(VERSION)/debian/changelog")
	cp -f "$(NAME)-$(VERSION).tar.gz" "$(NAME)_$(VERSION).orig.tar.gz"
	cp -f "$(NAME)-$(VERSION).tar.gz" "$(PBUILDER_TMP)/$(NAME)_$(VERSION).orig.tar.gz" 
	cd "$(PBUILDER_TMP)/$(NAME)-$(VERSION)"; \
		debuild -us -uc -S
	cp -f $(PBUILDER_TMP)/$(NAME)_$(VERSION)*.diff.gz .
	cp -f $(PBUILDER_TMP)/$(NAME)_$(VERSION)*.dsc .

deb: deb-src
	pbuilder build $(PBUILDER_FLAGS) $(NAME)_$(VERSION)*.dsc

