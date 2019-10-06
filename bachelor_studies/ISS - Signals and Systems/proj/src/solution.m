iptsetpref('UseIPPL',false);
%Nacitanie obrazku
obr = imread('xsulak04.bmp');

%Zaostrenie obrazu
mat1 = [ -0.5 -0.5 -0.5 ; -0.5 5.0 -0.5; -0.5 -0.5 -0.5];
obr1 = imfilter(obr, mat1);
imwrite(obr1,'../step1.bmp');

%Otocenie obrazu
obr2 = fliplr(obr1);
imwrite(obr2,'../step2.bmp');

%Medianovy filter
median = [5 5];
obr3 = medfilt2(obr2, median);
imwrite(obr3,'../step3.bmp');

%Rozmazanie obrazu
mat2 = [1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1]/49;
obr4 = imfilter(obr3, mat2);
imwrite(obr4,'../step4.bmp');

%Chyba v obraze
obr4f = fliplr(obr4);
noise=0;
chyba  = double(obr);
chyba2 = double(obr4f);
for (x=1:512)
    for (y=1:512)
        noise = noise + abs(chyba(x,y) - chyba2(x,y));
    end;
end;
noise=noise/512/512

%Roztiahnutie histogramu
%imhist(obr)
minimum = min(im2double(obr4(:)));
maximum = max(im2double(obr4(:)));
rozsah = [minimum maximum];
obr5tmp = imadjust(obr4, rozsah , [0 1]);
obr5 = uint8(obr5tmp);
imwrite(obr5,'../step5.bmp');
%imhist(obr5)

%Stredna hodnota , smerodatna odchylka
mean_no_hist = mean2(im2double(obr4))*255
std_no_hist = std2(im2double(obr4))*255
mean_hist = mean2(im2double(obr5))*255
str_hist =  std2(im2double(obr5))*255

%Kvantizacia obrazu
N = 2;
a = 0;
b = 255;
obr6 = double(obr5);
obr6tmp = zeros(512 , 512);
for i = 1:512
    for j = 1:512
        obr6tmp(i,j) = round(((2^N)-1)*(obr6(i,j)-a)/(b-a))*(b-a)/((2^N)-1) + a;
    end;
end;
obr6 = uint8(obr6tmp);
imwrite(obr6,'../step6.bmp');