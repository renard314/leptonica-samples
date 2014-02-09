//============================================================================
// Name        : Examples.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <allheaders.h>

using namespace std;

void loadAndDisplayPix(){
	Pix* pixOrg = pixRead("12.jpg");
		pixDisplay(pixOrg,0,0);
	pixDestroy(&pixOrg);
}

void simpleFilters(){
	Pix* pixOrg = pixRead("2.jpg");
	Pix* pixg = pixConvertRGBToLuminance(pixOrg);
	Pixa* pixaDisp = pixaCreate(0);

	Pix* pixContrast = pixContrastNorm(NULL, pixg, 100, 100, 55, 4, 4);
	Pix* pixSharp = pixUnsharpMasking(pixg,5,0.4); //make image sharper by amplifying high frequency parts
	Pix* pixNorm = pixEqualizeTRC(NULL,pixg,0.5,1);//histogram equalization. brings out more detail
	Pix* pixEdge = pixTwoSidedEdgeFilter(pixg,L_VERTICAL_EDGES); //find edges

	pixWrite("edges.bmp",pixEdge,IFF_BMP);
	pixInvert(pixEdge,pixEdge);  //invert and binarize to bring out the edges
	Pix* pixb = pixThresholdToBinary(pixEdge,252);

	pixaAddPix(pixaDisp, pixOrg, L_CLONE);
	pixaAddPix(pixaDisp, pixContrast, L_CLONE);
	pixaAddPix(pixaDisp, pixSharp, L_CLONE);
	pixaAddPix(pixaDisp, pixNorm, L_CLONE);
	pixaAddPix(pixaDisp, pixb, L_CLONE);
	Pix* pixd = pixaDisplayTiledAndScaled(pixaDisp, 32, 600, 3, 0, 25, 2);
	pixDisplay(pixd,0,0);

	pixaDestroy(&pixaDisp);
	pixDestroy(&pixg);
	pixDestroy(&pixNorm);
	pixDestroy(&pixOrg);
	pixDestroy(&pixSharp);
	pixDestroy(&pixb);
	pixDestroy(&pixEdge);
	pixDestroy(&pixContrast);
	pixDestroy(&pixd);
}
void gnuplotExample(){
	Pix* pixg= pixRead("68.png");
	NUMA* histo = pixGetGrayHistogram(pixg, 1);
	NUMA* norm = numaNormalizeHistogram(histo, 1);
	//NUMA* closed = numaDilate(norm, 5);


	GPLOT *gplot;
	gplot = gplotCreate("root", GPLOT_X11, "name", "x", "y");
	ostringstream title;

	l_int32 splitIndex;
	//find a good threshold value
	//http://tpgit.github.io/Leptonica/leptprotos_8h.html#aaf2fcc746d17ba6d3ab9105000b263f2
	numaSplitDistribution(norm,0.2,&splitIndex,NULL,NULL,NULL,NULL,NULL);

	gplotAddPlot(gplot, NULL, norm, GPLOT_LINES, "norm");
	Numa* nax = numaCreate(0);
	Numa* nay = numaCreate(0);
	numaAddNumber(nax,splitIndex);
	numaAddNumber(nay,0.04);
	gplotAddPlot(gplot, nax,nay,GPLOT_IMPULSES,"split");
	gplotMakeOutput(gplot);
	gplotDestroy(&gplot);
	pixDestroy(&pixg);
	numaDestroy(&histo);
	numaDestroy(&nax);
	numaDestroy(&nay);
	numaDestroy(&norm);
}

void dewarp(){
	Pix* pixg = pixRead("3.png");
	NUMA* histo = pixGetGrayHistogram(pixg, 1);
	NUMA* norm = numaNormalizeHistogram(histo, 1);
	Pix* pixb;

    /* Do combination of contrast norm and sauvola */
	Pix* pixt1 = pixContrastNorm(NULL, pixg, 100, 100, 55, 1, 1);
	pixSauvolaBinarizeTiled(pixg, 8, 0.34, 1, 1, NULL, &pixb);


	L_DEWARP *dew = dewarpCreate(pixb, 0, 5, 10, 0);
	dewarpBuildModel(dew, 0);
	dewarpApplyDisparity(dew, pixb, 0);

	//display everything
	Pixa* pixaDisp = pixaCreate(0);
	pixaAddPix(pixaDisp, pixg, L_CLONE);
	pixaAddPix(pixaDisp, pixb, L_CLONE);
	pixaAddPix(pixaDisp, dew->pixd, L_CLONE);
	Pix* pixd = pixaDisplayTiledAndScaled(pixaDisp, 32, 800, 3, 0, 25, 2);
	pixDisplay(pixd,0,0);


	pixDestroy(&pixg);
	pixDestroy(&pixd);
	pixaDestroy(&pixaDisp);
	dewarpDestroy(&dew);
	pixDestroy(&pixg);
	pixDestroy(&pixt1);
	pixDestroy(&pixb);
	numaDestroy(&histo);
	numaDestroy(&norm);

}

int main() {
	l_chooseDisplayProg(L_DISPLAY_WITH_XV);
	//system(" echo $PATH"); //check out where to put xv binary


	//loadAndDisplayPix();
	//simpleFilters();
	//gnuplotExample();
	dewarp();

	return 0;
}
