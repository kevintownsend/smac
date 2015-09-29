all: smac

smac: smac.cpp smac.hpp
	g++ -o smac smac.cpp

run: smac dense2.mtx
	smac -c dense.mtx dense.smac

vim:
	vim -p makefile smac.cpp smac.hpp TODO

matrices.zip:
	wget http://www.nvidia.com/content/NV_Research/matrices.zip

%.mtx: matrices.zip
	unzip -f matrices.zip $@
