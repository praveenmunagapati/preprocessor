% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_spones
  bbtest_success = 1;
NumErrors = 0;
try
  a = [1,0,3,0,5;0,0,2,3,0;1,0,0,0,1]

catch
  NumErrors = NumErrors + 1;
end
try
  b = spones(a)

catch
  NumErrors = NumErrors + 1;
end
try
  full(b)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end