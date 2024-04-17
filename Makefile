#*******************************************************************************
# Makefile for a Video Disk Recorder plugin                                    *
#                                                                              *
# version: 0.0.3ng-wirbel, 20160508                                            *
#*******************************************************************************

#*******************************************************************************
PLUGIN = control
VERSION = $(shell grep 'static const char \*VERSION *=' $(PLUGIN).c | awk '{ print $$6 }' | sed -e 's/[";]//g')
#*******************************************************************************



### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell PKG_CONFIG_PATH="$$PKG_CONFIG_PATH:../../.." pkg-config --variable=$(1) vdr))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
#
TMPDIR ?= /tmp



# uncomment next line for quiet compiler messages
Q = @

### The compiler options:
export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

### The version number of VDR's plugin API:
APIVERSION = $(call PKGCFG,apiversion)


### Allow user defined options to overwrite defaults:
-include $(PLGCFG)


### The name of the distribution archive:
ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)



### The name of the shared object file:
SOFILE = libvdr-$(PLUGIN).so




#*******************************************************************************
INCLUDES +=
DEFINES +=-D_GNU_SOURCE -DPLUGIN_NAME_I18N='"control"'
OBJS = control.o tcpsvr.o osdstate.o keyboard.o telnet.o global.o
#*******************************************************************************





#*******************************************************************************
### The main target:
all: $(SOFILE) i18n

#*******************************************************************************
### Implicit rules:
%.o: %.c
	@echo CC $@
	$(Q)$(CXX) -std=c++11 $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<


#*******************************************************************************
### Dependencies:

MAKEDEP = $(CXX) -std=c++11 -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	$(Q)@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@
-include $(DEPFILE)



#*******************************************************************************
### Internationalization (I18N):
PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	@echo MO $@
	$(Q)msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	@echo GT $@
	$(Q)xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<see README>' -o $@ `ls $^`

%.po: $(I18Npot)
	@echo PO $@
	$(Q)msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@echo IN $@
	$(Q)install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)





#*******************************************************************************
# TARGETS.
#*******************************************************************************
$(SOFILE): $(OBJS)
	@echo LD $@
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) -o $@


install-lib: $(SOFILE)
	@echo IN $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)
	$(Q)install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)


install: install-lib install-i18n



dist: $(I18Npo) clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz



clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.so.$(APIVERSION) *.tgz core* *~
