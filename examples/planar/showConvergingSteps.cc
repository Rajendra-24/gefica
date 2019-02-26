using namespace GeFiCa;
/**
 * \file showConvergingSteps.cc
 * \example planar/showConvergingSteps.cc
 * \brief Show middle steps of the converging process.
 */
{
   // calculate fields
   GeFiCa::Planar1D *detector = new GeFiCa::Planar1D(101);
   detector->MaxIterations=1;
   detector->Csor=1.9;
   detector->UpperBound=1.175*GeFiCa::cm;
   detector->V1=300*GeFiCa::volt;
   detector->V0=0*GeFiCa::volt;
   detector->SetAverageImpurity(1e10/GeFiCa::cm3);

   int n=10;
      TCanvas *C = new TCanvas();
      TMultiGraph *mg = new TMultiGraph();
   for (int i=0-1;i<n;i++) {
      detector->MaxIterations=i*20;
      detector->CalculatePotential(GeFiCa::kSOR2);
      detector->SaveField("planar1dSOR2.root");
      TChain *tn=new TChain("t");
      tn->Add("planar1dSOR2.root");
      tn->Draw("v:c1");
      TGraph *tg=new TGraph(tn->GetSelectedRows(), tn->GetV2(), tn->GetV1());
      tg->SetMarkerColor(i+2);
      tg->SetMarkerStyle(6);
      mg->Add(tg);
   }
   mg->SetTitle("; Thickness [cm]; Potential [V]");
   
   mg->Draw("ap");
}