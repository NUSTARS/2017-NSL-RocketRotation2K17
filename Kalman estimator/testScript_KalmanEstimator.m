%% Setup
clear all
close all

%% Test Data setup (Compas & Gyroscope)
nrOfSamples = 1000;
timeSpan = 10;
sigmaComp = 0.2;
sigmaGyro = 0.1;
timeVector = linspace(0,timeSpan,nrOfSamples);
SensorData = zeros(2,nrOfSamples);

% Create sensor data
%SensorData(2,:)=sin(pi*timeVector/timeSpan)+transpose(sigmaComp*randn(nrOfSamples,1));
%SensorData(1,:)=pi/timeSpan*cos(pi*timeVector/timeSpan)+transpose(sigmaGyro*randn(nrOfSamples,1));
% save('Kalman estimator/SensorData.mat','SensorData');
load('Kalman estimator/SensorData.mat');

% Plot sensor data
% figure
% plot(timeVector,SensorData(1,:),timeVector,SensorData(2,:))
% legend('Compas','Gyroscope')
% title('Simulated Sensor Values')

%% Setup state vector
% Initial estimage, State 1 is the angular force, state 2 is the rotational
% speed and state 4 is the rotation
stateVector_mes = zeros(3,nrOfSamples); % Measured state
stateVector_est = zeros(3,nrOfSamples); % Predicted state

rotationForce0=0;
rotationSpeed0=1;
rotation0=0;
stateVector_mes(:,1)=[rotationForce0,rotationSpeed0,rotation0];

%% Setup Matrices
deltaT = timeSpan/nrOfSamples;
A = [0,0,0;deltaT,1,0;0,deltaT,1]; % Dynamics, No drag (rotation damping) modelet
rotationSpeedProcessNoise = 0.01;
rotationProcessNoise = 0.0001;
Q = [0,0,0;0,rotationSpeedProcessNoise,0;0,0,rotationProcessNoise]; % Process noise
R = [sigmaGyro,0;0,sigmaComp]; % Sensor noise
K = zeros(3,2,nrOfSamples); % Gain matrix
P_mes = zeros(3,3,nrOfSamples); % Measured variance
P_est = zeros(3,3,nrOfSamples); % Estimated variance
H = [0,1,0;0,0,1]; % Sensor to State mapping matrix
% Setup initial 
initialForceVariance = 0;
initialSpeedVariance = 1;
initialRotationVariance = 1;
P_mes(:,:,1)=[initialForceVariance,0,0;0,initialSpeedVariance,0;0,0,initialRotationVariance];


%% Perform kalman state estimator

for i = 2:nrOfSamples
   % Prior update
   stateVector_est(:,i)=A*stateVector_mes(:,i-1); % Plus control estimation, not yet implementet
   P_est(:,:,i)=A*P_mes(:,:,i-1)*transpose(A)+Q;
   % Measurement update
   K(:,:,i)=P_est(:,:,i)*transpose(H)*inv(H*P_est(:,:,i)*transpose(H)+R);
   stateVector_mes(:,i)=stateVector_est(:,i)+K(:,:,i)*(SensorData(:,i)-H* stateVector_est(:,i));
   P_mes(:,:,i)=(eye(3)-K(:,:,i)*H)*P_est(:,:,i);
end

%% Plot
figure
plot(timeVector,SensorData(2,:),timeVector,stateVector_mes(3,:))
hold on
plot(timeVector,SensorData(1,:),timeVector,stateVector_mes(2,:))
legend('Compas sensor','Rotation Estimation','Gyroscope sensor','Rotation speed Estimation')
xlabel('Time [s]')
ylabel('Radian , Radian per second')
title('Kalman estimator')
