% Regression test function (black box) for asec
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_mathfunctions_asec
  bbtest_success = 1;
NumErrors = 0;
try
  x1 = -5:.01:-1;
catch
  NumErrors = NumErrors + 1;
end
try
  x2 = 1:.01:5;
catch
  NumErrors = NumErrors + 1;
end
try
  plot(x1,asec(x1),x2,asec(x2)); grid('on');
catch
  NumErrors = NumErrors + 1;
end
try
  mprint('asecplot');
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end