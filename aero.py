import numpy as np
import matplotlib.pyplot as plt

# Define simulation parameters
theta0 = np.pi / 4  # Initial angle of pendulum (radians)
omega0 = 0  # Initial angular velocity (radians/second)
L = 1  # Length of pendulum (meters)
g = 9.81  # Acceleration due to gravity (meters/second^2)
dt = 0.01  # Time step (seconds)
tmax = 10  # Maximum simulation time (seconds)

# Initialize arrays to store simulation results
t = np.arange(0, tmax, dt)  # Time array
theta = np.zeros_like(t)  # Angular displacement array
omega = np.zeros_like(t)  # Angular velocity array

# Set initial conditions
theta[0] = theta0
omega[0] = omega0

# Perform simulation using Euler-Cromer method
for i in range(1, len(t)):
    alpha = -g / L * np.sin(theta[i - 1])  # Angular acceleration
    omega[i] = omega[i - 1] + alpha * dt  # Update angular velocity
    theta[i] = theta[i - 1] + omega[i] * dt  # Update angular displacement

# Plot results
plt.plot(t, theta)
plt.xlabel('Time (s)')
plt.ylabel('Angle (rad)')
plt.show()
