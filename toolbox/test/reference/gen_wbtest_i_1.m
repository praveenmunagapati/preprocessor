function gen_wbtest_i_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=i;
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_i_1_ref.mat error_refs  y1_refs 
end