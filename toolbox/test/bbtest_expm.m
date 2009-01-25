% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_expm
  bbtest_success = 1;
NumErrors = 0;
try
  A = [1 1 0; 0 0 2; 0 0 -1]

catch
  NumErrors = NumErrors + 1;
end
try
  expm(A)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end