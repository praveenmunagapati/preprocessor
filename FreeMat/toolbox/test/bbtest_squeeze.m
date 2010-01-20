% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_squeeze
  bbtest_success = 1;
NumErrors = 0;
try
  x = zeros(1,4,3,1,1,2);

catch
  NumErrors = NumErrors + 1;
end
try
  size(x)

catch
  NumErrors = NumErrors + 1;
end
try
  y = squeeze(x);

catch
  NumErrors = NumErrors + 1;
end
try
  size(y)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end