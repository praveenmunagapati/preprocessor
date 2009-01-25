function test_val = wbtest_linspace_1(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_linspace_1_ref.mat
  fail_count = 0;
  error_flag = 0;
  y1 = [];
  try
    z=linspace(0,60000,60001);y1=z(end);
  catch
    error_flag = 1;
  end
  if (error_flag && ~error_refs)
     printf('Mismatch Errors: z=linspace(0,60000,60001);y1=z(end)\n');
     fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{1}))
    printf('Mismatch (exact): output 1 z=linspace(0,60000,60001);y1=z(end)\n');
    fail_count = fail_count + 1;
  end
  test_val = (fail_count == 0);
end