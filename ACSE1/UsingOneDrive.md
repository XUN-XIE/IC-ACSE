# Comments on using the command line OneDrive client in Jupyterhub for ACSE-1

## TL;DR

Once you've authenticated onedrive (run ```onedrive```) it's highly recommended to set up some basic config so the service acts by default as a backup:

```
echo 'ACSE-1' > ~/.config/onedrive/sync_list
echo 'upload_only = "true"' > ~/.config/onedrive/config
echo 'no_remote_delete = "true"' >> ~/.config/onedrive/config
```

This will mean onedrive only synchronizes the ACSE-1 directory, only uploads files, and never deletes anything from your OneDrive.

## The onedrive config file

When you're working with OneDrive on Jupyterhub you may need to customise the way that the OneDrive client works.

Your primary method for configuration is to create a file in your home directory called:

```.config/onedrive/config```

Full documentation for this file is available at https://github.com/abraunegg/onedrive/blob/master/docs/USAGE.md ; the notes below are a subset of this file aimed to reduce the amount you need to read for configuring onedrive in class.

## Checking your config file

If you've edited ```.config/onedrive/config``` and want to check that it's still valid config syntax, run:

```onedrive --display-config```

This is also useful to check what the current configuration is.

## Run onedrive as a backup service

In ```.config/onedrive/config``` you can force onedrive to act as a push service, uploading only, and not deleting remote files, by adding the following configuration lines:

```
upload_only = "true"
no_remote_delete = "true"
```

This is particularly useful if you're wanting to make sure a mistake on your side doesn't delete (potentially lots of) files in your OneDrive.

Note that ```no_remote_delete``` is only honoured when ```upload_only``` is set ```true```.

## Only synchronize specified directories

You can create an additional file in your onedrive config directory called ```sync_list``` , ie:

```.config/onedrive/sync_list```

which specifies directories and files that you want to synchronize to Jupyterhub. For example, if you're working on ACSE-1 and also want to syncrhonize a file of notes you're taking you might specify file contents of:

```
ACSE-1
notes/ACSE/ACSE-1/notes.md
```

After changing the sync_list, you must perform a full re-synchronization by adding --resync to your existing command line - for example: 

```onedrive --synchronize --resync```

## Don't synchronize specified directories

Instead of explicitly specifying which directories you want to synchronize, you might want to synchronize everything *except* particular directories. There are two options to acheive this, one of which is to put the following configuration in ```.config/onedrive/config``` :

```check_nosync = "true"```

and then create an empty file named ```.nosync``` in each directory that you don't want onedrive to synchronize to Jupyterhub.

An alternative method is to specify the directories that you don't want to synchronize in a config entry. For example, if you don't want to synchronize your ```Photos/``` and ```Diary/``` directories, or any files ending in ```.data```, write a line in ```.config/onedrive/config``` containing:

```
skip_dir = "Photos | Diary"
skip_file = "*.data"
```

After changing ```skip_dir``` or ```skip_file```, you must perform a full re-synchronization by adding --resync to your existing command line - for example: 

```onedrive --synchronize --resync```

## My authentication broke and I need to repeat it

If you need to to do a re-authentication with the onedrive client, run:

```onedrive --logout```


