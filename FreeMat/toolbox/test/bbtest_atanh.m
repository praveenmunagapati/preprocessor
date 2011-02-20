% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_atanh
  bbtest_success = 1;
NumErrors = 0;
try
  x = -0.99:.01:0.99;

catch
  NumErrors = NumErrors + 1;
end
try
  plot(x,atanh(x)); grid('on');

catch
  NumErrors = NumErrors + 1;
end
try
  mprint('atanhplot');

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end