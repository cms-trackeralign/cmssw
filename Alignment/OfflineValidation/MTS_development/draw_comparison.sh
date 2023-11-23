file_1_path="testMTSfull/MTS/single/PromptNewTemplate/testSingle/1/MTSValidation_PromptNewTemplate_1.root"
file_2_path="testMTSfull/MTS/single/mp3619/testSingle/1/MTSValidation_mp3619_1.root"
plot_name="testMTSfull/PromptNewTemplate_vs_mp3619.pdf"

root -l -b -q draw_comparison.C+\(\"${file_1_path}\",\"${file_2_path}\",\"${plot_name}\"\)
