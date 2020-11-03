# ACSE 1 additional sessions, afternoons of 17 & 18 October 2019

Presenter: Tim Greaves
           tim.greaves@imperial.ac.uk
           https://www.imperial.ac.uk/people/tim.greaves

## Session 1 : DevOps : 17/Oct/2019 1400 RSM 1.47

### Put programming into context

- When there are multiple developers working on a codebase:
    - Agree on standards for code interfacing - don't break someone else's code
    - Avoiding duplication of effort through solving the same problem multiple times
    - Benefit from team wisdom - someone else might have solved this before
    - Make sure you have a clear definition of the desired end result

- Code has to interface with humans
    - Almost all software has human interaction where someone sees or uses the results; even technical examples which may never be run will be read
    - Someone will re-use your functions and modules, so document the interfaces
    - Other people will end up building your code - give them clear instructions
    - One day someone will have to fix your code, so make sure it's well documented/commented 

- Capturing user requirements
    - Understand what problem the user wants to solve and then tell them what you've understood, ideally in written form to get feedback    
    - Understand the user's level of technical understanding - are they technically expert or do they need software that assumes no technical knowledge
    - Understand the resources the user has to run the software - laptop or supercomputer
    - Understand the required code lifetime and write software accordingly, very long lifetimes needing resilience against significant hardware and supporting software changes


- Optimising deployment.
  - Understand operating system requirements and make sure you build and test on all required platforms 
  - Is the deployment system connected to the internet? Some aren't and need that to be replicated during testing 
  - Where possible, script your deployment to optimise for deployment on a large scale or deployment by untrained users
  - Consider how you would get users to re-deploy in the case of software failure

- Think about whether you are a 'quality', 'speed', or 'flexible' programmer
  - If you're passionate about code quality don't work somewhere that emphasises agility over quality
  - If you like to innovate fast, don't work somewhere that insists on all code being perfect

### Move development work into production

- When moving from development into production/release:
  - Keep track of user requirements for hard deadlines that software delivery has to meet
  - Don't underestimate the cost of human developer time
  - Get a clear understanding of real user urgencies and priorities as opposed to perceived urgencies
  - Identify real emergency issues which need quick responses and be prepared to move very quickly to address critical bugs
  - Be careful not to introduce potentially serious bugs when moving quickly.

### Methods

- Code management, codebase interactions, reviews
  - Github is a hosted platform where data lives off-premesis : https://github.com/
  - Github Enterprise : very similar functionality to github but on-prem : https://github.com/enterprise
  - Azure DevOps : Microsoft's comprehensive DevOps solution : https://dev.azure.com/
  - Launchpad : Code hosting with a focus on developing for Ubuntu : https://launchpad.net/

- Continuous building and testing
  - Travis - free and paid-for CI for Github-based projects : https://travis-ci.org/
  - Jenkins - very flexible self-hosted Java-based CI with an extensive range of plugins : https://jenkins.io/
  - Buildbot - more minimal self-hosted CI written in Python and highly configurable for Python programmers : https://buildbot.net/
  - Azure Pipelines - CI within Azure DevOps but builds from any code hosting platform : https://azure.microsoft.com/en-us/services/devops/pipelines/

### Ideas

- Keep everything in VCS: code, configuration, notes, presentations...
- Commit regularly, but in a manner that suits you
  - If you commit every small change you make, make a dev branch of your own and then 'squash' commits into coherent units of work before pushing back to team branches
  - If you tend to forget to commit regularly, make yourself commit every hour and repackage your commits later
  - If you prefer fewer commits, still make sure to commit every time you complete a set of mutually consistent changes

## Session 2 - Systems : 18/Oct/2019 1300 RSM 1.51

This session aims to give a brief introduction to interacting with systems, focusing on understanding what's happening when
systems don't work as expected or aren't set up as you need them to be.

Gaining a deep understanding of systems requires similar methods to gaining a deep understanding of programming - a lot of
practise. This is a significant time investment, and worth considering whether you will get enough benefit to make it
worthwhile. If you are expecting to spent most of your career working as a contractor responsible for your own systems, or
working in small start-up companies without dedicated systems support, you may well get a lot of benefit from improving your 
knowledge of systems. They are also particularly rewarding if you enjoy computing and like puzzles, as you can think of a 
system as a very complex puzzle which is often extremely satisfying to understand and solve.

Unless you are working with systems for fun you will rarely need to work with perfectly operational systems. It is normal to
need to work on a system because it has given you an error, or because it does not meet your requirements.

In the first part of this session we'll consider a method for dealing with errors. In the second part of the session we'll 
think about dealing with changing or deploying systems to meet your requirements.

### Problem 1 : I have an system error and I don't know what's causing it

When you encounter an error, try to understand whether the system is telling you:

- You do not have the correct permissions to do what you want to do
- Something is missing in the system
- The system does not have enough resources for what you want to do
- The system is disconnected from the network
- The system is off
- There is a problem with the system configuration

The order of problems in that list is deliberate, based on the likelihood of a problem occurring and the time it takes to 
diagnose each possible problem. We'll consider each problem in turn.

#### Permissions

Modern systems use a permissions system for security and to protect you against unintentional system changes. You will spend
most of your time working as a normal user. Users have permission to read most system files but not change them, generally 
have permission to read and write their own files, and may or may not have permission to read and write other users' files.

If your error message relates to a file or directory, check:

- Can I read the file? (Open it in a text editor, or use a command such as 'wc' on the command line)
- Can I write the file? (Try using 'touch' on the command line)
- Can I create files in the directory? (Again, try using 'touch' on the command line)
- Can I execute (run) the file? (Try running it directly from the command line)
- What do the permissions actually look like on the file?

The last point requires a little more knowledge about file permissions. If you're interested in learning about file 
permissions and how to manage them, try reading https://kb.iu.edu/d/abdb

To solve the problem, you will generally need to manually change permissions on a file or directory. You may be able to do
this yourself from your user account (see the above link to learn how to do this), you may need to change to an administrator
account if you have one, or you may need to ask a systems administrator to help you. Administrators generally have permission
to alter any user or system file, an ability which should be used with great care.

This discussion of permissions is quite Linux/OS X/UNIX specific; if you are using Windows, similar permissions structures
exist between users and administrators, and examining file permissions through the 'properties' of a file can be useful.

#### Something is missing on the system

You may have moved onto this step as a result of examining file permissions and discovering a file is missing. Alternatively,
the error message may have told you a file cannot be found. In either case, consider:

- Do I know the full path to the missing file?
- If you have the full path, is it inside your user filespace?
- Does the path or filename contain any reference to libraries?
- Does the path or filename contain any reference to packages (particularly python) ?
- Has anything happened since the command last worked, particularly removing files or packages or running an update?

Finding the full path to the missing file is often key to solving this problem. Sometimes the path itself doesn't exist, but
whilst investigating this you find a similar path which contains the file or directory that has been reported as missing, 
and you can move on to solving a configuration problem.

If there isn't an obvious configuration problem, consider whether the missing file is a user file or a system file. Look at
the full path and see whether it's inside a home directory (generally containing a username). If it is, is it inside your 
home directory or someone else's? If it's in someone else's, you may not have permission to access it, resulting in 'file not 
found' - in this case talk to the user who owns the file. If it's in your file space, move on to the last point and consider
whether you have removed or renamed any files since the command last worked. This may or may not be fixable...

If it doesn't contain any reference to usernames, assume it's a system file that is missing. Consider whether there are any
references to libraries. If there are, you may be missing a package (often a 'development' package, containing libraries
suitable for compiling and linking a program - search in your package manager for a package relevant to the file reported
as missing, or search on the web for one) or may be missing configuration to tell your command where a library is. You can
often diagnose the second case by searching the system for the missing library file and then solving a configuration problem
with the information you gain from the search.

If you have any references to packages (particularly python packages) in your error, follow a similar debugging method to the
library method.

If none of the above steps give useful information, consider:

- When did the command last run successfully?
- What has changed since then?

Often you will find evidence for the cause of the problem in this thought process. If you removed files, check recycle bins. 
If you removed or added packages, or ran system updates, you may need to reinstall packages or change configuration based on
new versions of software.

#### Insufficient resources

This is often a quick but useful debugging step. Check:

- Are any disks / volumes attached to the system full? (Use 'df')
- Is the system out of memory? (Use 'top')
- Are the CPUs overloaded? (Use 'top')

Use of 'df' is covered in detail at https://www.lifewire.com/check-disk-space-commands-df-du-2201111

Note that the additional steps described there are often the solution to your
problem, namely the 'du' command which allows you to diagnose where space has
been used and where you might need to compress / remove files.

Use of 'top' is covered in detail at https://www.lifewire.com/linux-top-command-2201163

On local systems you will generally have to remove files or kill processes to
solve your problem. On cloud systems you may be able to increase resources to
solve your problem. This is generally not straightforward, and unless you are
confident doing it you will want to ask an administrator for help.

#### The system is disconnected from the network

Test:

- Can I send and receive data? (use 'ping 8.8.8.8' to contact the google nameserver)
- Can I resolve domain names? (use 'host google.com' to check name resolution)

If your 'ping' command fails, check whether you can restart network connections yourself. Generally this is an issue users
have permission to solve through network settings.

If your 'ping' command succeeds and your 'host' command fails, your name service is badly configured. Sometimes restarting
your network connection will fix this; if not, ask an administrator for help.

#### The system is off

This is mostly a debugging step for use of remote systems. Don't discount the possibility that someone or something has 
turned off the system. It's generally a quick check and can save a lot of wasted time if it turns out to be the case.

#### The system is misconfigured

You may have reached this step through information from another earlier step, or reached it by default when none of the other
steps gave you a useful result. This is generally a step that is hard to give any advice on without knowing the specific 
error, but useful things to consider are:

- Have I loaded any custom configuration during my session?
- Have I changed/updated the system recently?

If you have done anything like activating a python environment or loading an environment module, try starting a new, clean
session and see if the error also happens there. If you have updated the system recently, think about whether there are any
major changes to system packages that might need configuration changes in your own files, particularly if your configuration
file explicitly defines a package / library / module version which might have changed.

### Changing or deploying systems

This is a detailed and complex subject of which we will only consider a few cases that you as a user might need to know:

- I need to run a different operating system
- I need to isolate or precisely define my software environment
- I've been asked to run a container
- I need more resources
- I've been asked to run in the cloud

#### Running a different operating system

You may be required to run some software which is incompatible with your system. Typically, this would be a linux-specific
software package when you have a Windows or OS X laptop. You have various options here, particularly:

- Run a virtual machine
- Use the Linux subsystem for Windows
- Run in the cloud

Virtual machines allow you to run an entirely one operating system inside another. If you have some administrator permissions
this can be done very efficiently with the virtual machine having direct access to your CPU. Useful resources are:

- https://www.vmware.com/ for Windows
- https://www.parallels.com/ for OS X

These are very user-friendly and will give you a straightforward way to run one operating system inside another.

Bear in mind that you may hit licensing problems if you are running Windows or OS X inside the virtualisation software.

You should have already come across the Linux subsystem for Windows from the first day of ACSE 1. This is a very clean method 
for running basic Linux programs, in particular where they are entirely command-line based.

If you have the resources to do so, consider running in the cloud. We will make use of Azure Labs in a later ACSE module which
is a managed example of this method.

#### Isolating or precisely defining your software environment

You may need to isolate your software. Typically this will be a result of:

- Needing to run untrusted software inside a 'sandbox'
- Needing to run software which requires system configuration changes, and you don't want to change your base system
- Needing to test software in a very controlled environment

All three cases here have similar solutions. One is to use the previous method of a virtual machine. Another is to use a
virtual environment -- you may have already come across this during ACSE 1. If not, you will encounter it in later modules.

Alternatively, you might use a 'container'. Containers differ from virtual machines in that they do not replicate the most
basic level of the operating system (the kernel), just the software environment, but they differ from virtual environments
in that they don't allow software to access the underlying system unless that has been explicitly allowed.

An excellent introduction to containers is through Docker (container management software) at https://docs.docker.com/get-started/ 

#### I've been asked to use a container

See discussion in the previous section.

#### I need more resources

Sometimes you need to run or build software with more resources than you currently have available. You might:

- Need more storage space for large datasets
- Need more memory for efficiently handling large problems
- Need more processing power to keep up with testing requirements
- Need redundant systems for reliability

You may have got to this point from a diagnosis of 'insufficient resources' as we discussed earlier in this session, or you
may be anticipating greater resource requirements (as happened earlier in this module for the dask session).

Options available to you include:

- Upgrading the hardware of your existing system
- Getting access to existing high specification systems
- Running on High Performance Computers
- Running on cloud resources

If you are not experienced with hardware upgrades consider asking a skilled technician to help. However, any reasonably
careful and meticulous person should be able to upgrade hardware and there are normally very detailed tutorials online to
help you with this.

Many businesses and institutions have large computers which spend much of their time unused. Balancing against the cost of
buying new computing hardware or cloud services, a small amount of your time is often worth spending to find out whether 
there are unused servers you could make use of. Someone else may already have done the research for you; if not, make sure
to record your findings for your own or others' use in future.

Many technical businesses and universities have high performance computers available for use. It is worth spending some time
considering whether this solution is a good one for you *before* you start programming as if you plan to use HPC you will
probably want to invest significant development effort in making your software run efficiently in parallel. This may well be 
a requirement for you to demonstrate before getting access to HPC systems. Expect that you will need to:

- Demonstrate good scaling in parallel to prove you will make good use of HPC
- If you expect to run for a long time, make sure your software checkpoints so it can pick up from failures
- Install your software with no assumptions of a Linux/UNIX distribution
- Be very tolerant of unusual supporting software locations and configurations
- Get detailed debugging output to diagnose unusual problems

Additionally, if your software requires a lot of unusual supporting software packages, you may benefit from writing an
automated install method that assumes no more than 'basic Linux support' and builds all required packages from source.

Recent advances in cloud computing have made it a very realistic alternative for almost all resourcing problems. The primary
considerations are:

- Cost (economic)
- Network connectivity to your physical location
- Any requirements to keep code / data on-prem

We make extensive use of cloud resources for running the ACSE course, basing our resources on Microsoft Azure. Other options
you might consider are AWS (Amazon) and Google Cloud. 

For anyone interested in learning more about cloud services, your Imperial account will sign you on to Azure, and you should
receive £150 free credit for learning. A good starting point is https://azure.microsoft.com/en-us/get-started/

As a beginner, wherever possible try to deploy the most basic resources to minimise your spend, and at the end of every 
session on Azure make sure that any resources which can be turned off are turned off so as to save credit. This is a very
good practise to get into for general cloud use.

