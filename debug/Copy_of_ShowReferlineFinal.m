i =3;
j = i;

Bfilename = ['../input/boundary_',num2str(i),'.txt'];
    [type,name,x,y]=textread(Bfilename,'%s%s%n%n');%read data
    %type(length(type))
    if strcmp(type(length(type)),'E2')
        x=[x(1:(length(name)-2));x(1,1)];
        y=[y(1:(length(name)-2));y(1,1)];
    else
        x=[x;x(1)];
        y=[y;y(1)];
        length(x);
        length(y);
    end
figure(1)
plot(x,y,'b')
hold on

Rfilename = ['../output2/referline_',num2str(j),'.txt'];
[xR,yR] = textread(Rfilename,'%n%n');
figure(1)
[a,b] = size(xR);
for i=1:(a-1)
    x = [xR(i);xR(i+1)];
    y = [yR(i);yR(i+1)];
    if ((xR(i) == -2 && yR(i)==-2) ||(xR(i+1)== -2 && yR(i+1)==-2)||(xR(i) == -3 && yR(i)==-3) ||(xR(i+1)== -3 && yR(i+1)==-3))
        i
        continue;
    end 
    plot(x,y,'r');
%     name = [' \leftarrow ',num2str(i)];
%     text(xR(i),yR(i),name);
    name = [' \leftarrow ',num2str(i)];
    text((xR(i+1)+xR(i))/2,(yR(i+1)+yR(i))/2,name);
    hold on;
end
% plot(xR,yR,'b');

% Rfilename = ['../input/referline_',num2str(j),'.txt'];
% [xR,yR] = textread(Rfilename,'%n%n');
% figure(1);
% plot(xR,yR,'g');