function test_val = wbtest_strncmp_2(verbose)
  load reference/wbinputs.mat
  load reference/wbtest_strncmp_2_ref.mat
  fail_count = 0;
  for loopi=1:numel(wbinputs)
    for loopj=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      x2 = wbinputs{loopj};
      error_flag = 0;
     y1 = [];
      try
        y1=strncmp(x1,x2,3);
      catch
        error_flag = 1;
      end
    if (error_flag && ~error_refs(loopi,loopj))
       printf('Mismatch Errors: input %d, %d y1=strncmp(x1,x2,3)\n',loopi,loopj);
        fail_count = fail_count + 1;
  elseif (~error_flag && ~error_refs && ~wbtest_exact(y1,y1_refs{loopi,loopj}))
    printf('Mismatch (exact): input %d,%d output 1 y1=strncmp(x1,x2,3)\n',loopi,loopj);
    fail_count = fail_count + 1;
  end
    end
  end
  test_val = (fail_count == 0);
end