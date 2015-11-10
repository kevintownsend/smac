all: smac

smac: smac.cpp smac.hpp fzip.hpp spm.hpp
	g++ -std=gnu++0x -o smac smac.cpp

fzip.hpp: fzip/include/fzip.hpp
	cp fzip/include/fzip.hpp .

spm.hpp: spMatrixHelp/spm.hpp
	cp spMatrixHelp/spm.hpp .

example.smac: smac example.mtx
	smac -c example.mtx example.smac
	smac -d example.smac exampleAfter.mtx
	cat exampleAfter.mtx

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

%.smac: %.mtx
	smac -c $< $@

allSmac: cant.smac  consph.smac  cop20k_A.smac  dense2.smac  example.smac  mac_econ_fwd500.smac  mc2depi.smac  pdb1HYS.smac  pwtk.smac  qcd5_4.smac  rail4284.smac  rma10.smac  scircuit.smac  shipsec1.smac  webbase-1M.smac

clean:
	rm -rf smac
