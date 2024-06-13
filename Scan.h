#ifndef SCAN_H
#define SCAN_H

#include <iostream>
//#include "/software/at3/root/root-5.30.01_slc5_gcc4.1_x86-64/tutorials/fit/langaus.C"
#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TString.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TF1.h"

class Scan {
public:
  int       RunNum;
  float     DistMinBin;
  float     DistMaxBin;
  float     DistBinSize;
  double    ToTMPV;
  double    ToTMPVError;
  TFile *   File;
  TH2D  *   Map;
  TH1D  *   Distr;
  TF1   *   FitFunction;
  TString   ScanFilePath;
  TString   HistoName;
  TString   ScanType;
  TString   DistXaxisTitle;
  TLegend * DistLegend;
  std::string    ChipSN;
  //  ~Scan() { File->Close(); }                                                                                                                                                                                                                                                                          
  void      LoadFile() {
    std::cout << "Scan :" << std::endl << ScanFilePath << std::endl;
    File = 0;
    File = TFile::Open(ScanFilePath) ;

  }
  virtual void      LoadHisto(){
    if(File->IsZombie()){
      std::cout << "File does not exist: creating dummy histos" << std::endl ;
      Map   = new TH2D("NoFileMap","",1,0,1,1,0,1);
      Distr = new TH1D("NoFileDist","",1,0,1);
      return;
    }
    std::cout << "Histo :" << std::endl << HistoName   << std::endl;
    Map   = (TH2D*) File->Get(HistoName);
    Map->GetXaxis()->SetTitle("Columns");
    Map->GetYaxis()->SetTitle("Rows");
    Map->SetMinimum(0);
    Distr  = new TH1D(ScanType+Form("_%06d",RunNum),"",(int) ((DistMaxBin-DistMinBin)/DistBinSize), DistMinBin,DistMaxBin);
    for(int icol=0; icol<80; icol++)
      for(int irow=0; irow<336; irow++)
        Distr->Fill(Map->GetBinContent(icol+1,irow+1));
    Distr->SetLineColor(kBlue);
    Distr->SetFillColor(kBlue);
    Distr->SetFillStyle(3004 );
    Distr->GetXaxis()->SetTitle(DistXaxisTitle);
    Distr->GetYaxis()->SetTitle("Pixels");
    FitDist();
    SetDistLegend();
  }
  virtual void FitDist()       {
    FitFunction = new TF1("f","",-9990,-9991);
  }
  virtual void SetDistLegend() {
    DistLegend = new TLegend();
  }
};

class Digital : public Scan {
public:
  Digital(int run, std::string chipSN,  TString ScanPath) {
    ScanType     = "DIGITAL_TEST"; DistMinBin=0; DistMaxBin=60; DistBinSize=1; DistXaxisTitle="Passed Digital Test";
    RunNum       = run;
    ScanFilePath = ScanPath+Form("DIGITAL_TEST_%06d/histos.root",run);
    HistoName    = Form("loop1_0/Mod_%s_Occupancy_Point_000",chipSN.c_str());
    LoadFile();
    LoadHisto();
  }
};

class Analog : public Scan {
public:
  Analog(int run, std::string chipSN,  TString ScanPath) {
    ScanType     = "ANALOG_TEST"; DistMinBin=0; DistMaxBin=60; DistBinSize=1; DistXaxisTitle="Passed Analog Test";
    RunNum       = run;
    ScanFilePath = ScanPath+Form("ANALOG_TEST_%06d/histos.root",run);
    HistoName    = Form("Mod_%s_Occupancy_Point_000",chipSN.c_str());
    LoadFile();
    LoadHisto();
  }
};

class Threshold : public Scan {
public:
  Threshold(int run, std::string chipSN,  TString ScanPath) {
    ScanType     = "THRESHOLD_SCAN"; DistMinBin=0; DistMaxBin=3500; DistBinSize=25; DistXaxisTitle="Threshold [e^{-}]";
    RunNum       = run;
    ScanFilePath = ScanPath+Form("THRESHOLD_SCAN_%06d/analysis.root",run);
    HistoName    = Form("thresh2d_Mod_%s",chipSN.c_str());
    LoadFile();
    LoadHisto();
  }
  void FitDist()       {
    FitFunction = new TF1("f","[0]*TMath::Gaus(x,[1],[2])",DistMinBin,DistMaxBin);
    FitFunction->SetParameters(1e4,Distr->GetMean(),Distr->GetRMS());
    FitFunction->SetLineColor(kRed);
    FitFunction->SetLineStyle(2);
    Distr->Fit(FitFunction,"Q0");
  }
  void SetDistLegend(){
    DistLegend = new TLegend( 0.60521-0.4 , 0.715617-0.10, 0.933883-0.4, 0.932121-0.10 );
    DistLegend->AddEntry(Distr,"Threshold","F");
    DistLegend->AddEntry(Distr,Form("Mean: %.0f#pm%.0f e",Distr->GetMean(),Distr->GetMeanError()),"");
    DistLegend->AddEntry(Distr,Form("RMS:  %.0f#pm%.0f e",Distr->GetRMS() ,Distr->GetRMSError()) ,"");
    DistLegend->AddEntry(FitFunction,"Threshold Fit","L");
    DistLegend->AddEntry(FitFunction,Form("#mu: %.0f#pm%.0f e"   ,FitFunction->GetParameter(1), FitFunction->GetParError(1)),"");
    DistLegend->AddEntry(FitFunction,Form("#sigma: %.0f#pm%.0f e",FitFunction->GetParameter(2), FitFunction->GetParError(2)),"");
    DistLegend->SetBorderSize(0);
    DistLegend->SetFillStyle(0);
  }
};

class Noise : public Scan {
public:
  Noise(int run, std::string chipSN,  TString ScanPath) {
    ScanType     = "NOISE"; DistMinBin=0; DistMaxBin=400; DistBinSize=5; DistXaxisTitle="Noise [e^{-}]";
    RunNum       = run;
    ScanFilePath = ScanPath+Form("THRESHOLD_SCAN_%06d/histos.root",run);
    HistoName    = Form("Mod_%s_Sigma",chipSN.c_str());
    LoadFile();
    LoadHisto();
  }
  void FitDist()       {
    FitFunction = new TF1("f","[0]*TMath::Gaus(x,[1],[2])",DistMinBin,DistMaxBin);
    FitFunction->SetParameters(1e4,Distr->GetMean(),Distr->GetRMS());
    FitFunction->SetLineColor(kRed);
    FitFunction->SetLineStyle(2);
    Distr->Fit(FitFunction,"Q0");
  }
  void SetDistLegend(){
    DistLegend = new TLegend( 0.60521 , 0.715617-0.10, 0.933883, 0.932121-0.10 );
    DistLegend->AddEntry(Distr,"Noise","F");
    DistLegend->AddEntry(Distr,Form("Mean: %.0f#pm%.0f e",Distr->GetMean(),Distr->GetMeanError()),"");
    DistLegend->AddEntry(Distr,Form("RMS:  %.0f#pm%.0f e",Distr->GetRMS() ,Distr->GetRMSError()) ,"");
    DistLegend->AddEntry(FitFunction,"Noise Fit","L");
    DistLegend->AddEntry(FitFunction,Form("#mu: %.0f#pm%.0f e"   ,FitFunction->GetParameter(1), FitFunction->GetParError(1)),"");
    DistLegend->AddEntry(FitFunction,Form("#sigma: %.0f#pm%.0f e",FitFunction->GetParameter(2), FitFunction->GetParError(2)),"");
    DistLegend->SetBorderSize(0);
    DistLegend->SetFillStyle(0);
  }
};

class Tot : public Scan {
public:
  Tot(int run, std::string chipSN,  TString ScanPath) {
    ScanType     = "TOT_TEST"; DistMinBin=0; DistMaxBin=20; DistBinSize=0.2; DistXaxisTitle="ToT [25 ns]";
    RunNum       = run;
    ScanFilePath = ScanPath+Form("TOT_TEST_%06d/analysis.root",run);
    HistoName    = Form("ToT_Mean_Mod_%s",chipSN.c_str());
    LoadFile();
    LoadHisto();
  }
  void FitDist()       {
    FitFunction = new TF1("f","[0]*TMath::Gaus(x,[1],[2])",DistMinBin,DistMaxBin);
    FitFunction->SetParameters(1e4,10,1);
    FitFunction->SetLineColor(kRed);
    FitFunction->SetLineStyle(2);
    Distr->Fit(FitFunction,"Q0");
  }
  void SetDistLegend(){
    DistLegend = new TLegend( 0.20521 , 0.715617-0.10, 0.533883, 0.932121-0.10 );
    DistLegend->AddEntry(Distr,"ToT","F");
    DistLegend->AddEntry(Distr,Form("Mean: %.1f#pm%.1f e",Distr->GetMean(),Distr->GetMeanError()),"");
    DistLegend->AddEntry(Distr,Form("RMS:  %.1f#pm%.1f e",Distr->GetRMS() ,Distr->GetRMSError()) ,"");
    DistLegend->AddEntry(FitFunction,"ToT Fit","L");
    DistLegend->AddEntry(FitFunction,Form("#mu: %.1f#pm%.1f e"   ,FitFunction->GetParameter(1), FitFunction->GetParError(1)),"");
    DistLegend->AddEntry(FitFunction,Form("#sigma: %.1f#pm%.1f e",FitFunction->GetParameter(2), FitFunction->GetParError(2)),"");
    DistLegend->SetBorderSize(0);
    DistLegend->SetFillStyle(0);
  }
};

class Source : public Scan {
 public: 
  Source(int run, std::string chipSN, TString ScanPath){
    ScanType  = "SOURCESCAN"; DistMinBin=0; DistMaxBin=20; DistBinSize=1.0; DistXaxisTitle="ToT [25 ns]";
    RunNum = run;
    ChipSN=chipSN;
    ScanFilePath = ScanPath+Form("SELFTRIGGER_%06d/histos.root",run);
    HistoName    = Form("Mod_%s_Selftrigger_Occupancy",chipSN.c_str());
    LoadFile();
    LoadHisto();
  }
  void LoadHisto(){
    if(File->IsZombie()){
      std::cout << "File does not exist: creating dummy histos" << std::endl ;
      Map   = new TH2D("NoFileMap","",1,0,1,1,0,1);
      Distr = new TH1D("NoFileDist","",1,0,1);
      return;
    }
    std::cout << "Histo :" << std::endl << HistoName   << std::endl;
    Map   = (TH2D*) File->Get(HistoName);
    Map->GetXaxis()->SetTitle("Columns");
    Map->GetYaxis()->SetTitle("Rows");
    Distr = (TH1D*) File->Get(Form("Mod_%s_Selftrigger_ToT",ChipSN.c_str()));
    Distr->SetLineColor(kBlue);
    Distr->SetFillColor(kBlue);
    Distr->SetFillStyle(3004 );
    Distr->GetXaxis()->SetTitle(DistXaxisTitle);
    Distr->GetYaxis()->SetTitle("Hits");
    FitDist();
    SetDistLegend();

  }
  void FitDist(){
    /*
      double fr[2];
      double sv[4], pllo[4], plhi[4], fp[4], fpe[4];
      fr[0]=0.3*Distr->GetMean();
      fr[1]=3.0*Distr->GetMean();
  
      pllo[0]=0.01; pllo[1]=2.0;              pllo[2]=Distr->Integral()*0.3; pllo[3]=0.4;
      plhi[0]=5.0;  plhi[1]=20.0;             plhi[2]=Distr->Integral()*3.0; plhi[3]=5.0;
      sv  [0]=1.0;  sv  [1]=Distr->GetMean(); sv[2]  =Distr->Integral();     sv  [3]=3.0;
      
      double chisqr;
      int    ndf;
      double FWHM;
      FitFunction = langaufit((TH1F*)Distr,fr,sv,pllo,plhi,fp,fpe,&chisqr,&ndf);
      langaupro(fp,ToTMPV,FWHM);
      //    sigma=fpe[1];
      */
    FitFunction = new TF1("f","[0]*TMath::Landau(x,[1],[2])",DistMinBin,DistMaxBin);
    FitFunction->SetParameters(Distr->Integral(),Distr->GetMean(),1);
    FitFunction->SetLineColor(kRed);
    FitFunction->SetLineStyle(2);
    Distr->Fit(FitFunction,"Q0");
    ToTMPV     =FitFunction->GetParameter(1);
    ToTMPVError=FitFunction->GetParError (1);
  }

  void SetDistLegend(){
    DistLegend = new TLegend( 0.60521 , 0.715617-0.10, 0.933883, 0.932121-0.10 );
    DistLegend->AddEntry(Distr,"Hit ToT","F");
    DistLegend->AddEntry(Distr,Form("Mean: %.1f#pm%.1f ToT",Distr->GetMean(),Distr->GetMeanError()),"");
    DistLegend->AddEntry(Distr,Form("RMS:  %.1f#pm%.1f ToT",Distr->GetRMS() ,Distr->GetRMSError()) ,"");
    DistLegend->AddEntry(FitFunction,"ToT Fit","L");
    DistLegend->AddEntry(FitFunction,Form("MPV: %.1f#pm%.1f ToT"   ,ToTMPV, ToTMPVError),"");
    DistLegend->AddEntry(FitFunction,/*Form("#sigma: %.0f#pm%.0f ToT",FitFunction->GetParameter(2), FitFunction->GetParError(2))*/" ","");
    DistLegend->SetBorderSize(0);
    DistLegend->SetFillStyle(0);
  }

};

#endif
