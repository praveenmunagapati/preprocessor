% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_for
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
