% Regression test function (black box) for isspace
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_string_isspace
  bbtest_success = 1;
NumErrors = 0;
try
  isspace('  hello there world ')
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end