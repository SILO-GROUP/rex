# Rex

Rex executes chains of automations.  

Those automations are generally user-defined executables, represented in a 
project structure as json objects.

Rex, when executing, is supplied a config file and a plan.

## Config

The config file contains a json object that defines where the resources Rex
will operate with are located on the filesystem, and the general project 
structure for the plan being executed.

## Plans

A `plan` is central to the operation of rex.  It is a file containing a json
object that defines which `units` to execute from the `units library`.  Once
a unit is specified in a `plan`, it is referred to as a `task`.

It should be noted that the config file and the plan file are not required 
to be separate files, but this is generally easier to manage from a project
structure standpoint.

## Library / Units 

The `units library` is generally where these automations are defined.  This
is a directory containing files that end with the `.units` suffix.  Each of
those files is a json object with specifications for:

1. What is being executed.
2. How it is being executed.
3. The context around which it is executed.
4. An identifier by which the execution is referenced.

Exact specifications will be enumerated in a later section.

This allows these automations to executed with very tight control of the environment, 
current working directory, the execution process' owner and group context, whether a
pty is used etc.

Among other things, the path for the units library is defined in the config file 
supplied at the command line to rex.

For reasons later explained, the presence of a unit definition in the units library 
does NOT mean that rex will execute that unit.  It merely makes a unit available to
be included as a task in a plan.

## Self-Healing Workflows
Rex introduces self-healing workflows.

### Model A
1. Write a script that does a thing.
2. Write a script that checks if that thing is done.
3. Set up your check script as a target in a unit.
4. Set up your script that does the thing as that unit's rectifier.
5. Turn on the rectify pattern in the unit definition.

### Model B
Or, if you want a flow that's more simple:

1. Write a script that does a thing.
2. Write a script that fixes any environmental conditions that prevent that thing from being done.
3. Set up your target script to be the script that does the thing.
4. Set up your rectifier script to be the script that fixes the bad condition.
5. Turn on the rectify pattern in the unit definition.

## Traditional Workflows
You don't need a dual mode of automation (though it is highly recommended).  You can just do this:

1. Write a script that does a thing.
2. Set the user, group to run as, well as the shell that should be used to execute within.
3. Set the environment file to a file to be sourced for that shell containing all of your environment definitions for your automation.
4. Turn off the rectify pattern.
5. Repeat for every step.

As you can see, for small automations, it's often going to be more desirable to just write a shell script, but for very large efforts spanning many subsystems or components, you may want more control -- that's really where Rex comes in.