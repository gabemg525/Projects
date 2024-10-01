%x_single_period(t)= 5, 0<t<1 , T=2
% -5, 1<t<2
%x_periodic(t), 0<t<5T
t=0:.01:5;
z=@(t) t.^3-j.*2.*pi.*t.^2;
x_single_period=z(t);
%% period and fundamental angular freq.
dT=t(2)-t(1);
T= dT*length(t);
w0=2*pi/T;

k=-50:50; 
for ii=1:length(k)
    Ck(ii)=(1/T)*trapz(t, x_single_period.*exp(-j*k(ii)*w0*t))
end
w0_k=w0*k; 
figure (2)
subplot(2,1,1)
stem(k,abs(Ck))
hold on
legend('Magnitude Spectrum k = -50:50'), title('|Ck| vs \omega k')
xlabel('\omega')
subplot(2,1,2)
stem(w0_k,angle(Ck)*180/pi);
legend('Phase Spectrum k = -50:50'),title('\angle Ck vs \omega k')
xlabel('\omega')
ylabel('Degrees')
%% replicating single period
number_of_periods=2;
x_extended=repmat(x_single_period,1,number_of_periods); 
t_extended=linspace(t(1),t(1)+(length(x_extended)-1)*(t(2)-t(1)),length(x_extended));
%% signal reconstruction from FS components
x_reconstructed = zeros(1,length(t_extended));
for ii=1:length(k)
    x_reconstructed=x_reconstructed+Ck(ii)*exp(j*k(ii)*w0*t_extended);
end
figure (1)
subplot(3,1,1), plot(t_extended,real(x_reconstructed),t_extended,real(x_extended));
xlabel('t'), title('Real Decomposition'), legend('Real: Reconstructed Signal','Real: Original Signal');
subplot(3,1,2), plot(t_extended,imag(x_reconstructed), t_extended, imag(x_extended));
xlabel('t'), title('Imaginary Decomposition'), legend('Imaginary: Reconstructed Signal','Imaginary: Original Signal');