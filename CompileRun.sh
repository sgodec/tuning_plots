g++ -o CreateSummaryPlots CreateSummaryPlots.C `root-config --cflags --glibs` -lXMLIO -std=c++17
#g++ -o CreateSummaryPlots CreateSummaryPlots.C -pthread -m64 -I/software/at3/root/root-5.30.01_slc5_gcc4.1_x86-64/include -L/software/at3/root/root-5.30.01_slc5_gcc4.1_x86-64/lib -lGui -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore -lCint -pthread -lm -ldl -rdynamic -lXMLIO -std=c++11

./CreateSummaryPlots
