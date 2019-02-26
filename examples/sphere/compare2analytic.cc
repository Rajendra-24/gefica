/**
 * \file compare2analytic.cc
 * \example sphere/compare2analytic.cc
 * Compare numerical result to analytic calculation for 1D spherical detector
 */
using namespace GeFiCa;
void compare2analytic()
{
   // configure detector
   Sphere1D *num=new Sphere1D;
   num->InnerRadius=0.3*cm;
   num->OuterRadius=1*cm;
   num->SetAverageImpurity(3e9/cm3);
   num->V0=900*volt;
   num->V1=0*volt;
   num->Dump();
   cout<<"press any key to continue"<<endl; cin.get();

   // make a copy of the detector configuration
   Sphere1D *ana = (Sphere1D*) num->Clone("ana");

   // calculate potential using SOR method
   num->CalculatePotential(kSOR2);

   // fill grid with analytic result
   ana->CalculatePotential(kAnalytic);

   // prepare drawing style
   gROOT->SetStyle("Plain"); // pick up a good drawing style to modify
   gStyle->SetLegendBorderSize(0);
   gStyle->SetLegendFont(132);
   gStyle->SetLabelFont(132,"XY");
   gStyle->SetTitleFont(132,"XY");
   gStyle->SetLabelSize(0.05,"XY");
   gStyle->SetTitleSize(0.05,"XY");
   gStyle->SetPadRightMargin(0.01);
   gStyle->SetPadLeftMargin(0.12);
   gStyle->SetPadTopMargin(0.02);

   // generate graphics
   TTree *tn = num->GetTree();
   tn->Draw("v:c1");
   TGraph *gn = new TGraph(tn->GetSelectedRows(), tn->GetV2(), tn->GetV1());

   TTree *ta = ana->GetTree();
   ta->Draw("v:c1");
   TGraph *ga = new TGraph(ta->GetSelectedRows(), ta->GetV2(), ta->GetV1());

   // compare numerical result to analytic calculation
   gn->SetMarkerColor(kBlue);
   gn->SetMarkerStyle(kCircle);
   gn->SetMarkerSize(0.8);
   gn->SetTitle(";Radius [cm];Potential [V]");
   gn->GetXaxis()->SetRangeUser(0,3);
   gn->GetYaxis()->SetRangeUser(0,900);
   gn->Draw("ap");

   ga->SetLineColor(kRed);
   ga->Draw("l");

   TLegend *l = new TLegend(0.6,0.6,0.8,0.8);
   l->AddEntry(ga,"Analytic","l");
   l->AddEntry(gn,"SOR2","p");
   l->Draw();

   gPad->Print("s1d.png");
}
