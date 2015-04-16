
function [cdf] = make_cdf(file, XX);



hist = histc(file(:,3),XX);
cdf = cumsum (hist)./ length(file(:,3));