all: smac

smac: smac.cpp smac.hpp fzip.hpp rcr.hpp rcrHelper.hpp
	g++ -std=gnu++11 -o smac smac.cpp rcrHelper.cpp

fzip.hpp: fzip/include/fzip.hpp
	cp fzip/include/fzip.hpp .

rcr.hpp:
	cp spMatrixHelp/rcr.hpp .

rcrHelper.hpp:
	cp spMatrixHelp/rcrHelper.hpp .

rcrHelper.cpp:
	cp spMatrixHelp/rcrHelper.cpp .

run: smac example.mtx
	smac -c example.mtx dense.smac

vim:
	vim -p makefile smac.cpp smac.hpp TODO fzip/include/fzip.hpp

matrices.zip:
	wget http://www.nvidia.com/content/NV_Research/matrices.zip

%.mtx: matrices.zip
	unzip -f matrices.zip $@
