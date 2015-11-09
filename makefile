all: smac

smac: smac.cpp smac.hpp fzip.hpp spm.hpp
	g++ -std=gnu++0x -o smac smac.cpp

fzip.hpp: fzip/include/fzip.hpp
	cp fzip/include/fzip.hpp .

spm.hpp: spMatrixHelp/spm.hpp
	cp spMatrixHelp/spm.hpp .

run: smac example.mtx
	smac -c example.mtx example.smac
	smac -d example.smac exampleAfter.mtx
	cat exampleAfter.mtx

checkPattern: smac examplePattern.mtx
	smac -c examplePattern.mtx example.smac
	smac -d example.smac exampleAfter.mtx
	cat exampleAfter.mtx

checkSymmetric: smac exampleSymmetric.mtx
	smac -c exampleSymmetric.mtx example.smac
	smac -d example.smac exampleAfter.mtx
	cat exampleAfter.mtx

checkEmptyRow: smac exampleEmptyRow.mtx
	smac -c exampleEmptyRow.mtx example.smac
	smac -d example.smac exampleAfter.mtx
	cat exampleAfter.mtx

checkMultipleFiles: smac example.mtx
	smac -c --multipleFiles=2 example.mtx example.smac
	smac -d example0.smac exampleAfter.mtx
	cat exampleAfter.mtx

vim:
	vim -p makefile smac.cpp smac.hpp TODO fzip/include/fzip.hpp spMatrixHelp/spm.hpp

matrices.zip:
	wget http://www.nvidia.com/content/NV_Research/matrices.zip

%.mtx: matrices.zip
	unzip -f matrices.zip $@

clean:
	rm -rf smac
