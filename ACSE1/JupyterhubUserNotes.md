A suggested standard workflow for cluster users:

* Connect to https://acse-jhub.westeurope.cloudapp.azure.com/
* Click on 'Sign in with Github'
* Proceed through the Github signin process, allowing access when requested
* Wait for your session to launch, which takes on the order of a minute
* Open a terminal window (right hand side drop down 'New' menu -> Terminal)
* Start Onedrive authentication by typing

```onedrive```

* Copy and paste the URL this gives you into a browser window
* When requested, log in using your college credentials
* You will get a blank browser page in return; copy and paste the URL of this page back into the Jupyterhub terminal where it requests a return URL
* If you have any large directories you don't want to synchronize to Jupyterhub, you can exclude them by creating a configuration file in your jupyterhub account named ".config/onedrive/config" containing a line such as the following, which would omit synchronizing the 'data' and 'logfiles' directories.

```skip_dir = "data | logfiles"```

* More details on configuration can be found at https://github.com/abraunegg/onedrive/blob/master/docs/USAGE.md
* Synchronize your OneDrive account to Jupyterhub with:

```onedrive --synchronize```

* If you accidentally linked to the wrong account you can reset the local OneDrive configuration by removing the synchronized directory and the configuration directory:

```rm -rf ~/.config/onedrive ~/OneDrive```

* Start the OneDrive client in monitoring mode with:

```onedrive -m &```

* Change into the OneDrive directory:

```cd OneDrive```

* Clone the class repository:

```git clone https://github.com/acse-2019/ACSE-1.git```

* Trust any python notebooks:

```ipython trust ACSE-1/lectures/*/*.ipynb```
