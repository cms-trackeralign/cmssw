#define Cosmicv1_cxx
#include "Cosmicv1.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Cosmicv1::Loop()
{

   gROOT->ProcessLine(".x tdrstyle.C");    // Processing TDRStyle.C file

   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;

   int i = 0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
	
      i++;
  }
   
   int n = i;

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
//   cout << "size : "<< n <<endl;
//***************************************************************************************************************/
//					Various Rates Declerations						//
//**************************************************************************************************************/

   double * event_rate 		= new double[n];
   double * track_rate 		= new double[n]; 
   double * runNumber  		= new double[n];
   double * track_rate_PIX 	= new double[n]; 
   double * track_rate_FPIX	= new double[n]; 
   double * track_rate_BPIX 	= new double[n]; 
   double * track_rate_TOB 	= new double[n]; 
   double * track_rate_TIB 	= new double[n]; 
   double * track_rate_TID 	= new double[n]; 
   double * track_rate_TEC 	= new double[n];
   double * track_rate_TECP 	= new double[n];
   double * track_rate_TECM 	= new double[n];

/*   double * event_rate_PIX = new double[n];
   double * event_rate_FPIX = new double[n];
   double * event_rate_BPIX = new double[n];
   double * event_rate_TOB = new double[n];
   double * event_rate_TIB = new double[n];
   double * event_rate_TID = new double[n];
   double * event_rate_TEC = new double[n]; 
*/

/******************************************************************************************************************/

//***************************************************************************************************************/
//					Various Kinematical Histograms Declerations				//
//**************************************************************************************************************/

   TH1D *h_pt 		= new TH1D("h_pt","h_pt",200,0,7);
   TH1D *h_charge 	= new TH1D("h_charge","h_charge",10,-5,5);
   TH1D *h_chi2 	= new TH1D("h_chi2","h_chi2",200,0,100);
   TH1D *h_chi2_ndof 	= new TH1D("h_chi2_ndof","h_chi2_ndof",200,0,20);
   TH1D *h_eta 		= new TH1D("h_eta","h_eta",500,-3,3);
   TH1D *h_theta 	= new TH1D("h_theta","h_theta",500,-3,3);
   TH1D *h_phi 		= new TH1D("h_phi","h_phi",400,-3.5,3.5);
   TH1D *h_d0 		= new TH1D("h_d0","h_d0",1000,-85,85);
   TH1D *h_dz 		= new TH1D("h_dz","h_dz",1500,-350,350);

/******************************************************************************************************************/

//***************************************************************************************************************/
//					For Summary Track Rates Declerations					//
//**************************************************************************************************************/

   double FPIX_rate 	= 0;
   double BPIX_rate 	= 0;
   double TIB_rate 	= 0;
   double TID_rate 	= 0;
   double TEC_rate 	= 0;   
   double TOB_rate 	= 0;
   string Bar_Xtitle[6] = {"FPIX","BPIX","TIB","TID","TOB","TEC"};
   double Bar_Ytitle[6] = {0};
   
/******************************************************************************************************************/

//   FILE *file = NULL ;
//   file = fopen("Rates.txt","w");
//   fprintf(file,"\t\t|\t\t Various Track Rates\t\t|\n");
//   fprintf(file,"runnum\t| Total\t\t| FPIX\t\t| BPIX\t\t| \n");
//file<<"runnum\t"<<"| Total\t\t"<<"| FPIX\t\t"<<"| BPIX\t\t"<<endl;


   for (Long64_t jentry=0; jentry<nentries;jentry++) {		// Loop to calculate rates

      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      if (run_time->at(0) == 0) continue;

      for (int k = 0; k < pt->size() ; k++)			// Loop to calculate Kinematical distributions
      {
	h_pt->Fill(pt->at(k));
	h_charge->Fill(charge->at(k));
	h_chi2->Fill(chi2->at(k));
	h_chi2_ndof->Fill(chi2_ndof->at(k));
	h_eta->Fill(eta->at(k));
	h_theta->Fill(theta->at(k));
	h_phi->Fill(phi->at(k));
	h_d0->Fill(d0->at(k));
	h_dz->Fill(dz->at(k));

      }								// Loop Closed to calculate Kinematical distributions

      track_rate[ientry] = number_of_tracks->at(0)/run_time->at(0);
      event_rate[ientry] = number_of_events->at(0)/run_time->at(0);
      runNumber [ientry] = runnum->at(0);

      track_rate_PIX[ientry] = number_of_tracks_PIX->at(0)/run_time->at(0);
      track_rate_FPIX[ientry] = number_of_tracks_FPIX->at(0)/run_time->at(0);
      track_rate_BPIX[ientry] = number_of_tracks_BPIX->at(0)/run_time->at(0);
      track_rate_TOB[ientry] = number_of_tracks_TOB->at(0)/run_time->at(0);
      track_rate_TIB[ientry] = number_of_tracks_TIB->at(0)/run_time->at(0);
      track_rate_TID[ientry] = number_of_tracks_TID->at(0)/run_time->at(0);
      track_rate_TEC[ientry] = number_of_tracks_TEC->at(0)/run_time->at(0);
      track_rate_TECP[ientry] = number_of_tracks_TECP->at(0)/run_time->at(0);
      track_rate_TECM[ientry] = number_of_tracks_TECM->at(0)/run_time->at(0);

/*      event_rate_PIX[ientry] = number_of_events_PIX->at(0)/run_time->at(0);
      event_rate_BPIX[ientry] = number_of_events_BPIX->at(0)/run_time->at(0);
      event_rate_FPIX[ientry] = number_of_events_FPIX->at(0)/run_time->at(0);
      event_rate_TOB[ientry] = number_of_events_TOB->at(0)/run_time->at(0);
      event_rate_TIB[ientry] = number_of_events_TIB->at(0)/run_time->at(0);
      event_rate_TID[ientry] = number_of_events_TID->at(0)/run_time->at(0);
      event_rate_TEC[ientry] = number_of_events_TEC->at(0)/run_time->at(0);
*/     


      FPIX_rate += track_rate_FPIX[ientry];
      BPIX_rate += track_rate_BPIX[ientry];
      TIB_rate += track_rate_TIB[ientry];
      TID_rate += track_rate_TID[ientry];
      TEC_rate += track_rate_TEC[ientry];
      TOB_rate += track_rate_TOB[ientry];

//      fprintf(file,"%d\t| %.3lf\t\t| %.3lf\t\t| %.3lf\t\t| \n",runNumber[ientry],track_rate[ientry],track_rate_FPIX[ientry],track_rate_BPIX[ientry]);
      // if (Cut(ientry) < 0) continue;
     

     }								// Loop Closed to calculate rates





//	fclose(file);

/*******************************       Make Directories     ************************************/
	gSystem->Exec("mkdir -p Rate_Plots");
	gSystem->Exec("mkdir -p Kinematical_Plots");

/**********************************************************************************************/
	
	TCanvas *c  = new TCanvas("c","c",800,600);    // Declare canvas

/*******************************       pT Distribution       **********************************/     
	h_pt->SetLineColor(kBlue);
	h_pt->SetLineWidth(2);
	h_pt->SetTitle("pT distribution");
	h_pt->SetXTitle("pT (in GeV)");
	h_pt->Draw();
	c->SetGrid();
	c->SaveAs("pt.png");
	c->Clear();
	gSystem->Exec("mv pt.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       charge Distribution       **********************************/     

	h_charge->SetLineColor(kBlue);
	h_charge->SetLineWidth(2);
	h_charge->SetTitle("charge");
	h_charge->SetXTitle("");
	h_charge->Draw();
	c->SetGrid();
	c->SaveAs("charge.png");
	c->Clear();
	gSystem->Exec("mv charge.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       chi2 Distribution       **********************************/     

        h_chi2->SetLineColor(kBlue);
        h_chi2->SetLineWidth(2); 
        h_chi2->SetTitle("chi2 distribution");
        h_chi2->SetXTitle("");
        h_chi2->Draw();
        c->SetGrid();                         
        c->SaveAs("chi2.png");                                                                                                                  
        c->Clear();      
	gSystem->Exec("mv chi2.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       chi2 per ndof Distribution       **********************************/     

        h_chi2_ndof->SetLineColor(kBlue);
        h_chi2_ndof->SetLineWidth(2); 
        h_chi2_ndof->SetTitle("chi2 per ndof");
        h_chi2_ndof->SetXTitle("");
        h_chi2_ndof->Draw();
        c->SetGrid();    
        c->SaveAs("chi2_ndof.png");                                                                                                                       c->Clear();      
	c->Clear();
	gSystem->Exec("mv chi2_ndof.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       eta Distribution       **********************************/     

        h_eta->SetLineColor(kBlue);
        h_eta->SetLineWidth(2); 
        h_eta->SetTitle("eta Distribution");
        h_eta->SetXTitle("#eta");
        h_eta->Draw();
        c->SetGrid();                            
        c->SaveAs("eta.png");                                                                                                                  
        c->Clear();      
	gSystem->Exec("mv eta.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       theta Distribution       **********************************/     

        h_theta->SetLineColor(kBlue);
        h_theta->SetLineWidth(2); 
        h_theta->SetTitle("theta distribution");
        h_theta->SetXTitle("#theta");
        h_theta->Draw();
        c->SetGrid();    
        c->SaveAs("theta.png");                                                                                                                  
        c->Clear();      
	gSystem->Exec("mv theta.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       phi Distribution       **********************************/     

        h_phi->SetLineColor(kBlue);
        h_phi->SetLineWidth(2); 
        h_phi->SetTitle("phi distribution");
        h_phi->SetXTitle("#phi");
        h_phi->Draw();
        c->SetGrid();    
        c->SaveAs("phi.png");                                                                                                                  
        c->Clear();      
	gSystem->Exec("mv phi.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       d0 Distribution       **********************************/     

        h_d0->SetLineColor(kBlue);
        h_d0->SetLineWidth(2); 
        h_d0->SetTitle("d0 distribution");
        h_d0->SetXTitle("d0");
        h_d0->Draw();
        c->SetGrid();                         
        c->SaveAs("d0.png");                                                                                                                  
        c->Clear();      
	gSystem->Exec("mv d0.png Kinematical_Plots");
/**********************************************************************************************/


/*******************************       dz Distribution       **********************************/     

        h_dz->SetLineColor(kBlue);
        h_dz->SetLineWidth(2); 
        h_dz->SetTitle("dz distribution");
        h_dz->SetXTitle("dz");
        h_dz->Draw();
        c->SetGrid();                         
        c->SaveAs("dz.png");                                                                                                                  
        c->Clear();      
	gSystem->Exec("mv dz.png Kinematical_Plots");
/**********************************************************************************************/



			/*****************************/
			//	 Rate Plots	     //
			/****************************/



/*******************************  Overall track rate  ******************************************/ 
	TGraph * gr_track_rate = new TGraph(n,runNumber,track_rate);
	gr_track_rate->GetXaxis()->SetTitle("Run Number");
	gr_track_rate->GetXaxis()->SetNoExponent();
	gr_track_rate->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate->SetMarkerStyle(20);
	gr_track_rate->SetMarkerSize(1.2);
	gr_track_rate->SetMarkerColor(kBlue);
	gr_track_rate->SetTitle("Track Rate");
	gr_track_rate->Draw("AP");
	c->SetGrid();
        c->SaveAs("track_rate.png");
	c->Clear();
	gSystem->Exec("mv track_rate.png Rate_Plots");
/*********************************************************************************************/

/*******************************  Total Pixel track rate  ******************************************/ 
	TGraph * gr_track_rate_PIX = new TGraph(n,runNumber,track_rate_PIX);
	gr_track_rate_PIX->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_PIX->GetXaxis()->SetNoExponent();
	gr_track_rate_PIX->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_PIX->SetMarkerStyle(20);
	gr_track_rate_PIX->SetMarkerSize(1.2);
	gr_track_rate_PIX->SetMarkerColor(kBlue);
	gr_track_rate_PIX->SetTitle("Pixel Track Rate");
	gr_track_rate_PIX->Draw("AP");
	c->SetGrid();
        c->SaveAs("pixel_track_rate.png");
	c->Clear();
	gSystem->Exec("mv pixel_track_rate.png Rate_Plots");
/*********************************************************************************************/

/*******************************  FPIX track rate  ******************************************/ 
	TGraph * gr_track_rate_FPIX = new TGraph(n,runNumber,track_rate_FPIX);
	gr_track_rate_FPIX->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_FPIX->GetXaxis()->SetNoExponent();
	gr_track_rate_FPIX->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_FPIX->SetMarkerStyle(20);
	gr_track_rate_FPIX->SetMarkerSize(1.2);
	gr_track_rate_FPIX->SetMarkerColor(kBlue);
	gr_track_rate_FPIX->SetTitle("FPIX Track Rate");
	gr_track_rate_FPIX->Draw("AP");
	c->SetGrid();
        c->SaveAs("fpix_track_rate.png");
	c->Clear();
	gSystem->Exec("mv fpix_track_rate.png Rate_Plots");
/*********************************************************************************************/


/*******************************  BPIX track rate  ******************************************/ 
	TGraph * gr_track_rate_BPIX = new TGraph(n,runNumber,track_rate_BPIX);
	gr_track_rate_BPIX->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_BPIX->GetXaxis()->SetNoExponent();
	gr_track_rate_BPIX->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_BPIX->SetMarkerStyle(20);
	gr_track_rate_BPIX->SetMarkerSize(1.2);
	gr_track_rate_BPIX->SetMarkerColor(kBlue);
	gr_track_rate_BPIX->SetTitle("BPIX Track Rate");
	gr_track_rate_BPIX->Draw("AP");
	c->SetGrid();
        c->SaveAs("bpix_track_rate.png");
	c->Clear();
	gSystem->Exec("mv bpix_track_rate.png Rate_Plots");
/*********************************************************************************************/


/*******************************  TOB track rate  ******************************************/ 
	TGraph * gr_track_rate_TOB = new TGraph(n,runNumber,track_rate_TOB);
	gr_track_rate_TOB->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_TOB->GetXaxis()->SetNoExponent();
	gr_track_rate_TOB->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_TOB->SetMarkerStyle(20);
	gr_track_rate_TOB->SetMarkerSize(1.2);
	gr_track_rate_TOB->SetMarkerColor(kBlue);
	gr_track_rate_TOB->SetTitle("TOB Track Rate");
	gr_track_rate_TOB->Draw("AP");
	c->SetGrid();
        c->SaveAs("tob_track_rate.png");
	c->Clear();
	gSystem->Exec("mv tob_track_rate.png Rate_Plots");
/*********************************************************************************************/


/*******************************  TIB track rate  ******************************************/ 
	TGraph * gr_track_rate_TIB = new TGraph(n,runNumber,track_rate_TIB);
	gr_track_rate_TIB->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_TIB->GetXaxis()->SetNoExponent();
	gr_track_rate_TIB->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_TIB->SetMarkerStyle(20);
	gr_track_rate_TIB->SetMarkerSize(1.2);
	gr_track_rate_TIB->SetMarkerColor(kBlue);
	gr_track_rate_TIB->SetTitle("TIB Track Rate");
	gr_track_rate_TIB->Draw("AP");
	c->SetGrid();
        c->SaveAs("tib_track_rate.png");
	c->Clear();
	gSystem->Exec("mv tib_track_rate.png Rate_Plots");
/*********************************************************************************************/


/*******************************  TID track rate  ******************************************/ 
	TGraph * gr_track_rate_TID = new TGraph(n,runNumber,track_rate_TID);
	gr_track_rate_TID->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_TID->GetXaxis()->SetNoExponent();
	gr_track_rate_TID->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_TID->SetMarkerStyle(20);
	gr_track_rate_TID->SetMarkerSize(1.2);
	gr_track_rate_TID->SetMarkerColor(kBlue);
	gr_track_rate_TID->SetTitle("TID Track Rate");
	gr_track_rate_TID->Draw("AP");
	c->SetGrid();
        c->SaveAs("tid_track_rate.png");
	c->Clear();
	gSystem->Exec("mv tid_track_rate.png Rate_Plots");
/*********************************************************************************************/


/*******************************  Total TEC track rate  ******************************************/ 
	TGraph * gr_track_rate_TEC = new TGraph(n,runNumber,track_rate_TEC);
	gr_track_rate_TEC->GetXaxis()->SetTitle("Run Number");
	gr_track_rate_TEC->GetXaxis()->SetNoExponent();
	gr_track_rate_TEC->GetYaxis()->SetTitle("Track Rate (in Hz)");
	gr_track_rate_TEC->SetMarkerStyle(20);
	gr_track_rate_TEC->SetMarkerSize(1.2);
	gr_track_rate_TEC->SetMarkerColor(kBlue);
	gr_track_rate_TEC->SetTitle("TEC Track Rate");
	gr_track_rate_TEC->Draw("AP");
	c->SetGrid();
        c->SaveAs("tec_track_rate.png");
	c->Clear();
	gSystem->Exec("mv tec_track_rate.png Rate_Plots");
/*********************************************************************************************/


/*******************************  TEC+/- track rate  ******************************************/ 
	TMultiGraph *mg = new TMultiGraph("track rate","TRack Rate TEC+/-");		// Multigraph decleration
	
	TGraph * gr_track_rate_TECP = new TGraph(n,runNumber,track_rate_TECP);
	gr_track_rate_TECP->SetMarkerStyle(20);
	gr_track_rate_TECP->SetMarkerSize(1.2);
	gr_track_rate_TECP->SetMarkerColor(kBlack);

	TGraph * gr_track_rate_TECM = new TGraph(n,runNumber,track_rate_TECM);
	gr_track_rate_TECM->SetMarkerStyle(20);
	gr_track_rate_TECM->SetMarkerSize(1.2);
	gr_track_rate_TECM->SetMarkerColor(kRed);

	mg->Add(gr_track_rate_TECP);
	mg->Add(gr_track_rate_TECM);
	mg->Draw("AP");
	mg->GetXaxis()->SetTitle("Run Number");
	mg->GetXaxis()->SetNoExponent();
	mg->GetYaxis()->SetTitle("Track Rate (in Hz)");
	
	TLegend *leg = new TLegend(0.8,0.8,0.94,0.92);			// Legend for TEC+/-
	leg -> AddEntry(gr_track_rate_TECP, "TEC+","p");
	leg -> AddEntry(gr_track_rate_TECM, "TEC-","p");
	leg -> SetBorderSize(1);
	leg -> SetShadowColor(0);
	leg -> SetFillColor(0);
	leg -> Draw();
	c->SetGrid();
        c->SaveAs("tec_track_ratePM.png");
	c->Clear();
	gSystem->Exec("mv tec_track_ratePM.png Rate_Plots");
/*********************************************************************************************/


/*******************************  Overall event event rate  ******************************************/ 

	TGraph * gr_event_rate = new TGraph(n,runNumber,event_rate);
	gr_event_rate->GetXaxis()->SetTitle("Run Number");
	gr_event_rate->GetXaxis()->SetNoExponent();
	gr_event_rate->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate->SetMarkerStyle(20);
	gr_event_rate->SetMarkerSize(1.2);
	gr_event_rate->SetMarkerColor(kBlue);
	gr_event_rate->SetTitle("Event Rate");
	gr_event_rate->Draw("AP");
	c->SetGrid();
        c->SaveAs("event_rate.png");
	c->Clear();
	gSystem->Exec("mv event_rate.png Rate_Plots");
/*********************************************************************************************/

/*	TGraph * gr_event_rate_PIX = new TGraph(n,runNumber,event_rate_PIX);
	gr_event_rate_PIX->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_PIX->GetXaxis()->SetNoExponent();
	gr_event_rate_PIX->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_PIX->SetMarkerStyle(20);
	gr_event_rate_PIX->SetMarkerSize(1.2);
	gr_event_rate_PIX->SetMarkerColor(kBlue);
	gr_event_rate_PIX->SetTitle("PIXEL Event Rate");
	gr_event_rate_PIX->Draw("AP");
	c->SetGrid();

        c->SaveAs("pixel_event_rate.png");
	c->Clear();
	gSystem->Exec("mv pixel_event_rate.png Event_Rates");


	TGraph * gr_event_rate_FPIX = new TGraph(n,runNumber,event_rate_FPIX);
	gr_event_rate_FPIX->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_FPIX->GetXaxis()->SetNoExponent();
	gr_event_rate_FPIX->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_FPIX->SetMarkerStyle(20);
	gr_event_rate_FPIX->SetMarkerSize(1.2);
	gr_event_rate_FPIX->SetMarkerColor(kBlue);
	gr_event_rate_FPIX->SetTitle("FPIX Event Rate");
	gr_event_rate_FPIX->Draw("AP");
	c->SetGrid();

        c->SaveAs("fpix_event_rate.png");
	c->Clear();
	gSystem->Exec("mv fpix_event_rate.png Event_Rates");


	TGraph * gr_event_rate_BPIX = new TGraph(n,runNumber,event_rate_BPIX);
	gr_event_rate_BPIX->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_BPIX->GetXaxis()->SetNoExponent();
	gr_event_rate_BPIX->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_BPIX->SetMarkerStyle(20);
	gr_event_rate_BPIX->SetMarkerSize(1.2);
	gr_event_rate_BPIX->SetMarkerColor(kBlue);
	gr_event_rate_BPIX->SetTitle("BPIX Event Rate");
	gr_event_rate_BPIX->Draw("AP");
	c->SetGrid();

        c->SaveAs("bpix_event_rate.png");
	c->Clear();
	gSystem->Exec("mv bpix_event_rate.png Event_Rates");

	TGraph * gr_event_rate_TOB = new TGraph(n,runNumber,event_rate_TOB);
	gr_event_rate_TOB->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_TOB->GetXaxis()->SetNoExponent();
	gr_event_rate_TOB->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_TOB->SetMarkerStyle(20);
	gr_event_rate_TOB->SetMarkerSize(1.2);
	gr_event_rate_TOB->SetMarkerColor(kBlue);
	gr_event_rate_TOB->SetTitle("TOB Event Rate");
	gr_event_rate_TOB->Draw("AP");
	c->SetGrid();

        c->SaveAs("tob_event_rate.png");
	c->Clear();
	gSystem->Exec("mv tob_event_rate.png Event_Rates");

	TGraph * gr_event_rate_TIB = new TGraph(n,runNumber,event_rate_TIB);
	gr_event_rate_TIB->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_TIB->GetXaxis()->SetNoExponent();
	gr_event_rate_TIB->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_TIB->SetMarkerStyle(20);
	gr_event_rate_TIB->SetMarkerSize(1.2);
	gr_event_rate_TIB->SetMarkerColor(kBlue);
	gr_event_rate_TIB->SetTitle("TIB Event Rate");
	gr_event_rate_TIB->Draw("AP");
	c->SetGrid();

        c->SaveAs("tib_event_rate.png");
	c->Clear();
	gSystem->Exec("mv tib_event_rate.png Event_Rates");


	TGraph * gr_event_rate_TID = new TGraph(n,runNumber,event_rate_TID);
	gr_event_rate_TID->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_TID->GetXaxis()->SetNoExponent();
	gr_event_rate_TID->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_TID->SetMarkerStyle(20);
	gr_event_rate_TID->SetMarkerSize(1.2);
	gr_event_rate_TID->SetMarkerColor(kBlue);
	gr_event_rate_TID->SetTitle("TID Event Rate");
	gr_event_rate_TID->Draw("AP");
	c->SetGrid();

        c->SaveAs("tid_event_rate.png");
	c->Clear();
	gSystem->Exec("mv tid_event_rate.png Event_Rates");


	TGraph * gr_event_rate_TEC = new TGraph(n,runNumber,event_rate_TEC);
	gr_event_rate_TEC->GetXaxis()->SetTitle("Run Number");
	gr_event_rate_TEC->GetXaxis()->SetNoExponent();
	gr_event_rate_TEC->GetYaxis()->SetTitle("Event Rate (in Hz)");
	gr_event_rate_TEC->SetMarkerStyle(20);
	gr_event_rate_TEC->SetMarkerSize(1.2);
	gr_event_rate_TEC->SetMarkerColor(kBlue);
	gr_event_rate_TEC->SetTitle("TEC Event Rate");
	gr_event_rate_TEC->Draw("AP");
	c->SetGrid();

        c->SaveAs("tec_event_rate.png");
	c->Clear();
	gSystem->Exec("mv tec_event_rate.png Event_Rates");

*/

/****************************************************************************************************/
//			Summary Plot for track rate in each Subdetector				     //
/****************************************************************************************************/

   	TH1F *h1b = new TH1F("h1b","Subdetector track rate summary",6,0,6);
   	h1b->SetFillColor(4);
   	h1b->SetBarWidth(0.3);
   	h1b->SetBarOffset(0.35);
   	h1b->SetStats(0);

	Bar_Ytitle[0] = FPIX_rate/n;
	Bar_Ytitle[1] = BPIX_rate/n;
	Bar_Ytitle[2] = TIB_rate/n;
	Bar_Ytitle[3] = TID_rate/n;
	Bar_Ytitle[4] = TOB_rate/n;
	Bar_Ytitle[5] = TEC_rate/n;

      for (i=1; i<=6; i++)
        {
      	h1b->SetBinContent(i, Bar_Ytitle[i-1]);
      	h1b->GetXaxis()->SetBinLabel(i,Bar_Xtitle[i-1].c_str());
  	}

	gStyle->SetPaintTextFormat("1.4f");
	h1b->LabelsOption("v");
	h1b->SetLabelSize(0.04);
	h1b->GetYaxis()->SetTitle("Average Rate (Hz)");
	h1b->Draw("bTEXT");
	c->SaveAs("Summary_Chart.png");
        c->Close();


} 			// Cosmicv1::Loop() closed
