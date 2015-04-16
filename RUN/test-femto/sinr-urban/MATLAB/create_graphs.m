close all,
clear all;
clc;

R = 250;

PASSO_X = 20;
PASSO_Y = 20;
X = [-R:PASSO_X:R];
Y = [-R:PASSO_Y:R];
XX = [-20:1:50]; %%per la cdf

only_macro = load ('../OUTPUT/only_macro');
macro_with_streets = load('../OUTPUT/macro_with_streets');
macro_with_femto = load('../OUTPUT/macro_with_femto');	
macro_with_femto_reuse = load('../OUTPUT/macro_with_femto_reuse');	

figure('Name','Analisi SINR','NumberTitle','on');
SINR = make_sinr(only_macro, PASSO_X, PASSO_Y, R);
cdf_1 = make_cdf(only_macro, XX);
subplot(2,2,1)
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title ('onlymacro','FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

SINR = make_sinr(macro_with_streets, PASSO_X, PASSO_Y, R);
cdf_2 = make_cdf(macro_with_streets, XX);
subplot(2,2,2)
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title ('macro_with_streets','FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

SINR = make_sinr(macro_with_femto, PASSO_X, PASSO_Y, R);
cdf_3 = make_cdf(macro_with_femto, XX);
subplot(2,2,3)
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title ('macro_with_femto','FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

SINR = make_sinr(macro_with_femto_reuse, PASSO_X, PASSO_Y, R);
cdf_4 = make_cdf(macro_with_femto_reuse, XX);
subplot(2,2,4)
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title ('macro_with_femto_reuse','FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');



figure('Name','CDF of SINR','NumberTitle','on');
plot(XX,cdf_1,'b','LineWidth',1.7)
hold on 
grid
plot(XX,cdf_2,'r','LineWidth',1.7)
plot(XX,cdf_3,'k','LineWidth',1.7)
plot(XX,cdf_4,'g','LineWidth',1.7)
title ('CDF of ','FontSize',26,'FontName','Times New Roman');
xlabel('SINR [dB]')
ylabel('Probability');
xlim([-20 50]);
legend('only macro', 'macro_with_streets', 'macro_with_femto', 'macro_with_femto_reuse','Location', 'SouthEast');


