import numpy as np
import pandas as pd 
"""
Creates flood_probability with another column for squares of values
"""


df = pd.read_csv("flood_probability.csv", index_col=0)

# add columns (to be used for get_easting_northing_flood_probability_band)
# for radius squared and eastings/northings max/min: 
df["radius_squared"] = np.square(df["radius"])
df["X_max"] = df["X"]+df["radius"]
df["X_min"] = df["X"]-df["radius"]
df["Y_max"] = df["Y"]+df["radius"]
df["Y_min"] = df["Y"]-df["radius"]


df.to_csv(r'flood_probability.csv')