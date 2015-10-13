all: smac

smac: smac.cpp smac.hpp fzip.hpp spm.hpp
	g++ -std=gnu++11 -o smac smac.cpp

fzip.hpp: fzip/include/fzip.hpp
	cp fzip/include/fzip.hpp .

spm.hpp: spMatrixHelp/spm.hpp
	cp spMatrixHelp/spm.hpp .

run: smac example.mtx
	smac -c example.mtx dense.smac
	smac -d dense.smac exampleAfter.mtx
	cat exampleAfter.mtx

vim:
	vim -p makefile smac.cpp smac.hpp TODO fzip/include/fzip.hpp spMatrixHelp/spm.hpp

matrices.zip:
	wget http://www.nvidia.com/content/NV_Research/matrices.zip

%.mtx: matrices.zip
	unzip -f matrices.zip $@
