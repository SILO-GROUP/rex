# Config

The Rex configuration is as a json object in a file that Rex is pointed to as a commandline argument.

In that file's json object, there is a field named "config", whose 
properties define the configuration of Rex.

There are currently 4 parameters that can be configured in the configuration file:

1. `project_root`: The root directory of the project.
2. `units_path`: The path to the units Library.
3. `logs_path`: The path to the logs directory.
4. `config_version`: The version of the configuration file.

## Example

```json
{
  "config": {
    "project_root": "/home/user/project",
    "units_path": "/home/user/project/units",
    "logs_path": "/home/user/project/logs",
    "config_version": 1
  }
}
```

There are some things to be aware of when setting the configuration file:

1. The configuration file must be a valid json object.
2. The configuration file must have a field named "config", whose properties define the configuration of Rex.
3. All values for paths in this file are relative to the `project_root` path.
4. The `logs_path` location will be created if it does not exist when Rex begins to execute.