% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_cellstr
  bbtest_success = 1;
NumErrors = 0;
try
  a = ['quick';'brown';'fox  ';'is   ']

catch
  NumErrors = NumErrors + 1;
end
try
  cellstr(a)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end