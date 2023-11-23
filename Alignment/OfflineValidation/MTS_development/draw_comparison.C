#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include "TLegend.h"
#include "Riostream.h"

void draw_comparison(string file_1_path = "", string file_2_path = "", string plot_name = "") {
    // Open the first file
    TFile *file1 = new TFile(file_1_path.c_str());

    // Open the second file
    TFile *file2 = new TFile(file_2_path.c_str());

    // Get the tree named splitterTree under the folder cosmicValidation
    TTree *tree1 = (TTree*) file1->Get("cosmicValidation/splitterTree");
    TTree *tree2 = (TTree*) file2->Get("cosmicValidation/splitterTree");

    // Create a canvas to draw the histograms
    TCanvas *canvas = new TCanvas("canvas", "Canvas", 800, 600);

    // Set the colors for the histograms
    const int color1 = kRed;
    const int color2 = kBlue;

    // Create a histogram for the first file
    TH1F *hist1 = new TH1F("hist1", "Delta_qoverpt for file 1", 50, -0.1, 0.1);
    hist1->SetLineColor(color1);

    // Fill the histogram with the values from the first file
    tree1->Draw("Delta_qoverpt >> hist1");

    // Create a histogram for the second file
    TH1F *hist2 = new TH1F("hist2", "Delta_qoverpt for file 2", 50, -0.1, 0.1);
    hist2->SetLineColor(color2);

    // Fill the histogram with the values from the second file
    tree2->Draw("Delta_qoverpt >> hist2", "", "SAME");

    // Set the title and axis labels for the histograms
    hist1->SetTitle("Comparison of Delta_qoverpt for two files");
    hist1->GetXaxis()->SetTitle("Delta_qoverpt");
    hist1->GetYaxis()->SetTitle("Counts");

    // Draw the histograms on the canvas
    hist1->Draw();
    hist2->Draw("SAME");

    // Add a legend to the canvas
    TLegend *legend = new TLegend(0.6, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, "File 1", "l");
    legend->AddEntry(hist2, "File 2", "l");
    legend->Draw();

    // Save the canvas as a PDF file
    canvas->SaveAs(plot_name.c_str());

    // Close the files
    file1->Close();
    file2->Close();
}
