all: smac

smac: smac.cpp smac.hpp fzip.hpp
	g++ -std=gnu++11 -o smac smac.cpp

fzip.hpp: fzip/include/fzip.hpp
	cp fzip/include/fzip.hpp .

run: smac dense2.mtx
	smac -c dense.mtx dense.smac

vim:
	vim -p makefile smac.cpp smac.hpp TODO fzip/include/fzip.hpp

matrices.zip:
	wget http://www.nvidia.com/content/NV_Research/matrices.zip

%.mtx: matrices.zip
	unzip -f matrices.zip $@
