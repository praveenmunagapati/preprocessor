% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_vec
  bbtest_success = 1;
NumErrors = 0;
try
  A = [1,2,4,3;2,3,4,5]

catch
  NumErrors = NumErrors + 1;
end
try
  vec(A)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end