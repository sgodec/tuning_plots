#include "AtlasStyle.C"
#include "Scan.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TString.h"
#include "TStyle.h"
#include "TXMLEngine.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

std::map<std::string, std::vector<Scan>> xmlreadfile(const char *filename,
                                                     std::string &outpath);
void PlotScans(std::string, std::vector<Scan> VectorScans, std::string outpath);

void CreateSummaryPlots(const char *filename = "SummaryPlotConfig.xml") {
  SetAtlasStyle();
  std::cout << "Input XML file: " << filename << std::endl;
  std::string outpath = "";
  std::map<std::string, std::vector<Scan>> Scans =
      xmlreadfile(filename, outpath);

  for (std::map<std::string, std::vector<Scan>>::iterator iScans =
           Scans.begin();
       iScans != Scans.end(); ++iScans) {
    PlotScans(iScans->first, iScans->second, outpath);
  }
}

std::map<std::string, std::vector<Scan>> xmlreadfile(const char *filename,
                                                     std::string &outpath) {
  std::map<std::string, std::vector<Scan>> vscans;
  TXMLEngine *xml = new TXMLEngine;
  XMLDocPointer_t xmldoc = xml->ParseFile(filename);
  if (xmldoc == 0) {
    delete xml;
    return vscans;
  }
  std::cout << "Read file" << std::endl;
  XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);
  XMLAttrPointer_t mainattr = xml->GetFirstAttr(mainnode);

  while (mainattr != 0) {
    if (((std::string)xml->GetAttrName(mainattr)) == "outpath")
      outpath = (std::string)(xml->GetAttrValue(mainattr));
    mainattr = xml->GetNextAttr(mainattr);
  }
  XMLNodePointer_t child = xml->GetChild(mainnode);
  while (child != 0) {
    if (((std::string)xml->GetNodeName(child)) == "detector") {
      XMLAttrPointer_t attr = xml->GetFirstAttr(child);
      std::string chipSN;
      TString path;
      std::string name = "";
      while (attr != 0) {
        if (((std::string)xml->GetAttrName(attr)) == "chipSN")
          chipSN = (std::string)(xml->GetAttrValue(attr));
        if (((std::string)xml->GetAttrName(attr)) == "path")
          path = xml->GetAttrValue(attr);
        if (((std::string)xml->GetAttrName(attr)) == "name")
          name = (std::string)(xml->GetAttrValue(attr));
        attr = xml->GetNextAttr(attr);
      }
      std::cout << chipSN << std::endl;
      XMLNodePointer_t subchild = xml->GetChild(child);
      while (subchild != 0) {
        XMLAttrPointer_t subattr = xml->GetFirstAttr(subchild);
        int run = 0;
        while (subattr != 0) {
          if (((std::string)xml->GetAttrName(subattr)) == "run")
            run = atoi(xml->GetAttrValue(subattr));
          subattr = xml->GetNextAttr(subattr);
        }
        if (((std::string)xml->GetNodeName(subchild)) == "analog")
          vscans[name].push_back(Analog(run, chipSN, path));
        if (((std::string)xml->GetNodeName(subchild)) == "digital")
          vscans[name].push_back(Digital(run, chipSN, path));
        if (((std::string)xml->GetNodeName(subchild)) == "threshold")
          vscans[name].push_back(Threshold(run, chipSN, path));
        if (((std::string)xml->GetNodeName(subchild)) == "noise")
          vscans[name].push_back(Noise(run, chipSN, path));
        if (((std::string)xml->GetNodeName(subchild)) == "tot")
          vscans[name].push_back(Tot(run, chipSN, path));
        if (((std::string)xml->GetNodeName(subchild)) == "source")
          vscans[name].push_back(Source(run, chipSN, path));

        subchild = xml->GetNext(subchild);
      }
    }
    child = xml->GetNext(child);
  }
  return vscans;
}
void PlotScans(std::string name, std::vector<Scan> VectorScans, std::string outpath) {
  std::vector<TCanvas *> VectorCanvas;
  std::string FileOut = "SummaryPlots-" + name + ".pdf";
  for (unsigned iscan = 0; iscan < VectorScans.size(); iscan++) {
    VectorCanvas.push_back(new TCanvas(Form("c_%d", iscan), VectorScans.at(iscan).ScanType, 1200, 600));
    VectorCanvas.at(iscan)->Divide(2, 1);
    VectorCanvas.at(iscan)->cd(1);
    gStyle->SetOptTitle(1);
    VectorScans.at(iscan).Map->SetTitle(VectorScans.at(iscan).ScanType + " " + name.c_str());
    gPad->SetTopMargin(0.10);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    if (VectorScans.at(iscan).ScanType == "SOURCESCAN")
      VectorCanvas.at(iscan)->cd(1)->SetLogz();
    VectorScans.at(iscan).Map->Draw("colz");
    VectorCanvas.at(iscan)->cd(2);
    gPad->SetTopMargin(0.10);
    VectorScans.at(iscan).Distr->SetTitle("");
    VectorScans.at(iscan).Distr->Draw();

    // Set the fill style and color with transparency for the fit function
    VectorScans.at(iscan).FitFunction->SetFillColorAlpha(3, 0.35); // Green color with transparency
    VectorScans.at(iscan).FitFunction->SetFillStyle(1001); // Solid fill
                                                           //
    VectorScans.at(iscan).FitFunction->SetLineColor(2); // Set line color (e.g., red)
    VectorScans.at(iscan).FitFunction->SetLineStyle(2);

    // Draw the fit function with both the fill and the curve
    VectorScans.at(iscan).FitFunction->Draw("FC same"); 

    VectorScans.at(iscan).DistLegend->Draw();
    VectorCanvas.at(iscan)->SaveAs(((!iscan) ? (FileOut + "[") : FileOut).c_str());
    if (!iscan)
      VectorCanvas.at(iscan)->SaveAs((FileOut).c_str());
  }
  VectorCanvas.at(0)->SaveAs((FileOut + "]").c_str());
}
#ifndef __CINT__
int main(int argc, char *argv[]) {
  if (argc == 1) {
    CreateSummaryPlots();
  } else if (argc == 2) {
    CreateSummaryPlots(argv[1]);
  } else {
    std::cout << "Too many arguments!" << std::endl;
    std::cout << "Usage:    " << argv[0]
              << " <XML config file (default: SummaryPlotConfig.xml)>"
              << std::endl;
    std::cout << "Exit..." << std::endl;
  }
  return 0;
}
#endif


