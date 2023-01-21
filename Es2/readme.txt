							G. Cowan
					       		RHUL Physics
					       		4 November 2020


                      TMVA Examples
                      -------------

The programs in this directory contain simple examples of multivariate
analyses using routines in the TMVA package.  The examples are
standalone C++ programs that must be linked with the ROOT and TMVA
libraries.   TMVA is included automatically as part of ROOT.

To build the programs, first download the files copying the directory
structure.  Alternatively you can copy the tarball tmvaExamples.tar to
your area and type

tar -xvf tmvaExamples.tar

This will automatically place everything in its correct subdirectory.

To build the programs, go into the corresponding subdirectory and type
make.  Provided the root6 environmen is set up correctly this should
produce the corresponding executable program.

Note that the subdirectory inc needs to contain the header file
Event.h.  This declares the class Event, which corresponds to a single
data event.  To allow one to store the data easily in a file, this
class contains only data members, no member functions.

For more information on the TMVA methods, see 

https://root.cern/doc/master/group__TMVA.html   (main website)
https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf    (manual)

The examples in the individual subdirectories are:

generateData -- generates data according to a 3-dimensional
	        distribution for two different hypotheses
		(signal and background) and stores the
		result in a root file (as two TTree objects).
		
		The generateData directory already contains root files
		with 10K signal and background events for training and
		10K of each type for testing.  You only need to use
		the generateData program if you want to generate more
		data or to change the hypotheses.

tmvaTrain --    reads in the root file and trains two
		simple multivariate discriminators:  a
		Fisher discriminant and a multilayer 
		perceptron (neural network).  The 
		coefficients for the discriminant functions
		are stored in a subdirectory ./train/weights/.

analyzeData --  reads in n-tuples of signal and background
		data and computes the discriminating variable.
		The user can select events, compute efficiency,
		purity, etc.

analyzeDataCon -- same as analyzeData but contains some additional
                code to plot the decision boundary.
