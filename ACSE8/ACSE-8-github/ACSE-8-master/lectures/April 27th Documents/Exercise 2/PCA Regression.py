"""
Calculate Regression lines, Orthogonal Regression line, and PCA on a 
two-dimensional dataset extracted from the Kaggle houseprices dataset.

Author: O. Dubrule
"""
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

from sklearn.linear_model  import LinearRegression
from sklearn.metrics       import r2_score
from sklearn.decomposition import PCA
"""
Read and produce statistics on the two variables extracted from the Housprices 
dataset
"""
houses = pd.read_csv('./Houseprices.csv')
print( houses ['SalePrice']  .describe())
print( houses ['GrLivArea']  .describe())
"""
Extract 1-D arrays for linear regression
"""
x       =   houses[['GrLivArea']].values
y       =   houses[['SalePrice']].values
"""
Create and scale 2-D array used for PCA and orthogonal regression
"""
xy      =   houses[['GrLivArea','SalePrice']].values
xy[:,0] =   xy[:,0]/1000.
xy[:,1] =   xy[:,1]/100000.

def regression (U,V):
    """
    Calculate regression coefficients theta0 and theta1 for Linear Regression 
    of array V agains array U
    """
    U2        = np.multiply(U,U)
    V2        = np.multiply(V,V)
    UV        = np.multiply(U,V)
    
    Umean     = np.mean(U)
    Vmean     = np.mean(V)
    U2mean    = np.mean (U2)
    UVmean    = np.mean (UV)
    
    Theta0    = (U2mean*Vmean-Umean*UVmean)/(U2mean-Umean*Umean)
    Theta1    = (UVmean      -Umean*Vmean )/(U2mean-Umean*Umean)
    
    return Theta0,Theta1

def orthogonal_regression (U,V):   
    """
    The input parameters are the two uncentered arrays U and V respectively 
    containg the x and y coordinates of the data points
    
    Start by centering the arrays
    """
    U         = U-np.mean(U)
    V         = V-np.mean(V)

    U2        = np.multiply(U,U)
    V2        = np.multiply(V,V)
    UV        = np.multiply(U,V)
    
    U2sum     = np.sum (U2)
    V2sum     = np.sum (V2)
    UVsum     = np.sum (UV)
    
    Term1     = V2sum-U2sum
    Term2     = Term1 * Term1
    Term3     = 4. * UVsum * UVsum
    Slope     = (Term1+np.sqrt(Term2+Term3))/(2.*UVsum)
   
    return Slope


"""
LINEAR REGRESSION: First we do y vs x

First approach (analytical)
"""
Theta0 , Theta1  = regression(x,y)
Thetap0, Thetap1 = regression(y,x)
print('\n'*3,'REGRESSION:','\n'*2, 'Theta0     : %.4f'% Theta0 , 'Theta1     : %.4f'
      % Theta1 )
print(        ' Thetaprime0: %.4f'% Thetap0, '  Thetaprime1: %.4f'% Thetap1)
"""
Second approach (sklearn): first we do y vs x
"""
regress  = LinearRegression(fit_intercept=True)

regress.fit (x, y)
y_       = regress.predict(x)
print('\n'*2, 'Regression Coefficients for y vs x: \n', regress.intercept_, regress.coef_)
print(' R2 Variance score: %.2f' % r2_score(y, y_))
"""
Second approach (sklearn): now  we do y vs x
"""
regress.fit (y, x)
x_     = regress.predict (y)
print('\n'*2, 'Regression Coefficients for x vs y: \n', regress.intercept_, regress.coef_)
print(' R2 Variance score: %.2f' % r2_score(x,x_))
"""
PRINCIPAL COMPONENTS ANALYSIS

First approach (analytical): calculate slope of line using orthogonal regression routine
"""
slope_ortho = orthogonal_regression(xy[:,0],xy[:,1])
"""
Second approach:  use sklearn's PCA to calculate slope of major axis
"""
pca    = PCA (n_components=1)
pca.fit  (xy) 
print    ('\n'*3, 'PCA:','\n'*2,'Explained Variance:', pca.explained_variance_ratio_)
print    ('\n'*2, 'Coordinates in Feature Space of the PCA Component:','\n',pca.components_)
"""
Calculate slope of PCA line and compare with slope of orthogonal regression
"""
slope_pca = pca.components_[0,1]/pca.components_[0,0]
print (' Slope from PCA:', slope_pca, ' Slope from orthogonal regression: ', slope_ortho)
"""
Now plot all the results
"""
plt.figure  (figsize=(10, 8))
"""
Plot the input data in red
"""
plt.scatter (x, y, s=20, color='red')
"""
Plot the two Regression Lines (y vs x and x vs y) in blue and black
"""
plt.plot    (x  , y_ , color='blue'   ,linewidth=3, label='Regression y vs x')
plt.plot    (x_ , y  , color='black'  ,linewidth=3, label='Regression x vs y')
"""
Correct for the slopes following the previous scaling of the input data
"""
slope_ortho = slope_ortho * 100.
slope_pca   = slope_pca   * 100.
"""
Plot PCA and Orthogonal Regression lines, which should overlap (we should only see the green
line as it is the  second one to be plotted)
"""
plt.plot    (x,((np.mean(y) - slope_pca  *np.mean(x)) + slope_pca  *x), color='cyan'    ,linewidth=3)
plt.plot    (x,((np.mean(y) - slope_ortho*np.mean(x)) + slope_ortho*x), color='green'   ,linewidth=3, label= 'PCA (or Othogonal Regression)')

plt.xlim(0.,6000.)
plt.ylim(0.,800000.)

plt.xlabel('Greater Living Area Above Ground')
plt.ylabel('House Sale Price')

plt.title ('Houses Sale Price: Compare Linear Regressions, Orthogonal Regression and PCA')

plt.legend()

plt.show()