function gen_wbtest_csvread_3(verbose)
  myloc=fileparts(which('wbtest_csvread_3'));
  load([myloc,dirsep,'reference',dirsep,'wbinputs.mat'])
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    fn=[fileparts(which('wbtest_csvread_3')),dirsep,'sample_data.csv'];y1=csvread(fn,1,2,[1,2,3,4]);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_csvread_3_ref.mat error_refs  y1_refs 
end