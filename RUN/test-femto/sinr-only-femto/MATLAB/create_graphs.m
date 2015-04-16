close all,
clear all;
clc;

R = 25;

PASSO_X = 1;
PASSO_Y = 1;
X = [-R:PASSO_X:R];
Y = [-R:PASSO_Y:R];
XX = [-20:1:50]; %%per la cdf


SCENARIO='femto-henb-al-centro';
TEST='SINR';
figure('Name','Analisi SINR','NumberTitle','on');

RIUSO=1; 
AF=1;
dati=load(strcat('../',SCENARIO,'/',TEST,'_riuso_',num2str(RIUSO),'_af_',num2str(AF)));
SINR = make_sinr(dati, PASSO_X, PASSO_Y, R);
cdf_1 = make_cdf(dati, XX);
subplot(2,2,1)
%axes1 = axes('FontSize',20,'FontName','Times');
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title (strcat('RIUSO = ', num2str(RIUSO)),'FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');
%colorbar('peer',axes1,'ZTickLabel','','ZTick',[0 0.5 1],'FontSize',20,...
%    'FontName','Times New Roman');

RIUSO=2; 
AF=1;
dati=load(strcat('../',SCENARIO,'/',TEST,'_riuso_',num2str(RIUSO),'_af_',num2str(AF)));
SINR = make_sinr(dati, PASSO_X, PASSO_Y, R);
cdf_2 = make_cdf(dati, XX);
subplot(2,2,2)
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title (strcat('RIUSO = ', num2str(RIUSO)),'FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

RIUSO=4; 
AF=1;
dati=load(strcat('../',SCENARIO,'/',TEST,'_riuso_',num2str(RIUSO),'_af_',num2str(AF)));
SINR = make_sinr(dati, PASSO_X, PASSO_Y, R);
cdf_4 = make_cdf(dati, XX);
subplot(2,2,3)
surfc(X,Y,SINR)
zlim([-10 40]); 
xlim([-R R]);ylim([-R R])
title (strcat('RIUSO = ', num2str(RIUSO)),'FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

subplot(2,2,4)
plot(XX,cdf_1,'b','LineWidth',1.7)
hold on 
grid
plot(XX,cdf_2,'r','LineWidth',1.7)
plot(XX,cdf_4,'g','LineWidth',1.7)
title (strcat('CDF of ', TEST),'FontSize',26,'FontName','Times New Roman');
xlabel(TEST);
ylabel('Probability');
xlim([-20 50]);
legend('riuso 1', 'riuso 2', 'riuso 4','Location', 'SouthEast');





TEST='TBS';
figure('Name','Analisi TBS','NumberTitle','on');
XX = [1:100:100000]; %%per la cdf

RIUSO=1; 
AF=1;
dati=load(strcat('../',SCENARIO,'/',TEST,'_riuso_',num2str(RIUSO),'_af_',num2str(AF)));
SINR = make_tbs(dati, PASSO_X, PASSO_Y, R);
cdf_1 = make_cdf(dati, XX);
subplot(2,2,1)
%axes1 = axes('FontSize',20,'FontName','Times');
surfc(X,Y,SINR)
zlim([0 100000]); 
xlim([-R R]);ylim([-R R])
title (strcat('RIUSO = ', num2str(RIUSO)),'FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');
%colorbar('peer',axes1,'ZTickLabel','','ZTick',[0 0.5 1],'FontSize',20,...
%    'FontName','Times New Roman');

RIUSO=2; 
AF=1;
dati=load(strcat('../',SCENARIO,'/',TEST,'_riuso_',num2str(RIUSO),'_af_',num2str(AF)));
SINR = make_tbs(dati, PASSO_X, PASSO_Y, R);
cdf_2 = make_cdf(dati, XX);
subplot(2,2,2)
surfc(X,Y,SINR)
zlim([0 100000]); 
xlim([-R R]);ylim([-R R])
title (strcat('RIUSO = ', num2str(RIUSO)),'FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

RIUSO=4; 
AF=1;
dati=load(strcat('../',SCENARIO,'/',TEST,'_riuso_',num2str(RIUSO),'_af_',num2str(AF)));
SINR = make_tbs(dati, PASSO_X, PASSO_Y, R);
cdf_4 = make_cdf(dati, XX);
subplot(2,2,3)
surfc(X,Y,SINR)
zlim([0 100000]); 
xlim([-R R]);ylim([-R R])
title (strcat('RIUSO = ', num2str(RIUSO)),'FontSize',26,'FontName','Times New Roman');
xlabel('x','FontSize',26,'FontName','Times New Roman');
ylabel('y','FontSize',26,'FontName','Times New Roman');

subplot(2,2,4)
plot(XX,cdf_1,'b','LineWidth',1.7)
hold on 
grid
plot(XX,cdf_2,'r','LineWidth',1.7)
plot(XX,cdf_4,'g','LineWidth',1.7)
title (strcat('CDF of ', TEST),'FontSize',26,'FontName','Times New Roman');
xlabel(TEST);
ylabel('Probability');
xlim([1 100000]);
legend('riuso 1', 'riuso 2', 'riuso 4','Location', 'SouthEast');


