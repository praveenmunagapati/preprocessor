% Regression test function (black box) for for
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_flow_for
  bbtest_success = 1;
NumErrors = 0;
try
  accum = 0;
catch
  NumErrors = NumErrors + 1;
end
try
  for (i=1:100); accum = accum + i; end
catch
  NumErrors = NumErrors + 1;
end
try
  accum
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  accum = 0;
catch
  NumErrors = NumErrors + 1;
end
try
  for i=1:100; accum = accum + i; end
catch
  NumErrors = NumErrors + 1;
end
try
  accum
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
if (NumErrors ~= 0) bbtest_success = 0; return; end