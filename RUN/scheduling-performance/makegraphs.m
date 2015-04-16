clc;
close all;
clear all;

sched=[1 6 5 4]; % PF; LOG; EXP; FLS
users=[10 20 30 40];
delay=[0.04 0.06 0.08 0.1];
speed=[3 120];


test='PLRVIDEO';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='PLR [%]';
    ymax=100;
    ymin=0;
    plotnormal(mat',t, y, ymin, ymax)
end
end

test='TPUTVIDEO';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='Goodput [Mbps]';
    ymax=5;
    mat=mat./1000000;
    ymin=0;
    plotnormal(mat',t, y, ymin, ymax)
end
end


test='DELVIDEO';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='Goodput [Mbps]';
    y='Packets Delay [s]';
    ymax=100;
    ymin=0.01;
    plotlog(mat',t, y, ymin, ymax)
end
end







test='PLRVOIP';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='PLR [%]';
    ymax=20;
    ymin=0;
    plotnormal(mat',t, y, ymin, ymax)
end
end

test='TPUTVOIP';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='Goodput [Mbps]';
    ymax=1;
    mat=mat./1000000;
    ymin=0;
    plotnormal(mat',t, y, ymin, ymax)
end
end


test='DELVOIP';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='Goodput [Mbps]';
    y='Packets Delay [s]';
    ymax=1;
    ymin=0.001;
    plotlog(mat',t, y, ymin, ymax)
end
end











test='TPUTBE';
for v=1:length(speed)
for ue=1:length(users)
    for d=1:length(delay)
    for s=1:length(sched)
        dat=load(strcat('OUT/',test,'_SCHED_',num2str(sched(s)),'_UE_',num2str(users(ue)), '_V_',num2str(speed(v)),'_D_',num2str(delay(d))));
        mat(s,d)=mean(dat);
    end
    end
    t=strcat(test,'_SPEED_',num2str(speed(v)),'_UEs_',num2str(users(ue)));
    y='Goodput [Mbps]';
    ymax=10;
    mat=mat./1000000;
    ymin=0;
    plotnormal(mat',t, y, ymin, ymax)
end
end