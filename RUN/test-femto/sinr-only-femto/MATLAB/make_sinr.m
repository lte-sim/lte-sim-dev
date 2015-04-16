
function [SINR] = make_sinr(file, PASSO_X, PASSO_Y, R);



X = [-R:PASSO_X:R];
Y = [-R:PASSO_Y:R];


SINR = zeros(length(X),length(Y));
SINR_elem = zeros(length(X),length(Y));
%% create SINR map
for i=1:length(file(:,1))
    x = int32((R + file(i,1))/PASSO_X);
    y = int32((R + file(i,2))/PASSO_Y);
    if (x <= 0) x=1; end
    if (y <= 0) y=1; end
    if (x > length(X)) x=length(X); end
    if (y > length(X)) y=length(Y); end
    SINR(x,y) = SINR(x,y) + 10^(file(i,3)/10); %accumulo i cinr in lineare
    SINR_elem(x,y) = SINR_elem(x,y) + 1;  
end



%% Compute average and Clear position with no information
for i=1:length(SINR(:,1))
    for j=1:length(SINR(:,2))
			if (SINR(i,j) ~=  0) 
				SINR(i,j) = 10*log10(SINR(i,j) / SINR_elem(i,j));
			else
				SINR(i,j) = NaN;
            end
    end
end