function gen_wbtest_csvwrite_1(verbose)
  load reference/wbinputs.mat
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    csvwrite('test_csvwrite.csv',[1,2,3;5,6,7]);y1=csvread('test_csvwrite.csv');
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_csvwrite_1_ref.mat error_refs  y1_refs 
end