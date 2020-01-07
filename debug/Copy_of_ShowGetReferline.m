%i = input('enter the Result');
i =3;
% %show Referline
% Rfilename = ['../input/referline_',num2str(i),'.txt'];
% [xR,yR] = textread(Rfilename,'%n%n');
% figure(1)
% plot(xR,yR,'r');
% hold on;
Bfilename = ['../input/boundary_',num2str(i),'.txt'];
    [type,name,x,y]=textread(Bfilename,'%s%s%n%n');%read data
    %type(length(type))
    if strcmp(type(length(type)),'E2')
        x=[x(1:(length(name)-2));x(1,1)];
        y=[y(1:(length(name)-2));y(1,1)];
    else
        x=[x;x(1)];
        y=[y;y(1)];
        length(x)
        length(y)
    end
figure(1)
plot(x,y,'b')
hold on

filename = ['../output1/centerline_',num2str(i),'.txt'];
[x1,y1,x2,y2] = textread(filename,'%n%n%n%n');
[a,b] = size(x1);
for i=1:a
    x = [x1(i);x2(i)];
    y = [y1(i);y2(i)];
    plot(x,y,'r');
    name = [' \leftarrow ',num2str(i)];
    text((x1(i)+x2(i))/2,(y1(i)+y2(i))/2,name);
    hold on;
end
% figure(2)
% figure(2)
% for i=1:a
%     x = [x1(i);x2(i)];
%     y = [y1(i);y2(i)];
%     plot(x,y,'r');
%     name = [' \leftarrow ',num2str(i)];
%     text((x1(i)+x2(i))/2,(y1(i)+y2(i))/2,name);
%     hold on;
% end

%show Result
