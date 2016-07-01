# PATHS TO BE ADJUSTED BY USER

# path to ROOT installation
#ROOTDIR = /usr/local/root5
ROOTDIR = /afs/cern.ch/cms/slc5_amd64_gcc434/lcg/root/5.27.06b-cms18

# path to HepMC installation
#HepMCDIR = /usr
HepMCDIR = /afs/cern.ch/cms/slc5_amd64_gcc434/external/hepmc/2.05.01-cms2

#----------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------

INCS = `$(ROOTDIR)/bin/root-config --cflags` -I.
LIBS = `$(ROOTDIR)/bin/root-config --libs`

INCS += -I$(HepMCDIR)/include
LIBS += -L$(HepMCDIR)/lib -lHepMC

#----------------------------------------------------------------------------------------------------

GCC = g++ -Wall -O3 -fPIC
ROOTCINT = rootcint

# modules for which dictionary shall be generated
dict_modules = Model IslamModel PPPModel BSWModel BHModel JenkovszkyModel ExpModel InterpolationModel CoulombInterference Constants

# modules with no dictionaries
modules = Math Generator ModelFactory

# executable files
exe_files = ElegentTest ElegentTDistributionSampler ElegentBDistributionSampler ElegentSDistributionSampler

#----------------------------------------------------------------------------------------------------

# directories to create
dirsToCreate = obj lib bin #dict

dict_modules_obj = $(addprefix obj/, $(addsuffix .o, $(dict_modules)))
dict_modules_dict_src = $(addprefix dict/, $(addsuffix _dict.cc, $(dict_modules)))
dict_modules_dict_obj = $(addprefix dict/, $(addsuffix _dict.o, $(dict_modules)))

modules_obj = $(addprefix obj/, $(addsuffix .o, $(modules)))

exe_files_bin = $(addprefix bin/, $(exe_files))

#----------------------------------------------------------------------------------------------------

.PHONY: all dirs dicts libs exe info clean

all: libs exe

libs: dirs dicts lib/libElegent.so

exe: dirs libs $(exe_files_bin)

dirs: $(dirsToCreate)

$(dirsToCreate):
	mkdir -p $@

#dicts: $(dict_modules_dict_obj)

$(dict_modules_dict_obj) : dict/%_dict.o : dict/%_dict.cc
	$(GCC) $(INCS) -c $< -o $@

$(modules_dict_src) : dict/%_dict.cc : interface/%.h
	rm -f $@
	rm -f $(@:%.cc=%.h)
	$(ROOTCINT) $@ -c $<+

lib/libElegent.so: $(dict_modules_obj) $(modules_obj)
	@echo MAKING libElegent.so
	@$(GCC) -shared -olib/libElegent.so obj/*.o #dict/*.o

%.o : ../src/%.cc ../interface/%.h
	@echo COMPILING $<
	@$(GCC) $(INCS) -c $< -o $@

bin/% : src/%.cc lib/libElegent.so
	@echo BUILDING $@
	$(GCC) $(INCS) $(LIBS) -Llib -lElegent $< -o $@

info:
	echo $(dirsToCreate)
	@echo
	echo $(modules)
	@echo
	echo $(modules_obj)
	@echo
	echo $(modules_dict_src)
	@echo
	echo $(modules_dict_obj)

clean:
	rm -rf obj
	rm -rf lib
	#rm -rf dict
	rm -rf bin
