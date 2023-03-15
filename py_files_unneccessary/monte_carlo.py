import numpy as np
import matplotlib.pyplot as plt

# Define simulation parameters
n_trials = 10000  # Number of trials
n_hits = 0  # Number of hits inside circle

# Perform simulation
for i in range(n_trials):
    x, y = np.random.uniform(-1, 1, 2)  # Generate random point inside square
    if x ** 2 + y ** 2 <= 1:  # Check if point is inside circle
        n_hits += 1  # Increment hit counter

# Calculate pi estimate
pi_estimate = 4 * n_hits / n_trials

# Print result
print(f"Pi estimate: {pi_estimate:.4f}")

# Plot results
x = np.linspace(-1, 1, 100)
y = np.linspace(-1, 1, 100)
X, Y = np.meshgrid(x, y)
Z = X ** 2 + Y ** 2 <= 1
plt.contourf(X, Y, Z, levels=[0, 1], colors=['lightgray', 'blue'])
plt.gca().set_aspect('equal')
plt.show()
