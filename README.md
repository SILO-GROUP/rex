# Instructions
These are instructions for using Examplar.

## Build

Compiling Examplar is easy.  There are zero external dependencies.  Build does require *cmake*.

~~~~
$ cmake .
$ make
~~~~

Then place the binary where you'd like.  I'd recommend packaging it for your favorite Linux distribution.

## High Level Usage

1.  Write a script that does a thing.
2.  Write a script that checks if that thing is done.
3.  Set up your check as a target in a unit.
4.  Set up your script as its rectifier.
5.  Turn on the rectify pattern in the unit definition.

## Definitions
So you've got Examplar compiled and you're ready to start automating the world.

If you're thinking "how do I configure this thing", this article is for you.

### Units

A Unit is an automation definition, written in JSON in a UNIT FILE.  Deeper into Examplar’s internals, Units and Tasks have slightly different functions, but for the purposes of users, the terms can be used interchangeably.  A Task is a task to be performed in a Plan, and a Unit is its definition.  A Unit is a JSON object that has:

  * A `name`, which is an identifier for the Unit used by people.
  * A `target`, which is the path to the automation script performing the work.  This provides a clean linear path for huge chains of scripts to be executed in order and tracked on return for additional logic in chaining.
  * A `rectifier`, which is the path to the automation script to be executed if the target call fails.
  * A `rectify` attribute, which tells Examplar whether or not to execute the rectifier in the case of failure when executing the target.
  * An `active` attribute,which tells Examplar whether or not the Unit can be used in a Plan.  This gives Unit developers a way to tell Plan developers not to use the Unit.
  * A `required` attribute which tells Examplar whether or not the Plan can continue if the Unit fails.  If the rectify attribute is set to true, this attribute is checked after a rectifier failure.  If not, this is checked after target failure.  In either case, if the rectifier or target do not return successfully, Examplar will halt the execution of the Plan if this is turned on for the unit being executed.  Otherwise it simply moves to the next Unit being executed.

### Tasks

A Task is an action item in a Plan, just like in real life.  In the context of Examplar, a Task is a Unit that has been loaded and incorporated into a Plan in an actionable state.  Inactive Units can not be loaded into a Plan and thus can never be a Task.  The primary difference between a Task and a Unit is that a Unit is not actionable — it’s just a definition — while a Task a consumable, actionable automation.
Suite

A Suite is not visible to the user and this is only for informational purposes.  A Suite is a collection of all available Unit definitions loaded from one or more UNIT FILES.  Just as a Unit is the definition for a Task, a Suite is a collection of Units that define the Task components of a Plan.

A Suite is consumed by a Plan during the conversion of Units to Tasks, though this is not visible to the user — it just simply helps to understand the kind of abstraction taking place in the conceptual model of Examplar.
Plan

A Plan is the glue of all the components of Examplar and is deceptively simple.  A Plan loads a Suite for its Task definitions (Units), but the Tasks to actually execute are specified in the PLAN FILE.  The Tasks are executed in the order specified in the PLAN FILE.

### FILES
There are several files used by Examplar.

#### CONFIG FILE and Attributes

This is the one config file that Examplar uses.  The default path it looks is /etc/Examplar/config.json.

A config file at the time of writing this specifies a single JSON object with 5 attributes:

  * `units_path`: The `UNIT FILE` path or a path to a directory containing unit files.
  * `plan_path`: The PLAN FILE path.  There is only ever one plan executed in a single run.
  * `config_version`: The configuration VERSION.
  * `execution_context`:  The current working directory to use when loading unit files, plan files, or executing Tasks.
  * `execution_context_override`: A boolean indicating whether or not the execution context should be set, or left alone.  It is highly recommended to set this to `true`.

#### Configuration VERSION

The configuration version is checked to ensure that the configuration is consumable by that version of Examplar.  This will pave the way for reverse compatibility if the project moves in that direction.


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

### 6.  Run Examplar pointing at that config file.
Execute examplar:

examplar --verbose --config path/to/your/config/file.json

And you should see your 'hello world' script.

