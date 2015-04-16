function createfigure(ymatrix1, t, y, ymax)


figure1 = figure('Name', t);

axes1 = axes('Parent',figure1,'XTickLabel',{'10','20','30'},'XTick',[1 2 3],'FontSize',20);
box(axes1,'on');
grid(axes1,'on');
hold(axes1,'all');

% Create multiple lines using matrix input to bar
bar1 = bar(ymatrix1,'Parent',axes1);
set(bar1(1),'DisplayName','PF');
set(bar1(2),'DisplayName','FLS');
set(bar1(3),'DisplayName','EXP');
set(bar1(4),'DisplayName','LOG');

xlabel('Number of Users','FontSize',20);
ylabel(y,'FontSize',20);
ylim ([0 ymax]);

legend1 = legend(axes1,'show');
set(legend1,...
    'Position',[0.794558608718962 0.753098188751188 0.0997122246143713 0.157112868804529]);

saveas(figure1, t, 'png')
