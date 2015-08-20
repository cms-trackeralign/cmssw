void run()
{
    gROOT->ProcessLine(".L Cosmicv1.C");
    gROOT->ProcessLine("Cosmicv1 t");
    gROOT->ProcessLine("t.Loop()");
}
