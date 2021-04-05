# Rex
The Meta-Automation system you didn't know you were missing.  It's elegant, yet crude, it's advanced, yet simple.

## What is Rex?
Rex is a project-based, json-driven execution flow/workflow tool designed to place rails around very complex automations in a controlled way that
is easy to troubleshoot.

While designed for the generation of SURRO Linux, the design was kept broad for many other use cases and extensions.

At a high level, it is a very simple thing:  It executes scripts and other executables in a predetermined order, logs 
their output, and has basic error handling using exit codes of the executables it is running.

Rex relies on a library of `Units` which are files that define, in json format, the executables it will execute.  

Rex uses a `Plan` to define which of those units it will actually execute -- once selected they are called `Tasks`.  

This allows you to have many things defined by multiple teams, and, with sufficient abstraction, use the same library of
automations for multiple purposes throughout your environment -- bring shared library patterns in software development
to your infrastructure operations efforts if you so desire, and more.

# Instructions
These are instructions for some primitive ways of using Rex.

## Build
Compiling Rex is easy.  There are zero external dependencies.  Build does require *cmake*.

~~~~
$ cmake .
$ make
~~~~

Then place the binary where you'd like.  I'd recommend packaging it for your favorite Linux distribution.

## High Level Usage

### Self-Healing Workflows

Rex introduces self-healing workflows.

#### Model A
1.  Write a script that does a thing.
2.  Write a script that checks if that thing is done.
3.  Set up your check script as a `target` in a `unit`.
4.  Set up your script that does the thing as that unit's `rectifier`.
5.  Turn on the `rectify` pattern in the `unit definition`.

#### Model B
Or, if you want a flow that's more simple:

1. Write a script that does a thing.
2. Write a script that fixes any environmental conditions that prevent that thing from being done.
3. Set up your `target` script to be the script that does the thing.
4. Set up your `rectifier` script to be the script that fixes the bad condition.
5. Turn on the `rectify pattern` in the unit definition.

### Traditional Workflows
In fact, you don't need a dual mode of automation (though it is highly recommended):

1. Write a script that does a thing.
2. Set the `user`, `group` to run as, well as the `shell` that should be used to execute within.
3. Set the `environment file` to a file to be sourced for that shell containing all of your environment definitions for 
   your automation.
4. Turn off the `rectify` pattern.   
5. Repeat for every step.

As you can see, for small automations, it's often going to be more desirable to just write a shell script, but for
very large efforts spanning many subsystems or components, you may want more control -- that's really where Rex comes 
in.

## Definitions
So you've got Rex compiled and you're ready to start automating the world.

If you're thinking "how do I configure this thing", this README is for you.

### Units
A Unit is an automation definition, written in JSON in a UNIT FILE.  Deeper into Rex’s internals, Units and Tasks have slightly different functions, but for the purposes of users, the terms can be used interchangeably.  A Task is a task to be performed in a Plan, and a Unit is its definition.  A Unit is a JSON object that has:

* A `name`, which is an identifier for the Unit used by people.
* A `target`, which is the path to the automation script performing the work.  This provides a clean linear path for huge chains of scripts to be executed in order and tracked on return for additional logic in chaining.
* A `rectifier`, which is the path to the automation script to be executed if the target call fails.
* An `active` attribute,which tells Rex whether or not the Unit can be used in a Plan.  This gives Unit developers a way to tell Plan developers not to use the Unit.
* A `required` attribute which tells Rex whether or not the Plan can continue if the Unit fails.  If the rectify attribute is set to true, this attribute is checked after a rectifier failure.  If not, this is checked after target failure.  In either case, if the rectifier or target do not return successfully, Rex will halt the execution of the Plan if this is turned on for the unit being executed.  Otherwise it simply moves to the next Unit being executed.
* A `log` attribute which tells Rex whether or not to log the stdout of the task.  STDERR will always be logged regardless.
* A `user` attribute, along with its accompanying `group` attribute, which together set the identity context to execute the script as that user.
* A `rectify` attribute, which tells Rex whether or not to execute the rectifier in the case of failure when executing the target.
* An `environment` attribute, which points to the path of an environment file -- usually a shell script to be sourced to populate the environment executing the `target`.

### Tasks
A `Task` is an action item in a `Plan`, just like in real life.  In the context of Rex, a `Task` is a `Unit` that has been loaded and incorporated into a `Plan` in an actionable state.  Inactive `Units` can not be loaded into a `Plan` and thus can never be a `Task`.  The primary difference between a Task and a Unit is that a Unit is not actionable — it’s just a definition — while a Task is a consumable, actionable automation definition that is scheduled to execute.

### Suite
A `Suite` is not visible to the user and this is only for informational purposes.  A Suite is a collection of all available Unit definitions loaded from one or more UNIT FILES.  Just as a Unit is the definition for a Task, a Suite is a collection of Units that define the Task components of a Plan.

A Suite is consumed by a Plan during the conversion of Units to Tasks, though this is not visible to the user — it just simply helps to understand the kind of abstraction taking place in the conceptual model of Rex.
Plan

A Plan is the glue of all the components of Rex and is deceptively simple.  A Plan loads a Suite for its Task definitions (Units), but the Tasks to actually execute are specified in the PLAN FILE.  The Tasks are executed in the order specified in the PLAN FILE.

### FILES
There are several types of files used by a Rex project.

#### CONFIG FILE and Attributes
This is the one config file that Rex uses.  The default path it looks is /etc/Rex/config.json.

A config file at the time of writing this specifies a single JSON object with 5 attributes:

  * `units_path`: The `UNIT FILE` path or a path to a directory containing unit files.
  * `config_version`: The configuration VERSION.
  * `execution_context`:  The current working directory to use when loading unit files, plan files, or executing Tasks.
  * `execution_context_override`: A boolean indicating whether or not the execution context should be set, or left alone.  It is highly recommended to set this to `true`.
  * `logs_path`:  The path relative to the execution context to store logs.  This directory will be created if it does not exist.

#### Configuration VERSION
The configuration version is checked to ensure that the configuration is consumable by that version of Rex.  This will pave the way for reverse compatibility if the project moves in that direction.

#### UNIT FILE

The UNIT FILE is a specification of where the Units are defined.  All UNIT FILES in that directory will be amalgamated to generate the Suite.  These types of files must end in `*.units` for their filename.

#### PLAN FILE

The PLAN FILE is a specification of the order that Tasks are executed, and their dependencies upon each other.  Dependency implementation is a touchy matter that is pending implementation, so, mileage may vary until release.



## I still don't see how this works.
That's ok.  It's in its infancy so we're always looking for ways to make it simpler.  Here's a 'hello world' example.

### 1.  Write your tests.
First, we want to know all the things we need to be able to print "hello world" to the screen.  In this case we just need to make we have the "echo" binary.

Write a bash script that checks if the "echo" binary is on the system.

	#!/usr/bin/bash
	stat /usr/bin/echo
	exit $?

Save it as ~/check-echo.bash.
This script will be your "target" attribute for your "hello world" unit definition.

### 2. Write your automation.
Write a "hello world" script.

	#!/usr/bin/bash
	echo "hello world"
	exit $?

Save it as ~/hello.bash
This script will be your "rectify" attribute for your "hello world" unit definition.

### 3. Set up the Unit file.
At this point you've got both the script that checks if hello world can run and you've got your hello world script.  Time to set up the unit.

### 4. Add the Unit definition to the Plan.
Next, add the unit to the plan by name.

### 5. Set up your config file.
Point your config file at your plan file and your units directory.

### 6.  Run Rex pointing at that config file.
Execute rex:

```
rex --config path/to/your/config/file.json --plan path/to/your/plan/file.json
```

And you should see your 'hello world' script.  Check out the `test/` directory in this repo for an example project for 
more details.


