% SIZEFIG SIZEFIG Set Size of an Fig Window
% 
% Usage
% 
% The sizefig function changes the size of the currently
% selected fig window.  The general syntax for its use is
% 
%    sizefig(width,height)
% 
% where width and height are the dimensions of the fig
% window.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function sizefig(width,height)
  set(gcf,'figsize',[width,height])