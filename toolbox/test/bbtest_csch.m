% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_csch
  bbtest_success = 1;
NumErrors = 0;
try
  x1 = -pi+.01:.01:-.01;

catch
  NumErrors = NumErrors + 1;
end
try
  x2 = .01:.01:pi-.01;

catch
  NumErrors = NumErrors + 1;
end
try
  plot(x1,csch(x1),x2,csch(x2)); grid('on');

catch
  NumErrors = NumErrors + 1;
end
try
  mprint('cschplot');

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end